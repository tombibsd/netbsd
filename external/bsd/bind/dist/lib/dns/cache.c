/*	$NetBSD$	*/

/*
 * Copyright (C) 2004-2009, 2011-2013, 2015, 2016  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2003  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* Id: cache.c,v 1.91 2011/08/26 05:12:56 marka Exp  */

/*! \file */

#include <config.h>

#include <isc/json.h>
#include <isc/mem.h>
#include <isc/print.h>
#include <isc/string.h>
#include <isc/stats.h>
#include <isc/task.h>
#include <isc/time.h>
#include <isc/timer.h>
#include <isc/util.h>
#include <isc/xml.h>

#include <dns/cache.h>
#include <dns/db.h>
#include <dns/dbiterator.h>
#include <dns/events.h>
#include <dns/lib.h>
#include <dns/log.h>
#include <dns/masterdump.h>
#include <dns/rdata.h>
#include <dns/rdataset.h>
#include <dns/rdatasetiter.h>
#include <dns/result.h>
#include <dns/stats.h>

#include "rbtdb.h"

#define CACHE_MAGIC		ISC_MAGIC('$', '$', '$', '$')
#define VALID_CACHE(cache)	ISC_MAGIC_VALID(cache, CACHE_MAGIC)

/*!
 * Control incremental cleaning.
 * DNS_CACHE_MINSIZE is how many bytes is the floor for dns_cache_setcachesize().
 * See also DNS_CACHE_CLEANERINCREMENT
 */
#define DNS_CACHE_MINSIZE	2097152U /*%< Bytes.  2097152 = 2 MB */
/*!
 * Control incremental cleaning.
 * CLEANERINCREMENT is how many nodes are examined in one pass.
 * See also DNS_CACHE_MINSIZE
 */
#define DNS_CACHE_CLEANERINCREMENT	1000U	/*%< Number of nodes. */

/***
 ***	Types
 ***/

/*
 * A cache_cleaner_t encapsulates the state of the periodic
 * cache cleaning.
 */

typedef struct cache_cleaner cache_cleaner_t;

typedef enum {
	cleaner_s_idle,	/*%< Waiting for cleaning-interval to expire. */
	cleaner_s_busy,	/*%< Currently cleaning. */
	cleaner_s_done	/*%< Freed enough memory after being overmem. */
} cleaner_state_t;

/*
 * Convenience macros for comprehensive assertion checking.
 */
#define CLEANER_IDLE(c) ((c)->state == cleaner_s_idle && \
			 (c)->resched_event != NULL)
#define CLEANER_BUSY(c) ((c)->state == cleaner_s_busy && \
			 (c)->iterator != NULL && \
			 (c)->resched_event == NULL)

/*%
 * Accesses to a cache cleaner object are synchronized through
 * task/event serialization, or locked from the cache object.
 */
struct cache_cleaner {
	isc_mutex_t	lock;
	/*%<
	 * Locks overmem_event, overmem.  Note: never allocate memory
	 * while holding this lock - that could lead to deadlock since
	 * the lock is take by water() which is called from the memory
	 * allocator.
	 */

	dns_cache_t	*cache;
	isc_task_t	*task;
	unsigned int	cleaning_interval; /*% The cleaning-interval from
					      named.conf, in seconds. */
	isc_timer_t	*cleaning_timer;
	isc_event_t	*resched_event;	/*% Sent by cleaner task to
					   itself to reschedule */
	isc_event_t	*overmem_event;

	dns_dbiterator_t *iterator;
	unsigned int	increment;	/*% Number of names to
					   clean in one increment */
	cleaner_state_t	state;		/*% Idle/Busy. */
	isc_boolean_t	overmem;	/*% The cache is in an overmem state. */
	isc_boolean_t	 replaceiterator;
};

/*%
 * The actual cache object.
 */

struct dns_cache {
	/* Unlocked. */
	unsigned int		magic;
	isc_mutex_t		lock;
	isc_mutex_t		filelock;
	isc_mem_t		*mctx;		/* Main cache memory */
	isc_mem_t		*hmctx;		/* Heap memory */
	char			*name;

	/* Locked by 'lock'. */
	int			references;
	int			live_tasks;
	dns_rdataclass_t	rdclass;
	dns_db_t		*db;
	cache_cleaner_t		cleaner;
	char			*db_type;
	int			db_argc;
	char			**db_argv;
	size_t			size;
	isc_stats_t		*stats;

	/* Locked by 'filelock'. */
	char			*filename;
	/* Access to the on-disk cache file is also locked by 'filelock'. */
};

/***
 ***	Functions
 ***/

static isc_result_t
cache_cleaner_init(dns_cache_t *cache, isc_taskmgr_t *taskmgr,
		   isc_timermgr_t *timermgr, cache_cleaner_t *cleaner);

static void
cleaning_timer_action(isc_task_t *task, isc_event_t *event);

static void
incremental_cleaning_action(isc_task_t *task, isc_event_t *event);

static void
cleaner_shutdown_action(isc_task_t *task, isc_event_t *event);

static void
overmem_cleaning_action(isc_task_t *task, isc_event_t *event);

static inline isc_result_t
cache_create_db(dns_cache_t *cache, dns_db_t **db) {
	return (dns_db_create(cache->mctx, cache->db_type, dns_rootname,
			      dns_dbtype_cache, cache->rdclass,
			      cache->db_argc, cache->db_argv, db));
}

isc_result_t
dns_cache_create(isc_mem_t *cmctx, isc_taskmgr_t *taskmgr,
		 isc_timermgr_t *timermgr, dns_rdataclass_t rdclass,
		 const char *db_type, unsigned int db_argc, char **db_argv,
		 dns_cache_t **cachep)
{
	return (dns_cache_create3(cmctx, cmctx, taskmgr, timermgr, rdclass, "",
				  db_type, db_argc, db_argv, cachep));
}

isc_result_t
dns_cache_create2(isc_mem_t *cmctx, isc_taskmgr_t *taskmgr,
		  isc_timermgr_t *timermgr, dns_rdataclass_t rdclass,
		  const char *cachename, const char *db_type,
		  unsigned int db_argc, char **db_argv, dns_cache_t **cachep)
{
	return (dns_cache_create3(cmctx, cmctx, taskmgr, timermgr, rdclass,
				  cachename, db_type, db_argc, db_argv,
				  cachep));
}

isc_result_t
dns_cache_create3(isc_mem_t *cmctx, isc_mem_t *hmctx, isc_taskmgr_t *taskmgr,
		  isc_timermgr_t *timermgr, dns_rdataclass_t rdclass,
		  const char *cachename, const char *db_type,
		  unsigned int db_argc, char **db_argv, dns_cache_t **cachep)
{
	isc_result_t result;
	dns_cache_t *cache;
	int i, extra = 0;
	isc_task_t *dbtask;

	REQUIRE(cachep != NULL);
	REQUIRE(*cachep == NULL);
	REQUIRE(cmctx != NULL);
	REQUIRE(hmctx != NULL);
	REQUIRE(cachename != NULL);

	cache = isc_mem_get(cmctx, sizeof(*cache));
	if (cache == NULL)
		return (ISC_R_NOMEMORY);

	cache->mctx = cache->hmctx = NULL;
	isc_mem_attach(cmctx, &cache->mctx);
	isc_mem_attach(hmctx, &cache->hmctx);

	cache->name = NULL;
	if (cachename != NULL) {
		cache->name = isc_mem_strdup(cmctx, cachename);
		if (cache->name == NULL) {
			result = ISC_R_NOMEMORY;
			goto cleanup_mem;
		}
	}

	result = isc_mutex_init(&cache->lock);
	if (result != ISC_R_SUCCESS)
		goto cleanup_mem;

	result = isc_mutex_init(&cache->filelock);
	if (result != ISC_R_SUCCESS)
		goto cleanup_lock;

	cache->references = 1;
	cache->live_tasks = 0;
	cache->rdclass = rdclass;

	cache->stats = NULL;
	result = isc_stats_create(cmctx, &cache->stats,
				  dns_cachestatscounter_max);
	if (result != ISC_R_SUCCESS)
		goto cleanup_filelock;

	cache->db_type = isc_mem_strdup(cmctx, db_type);
	if (cache->db_type == NULL) {
		result = ISC_R_NOMEMORY;
		goto cleanup_stats;
	}

	/*
	 * For databases of type "rbt" we pass hmctx to dns_db_create()
	 * via cache->db_argv, followed by the rest of the arguments in
	 * db_argv (of which there really shouldn't be any).
	 */
	if (strcmp(cache->db_type, "rbt") == 0)
		extra = 1;

	cache->db_argc = db_argc + extra;
	cache->db_argv = NULL;

	if (cache->db_argc != 0) {
		cache->db_argv = isc_mem_get(cmctx,
					     cache->db_argc * sizeof(char *));
		if (cache->db_argv == NULL) {
			result = ISC_R_NOMEMORY;
			goto cleanup_dbtype;
		}

		for (i = 0; i < cache->db_argc; i++)
			cache->db_argv[i] = NULL;

		cache->db_argv[0] = (char *) hmctx;
		for (i = extra; i < cache->db_argc; i++) {
			cache->db_argv[i] = isc_mem_strdup(cmctx,
							   db_argv[i - extra]);
			if (cache->db_argv[i] == NULL) {
				result = ISC_R_NOMEMORY;
				goto cleanup_dbargv;
			}
		}
	}

	/*
	 * Create the database
	 */
	cache->db = NULL;
	result = cache_create_db(cache, &cache->db);
	if (result != ISC_R_SUCCESS)
		goto cleanup_dbargv;
	if (taskmgr != NULL) {
		dbtask = NULL;
		result = isc_task_create(taskmgr, 1, &dbtask);
		if (result != ISC_R_SUCCESS)
			goto cleanup_db;
		dns_db_settask(cache->db, dbtask);
		isc_task_detach(&dbtask);
	}

	cache->filename = NULL;

	cache->magic = CACHE_MAGIC;

	/*
	 * RBT-type cache DB has its own mechanism of cache cleaning and doesn't
	 * need the control of the generic cleaner.
	 */
	if (strcmp(db_type, "rbt") == 0)
		result = cache_cleaner_init(cache, NULL, NULL, &cache->cleaner);
	else {
		result = cache_cleaner_init(cache, taskmgr, timermgr,
					    &cache->cleaner);
	}
	if (result != ISC_R_SUCCESS)
		goto cleanup_db;

	result = dns_db_setcachestats(cache->db, cache->stats);
	if (result != ISC_R_SUCCESS)
		goto cleanup_db;


	*cachep = cache;
	return (ISC_R_SUCCESS);

 cleanup_db:
	dns_db_detach(&cache->db);
 cleanup_dbargv:
	for (i = extra; i < cache->db_argc; i++)
		if (cache->db_argv[i] != NULL)
			isc_mem_free(cmctx, cache->db_argv[i]);
	if (cache->db_argv != NULL)
		isc_mem_put(cmctx, cache->db_argv,
			    cache->db_argc * sizeof(char *));
 cleanup_dbtype:
	isc_mem_free(cmctx, cache->db_type);
 cleanup_filelock:
	DESTROYLOCK(&cache->filelock);
 cleanup_stats:
	isc_stats_detach(&cache->stats);
 cleanup_lock:
	DESTROYLOCK(&cache->lock);
 cleanup_mem:
	if (cache->name != NULL)
		isc_mem_free(cmctx, cache->name);
	isc_mem_detach(&cache->hmctx);
	isc_mem_putanddetach(&cache->mctx, cache, sizeof(*cache));
	return (result);
}

static void
cache_free(dns_cache_t *cache) {
	int i;

	REQUIRE(VALID_CACHE(cache));
	REQUIRE(cache->references == 0);

	isc_mem_setwater(cache->mctx, NULL, NULL, 0, 0);

	if (cache->cleaner.task != NULL)
		isc_task_detach(&cache->cleaner.task);

	if (cache->cleaner.overmem_event != NULL)
		isc_event_free(&cache->cleaner.overmem_event);

	if (cache->cleaner.resched_event != NULL)
		isc_event_free(&cache->cleaner.resched_event);

	if (cache->cleaner.iterator != NULL)
		dns_dbiterator_destroy(&cache->cleaner.iterator);

	DESTROYLOCK(&cache->cleaner.lock);

	if (cache->filename) {
		isc_mem_free(cache->mctx, cache->filename);
		cache->filename = NULL;
	}

	if (cache->db != NULL)
		dns_db_detach(&cache->db);

	if (cache->db_argv != NULL) {
		/*
		 * We don't free db_argv[0] in "rbt" cache databases
		 * as it's a pointer to hmctx
		 */
		int extra = 0;
		if (strcmp(cache->db_type, "rbt") == 0)
			extra = 1;
		for (i = extra; i < cache->db_argc; i++)
			if (cache->db_argv[i] != NULL)
				isc_mem_free(cache->mctx, cache->db_argv[i]);
		isc_mem_put(cache->mctx, cache->db_argv,
			    cache->db_argc * sizeof(char *));
	}

	if (cache->db_type != NULL)
		isc_mem_free(cache->mctx, cache->db_type);

	if (cache->name != NULL)
		isc_mem_free(cache->mctx, cache->name);

	if (cache->stats != NULL)
		isc_stats_detach(&cache->stats);

	DESTROYLOCK(&cache->lock);
	DESTROYLOCK(&cache->filelock);

	cache->magic = 0;
	isc_mem_detach(&cache->hmctx);
	isc_mem_putanddetach(&cache->mctx, cache, sizeof(*cache));
}


void
dns_cache_attach(dns_cache_t *cache, dns_cache_t **targetp) {

	REQUIRE(VALID_CACHE(cache));
	REQUIRE(targetp != NULL && *targetp == NULL);

	LOCK(&cache->lock);
	cache->references++;
	UNLOCK(&cache->lock);

	*targetp = cache;
}

void
dns_cache_detach(dns_cache_t **cachep) {
	dns_cache_t *cache;
	isc_boolean_t free_cache = ISC_FALSE;

	REQUIRE(cachep != NULL);
	cache = *cachep;
	REQUIRE(VALID_CACHE(cache));

	LOCK(&cache->lock);
	REQUIRE(cache->references > 0);
	cache->references--;
	if (cache->references == 0) {
		cache->cleaner.overmem = ISC_FALSE;
		free_cache = ISC_TRUE;
	}

	*cachep = NULL;

	if (free_cache) {
		/*
		 * When the cache is shut down, dump it to a file if one is
		 * specified.
		 */
		isc_result_t result = dns_cache_dump(cache);
		if (result != ISC_R_SUCCESS)
			isc_log_write(dns_lctx, DNS_LOGCATEGORY_DATABASE,
				      DNS_LOGMODULE_CACHE, ISC_LOG_WARNING,
				      "error dumping cache: %s ",
				      isc_result_totext(result));

		/*
		 * If the cleaner task exists, let it free the cache.
		 */
		if (cache->live_tasks > 0) {
			isc_task_shutdown(cache->cleaner.task);
			free_cache = ISC_FALSE;
		}
	}

	UNLOCK(&cache->lock);

	if (free_cache)
		cache_free(cache);
}

void
dns_cache_attachdb(dns_cache_t *cache, dns_db_t **dbp) {
	REQUIRE(VALID_CACHE(cache));
	REQUIRE(dbp != NULL && *dbp == NULL);
	REQUIRE(cache->db != NULL);

	LOCK(&cache->lock);
	dns_db_attach(cache->db, dbp);
	UNLOCK(&cache->lock);

}

isc_result_t
dns_cache_setfilename(dns_cache_t *cache, const char *filename) {
	char *newname;

	REQUIRE(VALID_CACHE(cache));
	REQUIRE(filename != NULL);

	newname = isc_mem_strdup(cache->mctx, filename);
	if (newname == NULL)
		return (ISC_R_NOMEMORY);

	LOCK(&cache->filelock);
	if (cache->filename)
		isc_mem_free(cache->mctx, cache->filename);
	cache->filename = newname;
	UNLOCK(&cache->filelock);

	return (ISC_R_SUCCESS);
}

isc_result_t
dns_cache_load(dns_cache_t *cache) {
	isc_result_t result;

	REQUIRE(VALID_CACHE(cache));

	if (cache->filename == NULL)
		return (ISC_R_SUCCESS);

	LOCK(&cache->filelock);
	result = dns_db_load(cache->db, cache->filename);
	UNLOCK(&cache->filelock);

	return (result);
}

isc_result_t
dns_cache_dump(dns_cache_t *cache) {
	isc_result_t result;

	REQUIRE(VALID_CACHE(cache));

	if (cache->filename == NULL)
		return (ISC_R_SUCCESS);

	LOCK(&cache->filelock);
	result = dns_master_dump(cache->mctx, cache->db, NULL,
				 &dns_master_style_cache, cache->filename);
	UNLOCK(&cache->filelock);
	return (result);

}

void
dns_cache_setcleaninginterval(dns_cache_t *cache, unsigned int t) {
	isc_interval_t interval;
	isc_result_t result;

	LOCK(&cache->lock);

	/*
	 * It may be the case that the cache has already shut down.
	 * If so, it has no timer.
	 */
	if (cache->cleaner.cleaning_timer == NULL)
		goto unlock;

	cache->cleaner.cleaning_interval = t;

	if (t == 0) {
		result = isc_timer_reset(cache->cleaner.cleaning_timer,
					 isc_timertype_inactive,
					 NULL, NULL, ISC_TRUE);
	} else {
		isc_interval_set(&interval, cache->cleaner.cleaning_interval,
				 0);
		result = isc_timer_reset(cache->cleaner.cleaning_timer,
					 isc_timertype_ticker,
					 NULL, &interval, ISC_FALSE);
	}
	if (result != ISC_R_SUCCESS)
		isc_log_write(dns_lctx, DNS_LOGCATEGORY_DATABASE,
			      DNS_LOGMODULE_CACHE, ISC_LOG_WARNING,
			      "could not set cache cleaning interval: %s",
			      isc_result_totext(result));

 unlock:
	UNLOCK(&cache->lock);
}

unsigned int
dns_cache_getcleaninginterval(dns_cache_t *cache) {
	unsigned int t;

	REQUIRE(VALID_CACHE(cache));

	LOCK(&cache->lock);
	t = cache->cleaner.cleaning_interval;
	UNLOCK(&cache->lock);

	return (t);
}

const char *
dns_cache_getname(dns_cache_t *cache) {
	REQUIRE(VALID_CACHE(cache));

	return (cache->name);
}

/*
 * Initialize the cache cleaner object at *cleaner.
 * Space for the object must be allocated by the caller.
 */

static isc_result_t
cache_cleaner_init(dns_cache_t *cache, isc_taskmgr_t *taskmgr,
		   isc_timermgr_t *timermgr, cache_cleaner_t *cleaner)
{
	isc_result_t result;

	result = isc_mutex_init(&cleaner->lock);
	if (result != ISC_R_SUCCESS)
		goto fail;

	cleaner->increment = DNS_CACHE_CLEANERINCREMENT;
	cleaner->state = cleaner_s_idle;
	cleaner->cache = cache;
	cleaner->iterator = NULL;
	cleaner->overmem = ISC_FALSE;
	cleaner->replaceiterator = ISC_FALSE;

	cleaner->task = NULL;
	cleaner->cleaning_timer = NULL;
	cleaner->resched_event = NULL;
	cleaner->overmem_event = NULL;
	cleaner->cleaning_interval = 0; /* Initially turned off. */

	result = dns_db_createiterator(cleaner->cache->db, ISC_FALSE,
				       &cleaner->iterator);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	if (taskmgr != NULL && timermgr != NULL) {
		result = isc_task_create(taskmgr, 1, &cleaner->task);
		if (result != ISC_R_SUCCESS) {
			UNEXPECTED_ERROR(__FILE__, __LINE__,
					 "isc_task_create() failed: %s",
					 dns_result_totext(result));
			result = ISC_R_UNEXPECTED;
			goto cleanup;
		}
		cleaner->cache->live_tasks++;
		isc_task_setname(cleaner->task, "cachecleaner", cleaner);

		result = isc_task_onshutdown(cleaner->task,
					     cleaner_shutdown_action, cache);
		if (result != ISC_R_SUCCESS) {
			UNEXPECTED_ERROR(__FILE__, __LINE__,
					 "cache cleaner: "
					 "isc_task_onshutdown() failed: %s",
					 dns_result_totext(result));
			goto cleanup;
		}

		result = isc_timer_create(timermgr, isc_timertype_inactive,
					   NULL, NULL, cleaner->task,
					   cleaning_timer_action, cleaner,
					   &cleaner->cleaning_timer);
		if (result != ISC_R_SUCCESS) {
			UNEXPECTED_ERROR(__FILE__, __LINE__,
					 "isc_timer_create() failed: %s",
					 dns_result_totext(result));
			result = ISC_R_UNEXPECTED;
			goto cleanup;
		}

		cleaner->resched_event =
			isc_event_allocate(cache->mctx, cleaner,
					   DNS_EVENT_CACHECLEAN,
					   incremental_cleaning_action,
					   cleaner, sizeof(isc_event_t));
		if (cleaner->resched_event == NULL) {
			result = ISC_R_NOMEMORY;
			goto cleanup;
		}

		cleaner->overmem_event =
			isc_event_allocate(cache->mctx, cleaner,
					   DNS_EVENT_CACHEOVERMEM,
					   overmem_cleaning_action,
					   cleaner, sizeof(isc_event_t));
		if (cleaner->overmem_event == NULL) {
			result = ISC_R_NOMEMORY;
			goto cleanup;
		}
	}

	return (ISC_R_SUCCESS);

 cleanup:
	if (cleaner->overmem_event != NULL)
		isc_event_free(&cleaner->overmem_event);
	if (cleaner->resched_event != NULL)
		isc_event_free(&cleaner->resched_event);
	if (cleaner->cleaning_timer != NULL)
		isc_timer_detach(&cleaner->cleaning_timer);
	if (cleaner->task != NULL)
		isc_task_detach(&cleaner->task);
	if (cleaner->iterator != NULL)
		dns_dbiterator_destroy(&cleaner->iterator);
	DESTROYLOCK(&cleaner->lock);
 fail:
	return (result);
}

static void
begin_cleaning(cache_cleaner_t *cleaner) {
	isc_result_t result = ISC_R_SUCCESS;

	REQUIRE(CLEANER_IDLE(cleaner));

	/*
	 * Create an iterator, if it does not already exist, and
	 * position it at the beginning of the cache.
	 */
	if (cleaner->iterator == NULL)
		result = dns_db_createiterator(cleaner->cache->db, ISC_FALSE,
					       &cleaner->iterator);
	if (result != ISC_R_SUCCESS)
		isc_log_write(dns_lctx, DNS_LOGCATEGORY_DATABASE,
			      DNS_LOGMODULE_CACHE, ISC_LOG_WARNING,
			      "cache cleaner could not create "
			      "iterator: %s", isc_result_totext(result));
	else {
		dns_dbiterator_setcleanmode(cleaner->iterator, ISC_TRUE);
		result = dns_dbiterator_first(cleaner->iterator);
	}
	if (result != ISC_R_SUCCESS) {
		/*
		 * If the result is ISC_R_NOMORE, the database is empty,
		 * so there is nothing to be cleaned.
		 */
		if (result != ISC_R_NOMORE && cleaner->iterator != NULL) {
			UNEXPECTED_ERROR(__FILE__, __LINE__,
					 "cache cleaner: "
					 "dns_dbiterator_first() failed: %s",
					 dns_result_totext(result));
			dns_dbiterator_destroy(&cleaner->iterator);
		} else if (cleaner->iterator != NULL) {
			result = dns_dbiterator_pause(cleaner->iterator);
			RUNTIME_CHECK(result == ISC_R_SUCCESS);
		}
	} else {
		/*
		 * Pause the iterator to free its lock.
		 */
		result = dns_dbiterator_pause(cleaner->iterator);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);

		isc_log_write(dns_lctx, DNS_LOGCATEGORY_DATABASE,
			      DNS_LOGMODULE_CACHE, ISC_LOG_DEBUG(1),
			      "begin cache cleaning, mem inuse %lu",
			    (unsigned long)isc_mem_inuse(cleaner->cache->mctx));
		cleaner->state = cleaner_s_busy;
		isc_task_send(cleaner->task, &cleaner->resched_event);
	}

	return;
}

static void
end_cleaning(cache_cleaner_t *cleaner, isc_event_t *event) {
	isc_result_t result;

	REQUIRE(CLEANER_BUSY(cleaner));
	REQUIRE(event != NULL);

	result = dns_dbiterator_pause(cleaner->iterator);
	if (result != ISC_R_SUCCESS)
		dns_dbiterator_destroy(&cleaner->iterator);

	dns_cache_setcleaninginterval(cleaner->cache,
				      cleaner->cleaning_interval);

	isc_log_write(dns_lctx, DNS_LOGCATEGORY_DATABASE, DNS_LOGMODULE_CACHE,
		      ISC_LOG_DEBUG(1), "end cache cleaning, mem inuse %lu",
		      (unsigned long)isc_mem_inuse(cleaner->cache->mctx));

	cleaner->state = cleaner_s_idle;
	cleaner->resched_event = event;
}

/*
 * This is run once for every cache-cleaning-interval as defined in named.conf.
 */
static void
cleaning_timer_action(isc_task_t *task, isc_event_t *event) {
	cache_cleaner_t *cleaner = event->ev_arg;

	UNUSED(task);

	INSIST(task == cleaner->task);
	INSIST(event->ev_type == ISC_TIMEREVENT_TICK);

	isc_log_write(dns_lctx, DNS_LOGCATEGORY_DATABASE, DNS_LOGMODULE_CACHE,
		      ISC_LOG_DEBUG(1), "cache cleaning timer fired, "
		      "cleaner state = %d", cleaner->state);

	if (cleaner->state == cleaner_s_idle)
		begin_cleaning(cleaner);

	isc_event_free(&event);
}

/*
 * This is called when the cache either surpasses its upper limit
 * or shrinks beyond its lower limit.
 */
static void
overmem_cleaning_action(isc_task_t *task, isc_event_t *event) {
	cache_cleaner_t *cleaner = event->ev_arg;
	isc_boolean_t want_cleaning = ISC_FALSE;

	UNUSED(task);

	INSIST(task == cleaner->task);
	INSIST(event->ev_type == DNS_EVENT_CACHEOVERMEM);
	INSIST(cleaner->overmem_event == NULL);

	isc_log_write(dns_lctx, DNS_LOGCATEGORY_DATABASE, DNS_LOGMODULE_CACHE,
		      ISC_LOG_DEBUG(1), "overmem_cleaning_action called, "
		      "overmem = %d, state = %d", cleaner->overmem,
		      cleaner->state);

	LOCK(&cleaner->lock);

	if (cleaner->overmem) {
		if (cleaner->state == cleaner_s_idle)
			want_cleaning = ISC_TRUE;
	} else {
		if (cleaner->state == cleaner_s_busy)
			/*
			 * end_cleaning() can't be called here because
			 * then both cleaner->overmem_event and
			 * cleaner->resched_event will point to this
			 * event.  Set the state to done, and then
			 * when the incremental_cleaning_action() event
			 * is posted, it will handle the end_cleaning.
			 */
			cleaner->state = cleaner_s_done;
	}

	cleaner->overmem_event = event;

	UNLOCK(&cleaner->lock);

	if (want_cleaning)
		begin_cleaning(cleaner);
}

/*
 * Do incremental cleaning.
 */
static void
incremental_cleaning_action(isc_task_t *task, isc_event_t *event) {
	cache_cleaner_t *cleaner = event->ev_arg;
	isc_result_t result;
	unsigned int n_names;
	isc_time_t start;

	UNUSED(task);

	INSIST(task == cleaner->task);
	INSIST(event->ev_type == DNS_EVENT_CACHECLEAN);

	if (cleaner->state == cleaner_s_done) {
		cleaner->state = cleaner_s_busy;
		end_cleaning(cleaner, event);
		LOCK(&cleaner->cache->lock);
		LOCK(&cleaner->lock);
		if (cleaner->replaceiterator) {
			dns_dbiterator_destroy(&cleaner->iterator);
			(void) dns_db_createiterator(cleaner->cache->db,
						     ISC_FALSE,
						     &cleaner->iterator);
			cleaner->replaceiterator = ISC_FALSE;
		}
		UNLOCK(&cleaner->lock);
		UNLOCK(&cleaner->cache->lock);
		return;
	}

	INSIST(CLEANER_BUSY(cleaner));

	n_names = cleaner->increment;

	REQUIRE(DNS_DBITERATOR_VALID(cleaner->iterator));

	isc_time_now(&start);
	while (n_names-- > 0) {
		dns_dbnode_t *node = NULL;

		result = dns_dbiterator_current(cleaner->iterator, &node,
						NULL);
		if (result != ISC_R_SUCCESS) {
			UNEXPECTED_ERROR(__FILE__, __LINE__,
				 "cache cleaner: dns_dbiterator_current() "
				 "failed: %s", dns_result_totext(result));

			end_cleaning(cleaner, event);
			return;
		}

		/*
		 * The node was not needed, but was required by
		 * dns_dbiterator_current().  Give up its reference.
		 */
		dns_db_detachnode(cleaner->cache->db, &node);

		/*
		 * Step to the next node.
		 */
		result = dns_dbiterator_next(cleaner->iterator);

		if (result != ISC_R_SUCCESS) {
			/*
			 * Either the end was reached (ISC_R_NOMORE) or
			 * some error was signaled.  If the cache is still
			 * overmem and no error was encountered,
			 * keep trying to clean it, otherwise stop cleaning.
			 */
			if (result != ISC_R_NOMORE)
				UNEXPECTED_ERROR(__FILE__, __LINE__,
						 "cache cleaner: "
						 "dns_dbiterator_next() "
						 "failed: %s",
						 dns_result_totext(result));
			else if (cleaner->overmem) {
				result = dns_dbiterator_first(cleaner->
							      iterator);
				if (result == ISC_R_SUCCESS) {
					isc_log_write(dns_lctx,
						      DNS_LOGCATEGORY_DATABASE,
						      DNS_LOGMODULE_CACHE,
						      ISC_LOG_DEBUG(1),
						      "cache cleaner: "
						      "still overmem, "
						      "reset and try again");
					continue;
				}
			}

			end_cleaning(cleaner, event);
			return;
		}
	}

	/*
	 * We have successfully performed a cleaning increment but have
	 * not gone through the entire cache.  Free the iterator locks
	 * and reschedule another batch.  If it fails, just try to continue
	 * anyway.
	 */
	result = dns_dbiterator_pause(cleaner->iterator);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);

	isc_log_write(dns_lctx, DNS_LOGCATEGORY_DATABASE, DNS_LOGMODULE_CACHE,
		      ISC_LOG_DEBUG(1), "cache cleaner: checked %u nodes, "
		      "mem inuse %lu, sleeping", cleaner->increment,
		      (unsigned long)isc_mem_inuse(cleaner->cache->mctx));

	isc_task_send(task, &event);
	INSIST(CLEANER_BUSY(cleaner));
	return;
}

/*
 * Do immediate cleaning.
 */
isc_result_t
dns_cache_clean(dns_cache_t *cache, isc_stdtime_t now) {
	isc_result_t result;
	dns_dbiterator_t *iterator = NULL;

	REQUIRE(VALID_CACHE(cache));

	result = dns_db_createiterator(cache->db, 0, &iterator);
	if (result != ISC_R_SUCCESS)
		return result;

	result = dns_dbiterator_first(iterator);

	while (result == ISC_R_SUCCESS) {
		dns_dbnode_t *node = NULL;
		result = dns_dbiterator_current(iterator, &node,
						(dns_name_t *)NULL);
		if (result != ISC_R_SUCCESS)
			break;

		/*
		 * Check TTLs, mark expired rdatasets stale.
		 */
		result = dns_db_expirenode(cache->db, node, now);
		if (result != ISC_R_SUCCESS) {
			UNEXPECTED_ERROR(__FILE__, __LINE__,
					 "cache cleaner: dns_db_expirenode() "
					 "failed: %s",
					 dns_result_totext(result));
			/*
			 * Continue anyway.
			 */
		}

		/*
		 * This is where the actual freeing takes place.
		 */
		dns_db_detachnode(cache->db, &node);

		result = dns_dbiterator_next(iterator);
	}

	dns_dbiterator_destroy(&iterator);

	if (result == ISC_R_NOMORE)
		result = ISC_R_SUCCESS;

	return (result);
}

static void
water(void *arg, int mark) {
	dns_cache_t *cache = arg;
	isc_boolean_t overmem = ISC_TF(mark == ISC_MEM_HIWATER);

	REQUIRE(VALID_CACHE(cache));

	LOCK(&cache->cleaner.lock);

	if (overmem != cache->cleaner.overmem) {
		dns_db_overmem(cache->db, overmem);
		cache->cleaner.overmem = overmem;
		isc_mem_waterack(cache->mctx, mark);
	}

	if (cache->cleaner.overmem_event != NULL)
		isc_task_send(cache->cleaner.task,
			      &cache->cleaner.overmem_event);

	UNLOCK(&cache->cleaner.lock);
}

void
dns_cache_setcachesize(dns_cache_t *cache, size_t size) {
	size_t hiwater, lowater;

	REQUIRE(VALID_CACHE(cache));

	/*
	 * Impose a minimum cache size; pathological things happen if there
	 * is too little room.
	 */
	if (size != 0U && size < DNS_CACHE_MINSIZE)
		size = DNS_CACHE_MINSIZE;

	LOCK(&cache->lock);
	cache->size = size;
	UNLOCK(&cache->lock);

	hiwater = size - (size >> 3);	/* Approximately 7/8ths. */
	lowater = size - (size >> 2);	/* Approximately 3/4ths. */

	/*
	 * If the cache was overmem and cleaning, but now with the new limits
	 * it is no longer in an overmem condition, then the next
	 * isc_mem_put for cache memory will do the right thing and trigger
	 * water().
	 */

	if (size == 0U || hiwater == 0U || lowater == 0U)
		/*
		 * Disable cache memory limiting.
		 */
		isc_mem_setwater(cache->mctx, water, cache, 0, 0);
	else
		/*
		 * Establish new cache memory limits (either for the first
		 * time, or replacing other limits).
		 */
		isc_mem_setwater(cache->mctx, water, cache, hiwater, lowater);
}

size_t
dns_cache_getcachesize(dns_cache_t *cache) {
	size_t size;

	REQUIRE(VALID_CACHE(cache));

	LOCK(&cache->lock);
	size = cache->size;
	UNLOCK(&cache->lock);

	return (size);
}

/*
 * The cleaner task is shutting down; do the necessary cleanup.
 */
static void
cleaner_shutdown_action(isc_task_t *task, isc_event_t *event) {
	dns_cache_t *cache = event->ev_arg;
	isc_boolean_t should_free = ISC_FALSE;

	UNUSED(task);

	INSIST(task == cache->cleaner.task);
	INSIST(event->ev_type == ISC_TASKEVENT_SHUTDOWN);

	if (CLEANER_BUSY(&cache->cleaner))
		end_cleaning(&cache->cleaner, event);
	else
		isc_event_free(&event);

	LOCK(&cache->lock);

	cache->live_tasks--;
	INSIST(cache->live_tasks == 0);

	if (cache->references == 0)
		should_free = ISC_TRUE;

	/*
	 * By detaching the timer in the context of its task,
	 * we are guaranteed that there will be no further timer
	 * events.
	 */
	if (cache->cleaner.cleaning_timer != NULL)
		isc_timer_detach(&cache->cleaner.cleaning_timer);

	/* Make sure we don't reschedule anymore. */
	(void)isc_task_purge(task, NULL, DNS_EVENT_CACHECLEAN, NULL);

	UNLOCK(&cache->lock);

	if (should_free)
		cache_free(cache);
}

isc_result_t
dns_cache_flush(dns_cache_t *cache) {
	dns_db_t *db = NULL, *olddb;
	dns_dbiterator_t *dbiterator = NULL, *olddbiterator = NULL;
	isc_result_t result;

	result = cache_create_db(cache, &db);
	if (result != ISC_R_SUCCESS)
		return (result);

	result = dns_db_createiterator(db, ISC_FALSE, &dbiterator);
	if (result != ISC_R_SUCCESS) {
		dns_db_detach(&db);
		return (result);
	}

	LOCK(&cache->lock);
	LOCK(&cache->cleaner.lock);
	if (cache->cleaner.state == cleaner_s_idle) {
		olddbiterator = cache->cleaner.iterator;
		cache->cleaner.iterator = dbiterator;
		dbiterator = NULL;
	} else {
		if (cache->cleaner.state == cleaner_s_busy)
			cache->cleaner.state = cleaner_s_done;
		cache->cleaner.replaceiterator = ISC_TRUE;
	}
	olddb = cache->db;
	cache->db = db;
	dns_db_setcachestats(cache->db, cache->stats);
	UNLOCK(&cache->cleaner.lock);
	UNLOCK(&cache->lock);

	if (dbiterator != NULL)
		dns_dbiterator_destroy(&dbiterator);
	if (olddbiterator != NULL)
		dns_dbiterator_destroy(&olddbiterator);
	dns_db_detach(&olddb);

	return (ISC_R_SUCCESS);
}

static isc_result_t
clearnode(dns_db_t *db, dns_dbnode_t *node) {
	isc_result_t result;
	dns_rdatasetiter_t *iter = NULL;

	result = dns_db_allrdatasets(db, node, NULL, (isc_stdtime_t)0, &iter);
	if (result != ISC_R_SUCCESS)
		return (result);

	for (result = dns_rdatasetiter_first(iter);
	     result == ISC_R_SUCCESS;
	     result = dns_rdatasetiter_next(iter))
	{
		dns_rdataset_t rdataset;
		dns_rdataset_init(&rdataset);

		dns_rdatasetiter_current(iter, &rdataset);
		result = dns_db_deleterdataset(db, node, NULL,
					       rdataset.type, rdataset.covers);
		dns_rdataset_disassociate(&rdataset);
		if (result != ISC_R_SUCCESS && result != DNS_R_UNCHANGED)
			break;
	}

	if (result == ISC_R_NOMORE)
		result = ISC_R_SUCCESS;

	dns_rdatasetiter_destroy(&iter);
	return (result);
}

static isc_result_t
cleartree(dns_db_t *db, dns_name_t *name) {
	isc_result_t result, answer = ISC_R_SUCCESS;
	dns_dbiterator_t *iter = NULL;
	dns_dbnode_t *node = NULL, *top = NULL;
	dns_fixedname_t fnodename;
	dns_name_t *nodename;

	/*
	 * Create the node if it doesn't exist so dns_dbiterator_seek()
	 * can find it.  We will continue even if this fails.
	 */
	(void)dns_db_findnode(db, name, ISC_TRUE, &top);

	dns_fixedname_init(&fnodename);
	nodename = dns_fixedname_name(&fnodename);

	result = dns_db_createiterator(db, 0, &iter);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	result = dns_dbiterator_seek(iter, name);
	if (result == DNS_R_PARTIALMATCH)
		result = dns_dbiterator_next(iter);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	while (result == ISC_R_SUCCESS) {
		result = dns_dbiterator_current(iter, &node, nodename);
		if (result == DNS_R_NEWORIGIN)
			result = ISC_R_SUCCESS;
		if (result != ISC_R_SUCCESS)
			goto cleanup;
		/*
		 * Are we done?
		 */
		if (! dns_name_issubdomain(nodename, name))
			goto cleanup;

		/*
		 * If clearnode fails record and move onto the next node.
		 */
		result = clearnode(db, node);
		if (result != ISC_R_SUCCESS && answer == ISC_R_SUCCESS)
			answer = result;
		dns_db_detachnode(db, &node);
		result = dns_dbiterator_next(iter);
	}

 cleanup:
	if (result == ISC_R_NOMORE || result == ISC_R_NOTFOUND)
		result = ISC_R_SUCCESS;
	if (result != ISC_R_SUCCESS && answer == ISC_R_SUCCESS)
		answer = result;
	if (node != NULL)
		dns_db_detachnode(db, &node);
	if (iter != NULL)
		dns_dbiterator_destroy(&iter);
	if (top != NULL)
		dns_db_detachnode(db, &top);

	return (answer);
}

isc_result_t
dns_cache_flushname(dns_cache_t *cache, dns_name_t *name) {
	return (dns_cache_flushnode(cache, name, ISC_FALSE));
}

isc_result_t
dns_cache_flushnode(dns_cache_t *cache, dns_name_t *name,
		    isc_boolean_t tree)
{
	isc_result_t result;
	dns_dbnode_t *node = NULL;
	dns_db_t *db = NULL;

	if (tree && dns_name_equal(name, dns_rootname))
		return (dns_cache_flush(cache));

	LOCK(&cache->lock);
	if (cache->db != NULL)
		dns_db_attach(cache->db, &db);
	UNLOCK(&cache->lock);
	if (db == NULL)
		return (ISC_R_SUCCESS);

	if (tree) {
		result = cleartree(cache->db, name);
	} else {
		result = dns_db_findnode(cache->db, name, ISC_FALSE, &node);
		if (result == ISC_R_NOTFOUND) {
			result = ISC_R_SUCCESS;
			goto cleanup_db;
		}
		if (result != ISC_R_SUCCESS)
			goto cleanup_db;
		result = clearnode(cache->db, node);
		dns_db_detachnode(cache->db, &node);
	}

 cleanup_db:
	dns_db_detach(&db);
	return (result);
}

isc_stats_t *
dns_cache_getstats(dns_cache_t *cache) {
	REQUIRE(VALID_CACHE(cache));
	return (cache->stats);
}

void
dns_cache_updatestats(dns_cache_t *cache, isc_result_t result) {
	REQUIRE(VALID_CACHE(cache));
	if (cache->stats == NULL)
		return;

	switch (result) {
	case ISC_R_SUCCESS:
	case DNS_R_NCACHENXDOMAIN:
	case DNS_R_NCACHENXRRSET:
	case DNS_R_CNAME:
	case DNS_R_DNAME:
	case DNS_R_GLUE:
	case DNS_R_ZONECUT:
		isc_stats_increment(cache->stats,
				    dns_cachestatscounter_queryhits);
		break;
	default:
		isc_stats_increment(cache->stats,
				    dns_cachestatscounter_querymisses);
	}
}

/*
 * XXX: Much of the following code has been copied in from statschannel.c.
 * We should refactor this into a generic function in stats.c that can be
 * called from both places.
 */
typedef struct
cache_dumparg {
	isc_statsformat_t	type;
	void			*arg;		/* type dependent argument */
	int			ncounters;	/* for general statistics */
	int			*counterindices; /* for general statistics */
	isc_uint64_t		*countervalues;	 /* for general statistics */
	isc_result_t		result;
} cache_dumparg_t;

static void
getcounter(isc_statscounter_t counter, isc_uint64_t val, void *arg) {
	cache_dumparg_t *dumparg = arg;

	REQUIRE(counter < dumparg->ncounters);
	dumparg->countervalues[counter] = val;
}

static void
getcounters(isc_stats_t *stats, isc_statsformat_t type, int ncounters,
	    int *indices, isc_uint64_t *values)
{
	cache_dumparg_t dumparg;

	memset(values, 0, sizeof(values[0]) * ncounters);

	dumparg.type = type;
	dumparg.ncounters = ncounters;
	dumparg.counterindices = indices;
	dumparg.countervalues = values;

	isc_stats_dump(stats, getcounter, &dumparg, ISC_STATSDUMP_VERBOSE);
}

void
dns_cache_dumpstats(dns_cache_t *cache, FILE *fp) {
	int indices[dns_cachestatscounter_max];
	isc_uint64_t values[dns_cachestatscounter_max];

	REQUIRE(VALID_CACHE(cache));

	getcounters(cache->stats, isc_statsformat_file,
		    dns_cachestatscounter_max, indices, values);

	fprintf(fp, "%20" ISC_PRINT_QUADFORMAT "u %s\n",
		values[dns_cachestatscounter_hits],
		"cache hits");
	fprintf(fp, "%20" ISC_PRINT_QUADFORMAT "u %s\n",
		values[dns_cachestatscounter_misses],
		"cache misses");
	fprintf(fp, "%20" ISC_PRINT_QUADFORMAT "u %s\n",
		values[dns_cachestatscounter_queryhits],
		"cache hits (from query)");
	fprintf(fp, "%20" ISC_PRINT_QUADFORMAT "u %s\n",
		values[dns_cachestatscounter_querymisses],
		"cache misses (from query)");
	fprintf(fp, "%20" ISC_PRINT_QUADFORMAT "u %s\n",
		values[dns_cachestatscounter_deletelru],
		"cache records deleted due to memory exhaustion");
	fprintf(fp, "%20" ISC_PRINT_QUADFORMAT "u %s\n",
		values[dns_cachestatscounter_deletettl],
		"cache records deleted due to TTL expiration");
	fprintf(fp, "%20u %s\n", dns_db_nodecount(cache->db),
		"cache database nodes");
	fprintf(fp, "%20" ISC_PLATFORM_QUADFORMAT "u %s\n",
		(isc_uint64_t) dns_db_hashsize(cache->db),
		"cache database hash buckets");

	fprintf(fp, "%20u %s\n", (unsigned int) isc_mem_total(cache->mctx),
		"cache tree memory total");
	fprintf(fp, "%20u %s\n", (unsigned int) isc_mem_inuse(cache->mctx),
		"cache tree memory in use");
	fprintf(fp, "%20u %s\n", (unsigned int) isc_mem_maxinuse(cache->mctx),
		"cache tree highest memory in use");

	fprintf(fp, "%20u %s\n", (unsigned int) isc_mem_total(cache->hmctx),
		"cache heap memory total");
	fprintf(fp, "%20u %s\n", (unsigned int) isc_mem_inuse(cache->hmctx),
		"cache heap memory in use");
	fprintf(fp, "%20u %s\n", (unsigned int) isc_mem_maxinuse(cache->hmctx),
		"cache heap highest memory in use");
}

#ifdef HAVE_LIBXML2
#define TRY0(a) do { xmlrc = (a); if (xmlrc < 0) goto error; } while(/*CONSTCOND*/0)
static int
renderstat(const char *name, isc_uint64_t value, xmlTextWriterPtr writer) {
	int xmlrc;

	TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "counter"));
	TRY0(xmlTextWriterWriteAttribute(writer,
					 ISC_XMLCHAR "name", ISC_XMLCHAR name));
	TRY0(xmlTextWriterWriteFormatString(writer,
					    "%" ISC_PRINT_QUADFORMAT "u",
					    value));
	TRY0(xmlTextWriterEndElement(writer)); /* counter */

error:
	return (xmlrc);
}

int
dns_cache_renderxml(dns_cache_t *cache, xmlTextWriterPtr writer) {
	int indices[dns_cachestatscounter_max];
	isc_uint64_t values[dns_cachestatscounter_max];
	int xmlrc;

	REQUIRE(VALID_CACHE(cache));

	getcounters(cache->stats, isc_statsformat_file,
		    dns_cachestatscounter_max, indices, values);
	TRY0(renderstat("CacheHits",
		   values[dns_cachestatscounter_hits], writer));
	TRY0(renderstat("CacheMisses",
		   values[dns_cachestatscounter_misses], writer));
	TRY0(renderstat("QueryHits",
		   values[dns_cachestatscounter_queryhits], writer));
	TRY0(renderstat("QueryMisses",
		   values[dns_cachestatscounter_querymisses], writer));
	TRY0(renderstat("DeleteLRU",
		   values[dns_cachestatscounter_deletelru], writer));
	TRY0(renderstat("DeleteTTL",
		   values[dns_cachestatscounter_deletettl], writer));

	TRY0(renderstat("CacheNodes", dns_db_nodecount(cache->db), writer));
	TRY0(renderstat("CacheBuckets", dns_db_hashsize(cache->db), writer));

	TRY0(renderstat("TreeMemTotal", isc_mem_total(cache->mctx), writer));
	TRY0(renderstat("TreeMemInUse", isc_mem_inuse(cache->mctx), writer));
	TRY0(renderstat("TreeMemMax", isc_mem_maxinuse(cache->mctx), writer));

	TRY0(renderstat("HeapMemTotal", isc_mem_total(cache->hmctx), writer));
	TRY0(renderstat("HeapMemInUse", isc_mem_inuse(cache->hmctx), writer));
	TRY0(renderstat("HeapMemMax", isc_mem_maxinuse(cache->hmctx), writer));
error:
	return (xmlrc);
}
#endif

#ifdef HAVE_JSON
#define CHECKMEM(m) do { \
	if (m == NULL) { \
		result = ISC_R_NOMEMORY;\
		goto error;\
	} \
} while(/*CONSTCOND*/0)

isc_result_t
dns_cache_renderjson(dns_cache_t *cache, json_object *cstats) {
	isc_result_t result = ISC_R_SUCCESS;
	int indices[dns_cachestatscounter_max];
	isc_uint64_t values[dns_cachestatscounter_max];
	json_object *obj;

	REQUIRE(VALID_CACHE(cache));

	getcounters(cache->stats, isc_statsformat_file,
		    dns_cachestatscounter_max, indices, values);

	obj = json_object_new_int64(values[dns_cachestatscounter_hits]);
	CHECKMEM(obj);
	json_object_object_add(cstats, "CacheHits", obj);

	obj = json_object_new_int64(values[dns_cachestatscounter_misses]);
	CHECKMEM(obj);
	json_object_object_add(cstats, "CacheMisses", obj);

	obj = json_object_new_int64(values[dns_cachestatscounter_queryhits]);
	CHECKMEM(obj);
	json_object_object_add(cstats, "QueryHits", obj);

	obj = json_object_new_int64(values[dns_cachestatscounter_querymisses]);
	CHECKMEM(obj);
	json_object_object_add(cstats, "QueryMisses", obj);

	obj = json_object_new_int64(values[dns_cachestatscounter_deletelru]);
	CHECKMEM(obj);
	json_object_object_add(cstats, "DeleteLRU", obj);

	obj = json_object_new_int64(values[dns_cachestatscounter_deletettl]);
	CHECKMEM(obj);
	json_object_object_add(cstats, "DeleteTTL", obj);

	obj = json_object_new_int64(dns_db_nodecount(cache->db));
	CHECKMEM(obj);
	json_object_object_add(cstats, "CacheNodes", obj);

	obj = json_object_new_int64(dns_db_hashsize(cache->db));
	CHECKMEM(obj);
	json_object_object_add(cstats, "CacheBuckets", obj);

	obj = json_object_new_int64(isc_mem_total(cache->mctx));
	CHECKMEM(obj);
	json_object_object_add(cstats, "TreeMemTotal", obj);

	obj = json_object_new_int64(isc_mem_inuse(cache->mctx));
	CHECKMEM(obj);
	json_object_object_add(cstats, "TreeMemInUse", obj);

	obj = json_object_new_int64(isc_mem_maxinuse(cache->mctx));
	CHECKMEM(obj);
	json_object_object_add(cstats, "HeapMemMax", obj);

	obj = json_object_new_int64(isc_mem_total(cache->hmctx));
	CHECKMEM(obj);
	json_object_object_add(cstats, "HeapMemTotal", obj);

	obj = json_object_new_int64(isc_mem_inuse(cache->hmctx));
	CHECKMEM(obj);
	json_object_object_add(cstats, "HeapMemInUse", obj);

	obj = json_object_new_int64(isc_mem_maxinuse(cache->hmctx));
	CHECKMEM(obj);
	json_object_object_add(cstats, "HeapMemMax", obj);

	result = ISC_R_SUCCESS;
error:
	return (result);
}
#endif
