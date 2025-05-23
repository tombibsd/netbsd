/*	$NetBSD$	*/

/*
 * Copyright (C) 2004-2016  Internet Systems Consortium, Inc. ("ISC")
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

/*! \file */

#include <config.h>

#include <string.h>

#include <isc/hex.h>
#include <isc/mem.h>
#include <isc/print.h>
#include <isc/rwlock.h>
#include <isc/serial.h>
#include <isc/stats.h>
#include <isc/thread.h>
#include <isc/util.h>

#include <dns/adb.h>
#include <dns/byaddr.h>
#include <dns/cache.h>
#include <dns/db.h>
#include <dns/dlz.h>
#include <dns/dns64.h>
#include <dns/dnssec.h>
#include <dns/events.h>
#include <dns/message.h>
#include <dns/ncache.h>
#include <dns/nsec3.h>
#include <dns/order.h>
#include <dns/rdata.h>
#include <dns/rdataclass.h>
#include <dns/rdatalist.h>
#include <dns/rdataset.h>
#include <dns/rdatasetiter.h>
#include <dns/rdatastruct.h>
#include <dns/rdatatype.h>
#include <dns/resolver.h>
#include <dns/result.h>
#include <dns/stats.h>
#include <dns/tkey.h>
#include <dns/view.h>
#include <dns/zone.h>
#include <dns/zt.h>

#include <named/client.h>
#include <named/globals.h>
#include <named/log.h>
#include <named/server.h>
#include <named/sortlist.h>
#include <named/xfrout.h>

#include "pfilter.h"

#if 0
/*
 * It has been recommended that DNS64 be changed to return excluded
 * AAAA addresses if DNS64 synthesis does not occur.  This minimises
 * the impact on the lookup results.  While most DNS AAAA lookups are
 * done to send IP packets to a host, not all of them are and filtering
 * excluded addresses has a negative impact on those uses.
 */
#define dns64_bis_return_excluded_addresses 1
#endif

/*% Partial answer? */
#define PARTIALANSWER(c)	(((c)->query.attributes & \
				  NS_QUERYATTR_PARTIALANSWER) != 0)
/*% Use Cache? */
#define USECACHE(c)		(((c)->query.attributes & \
				  NS_QUERYATTR_CACHEOK) != 0)
/*% Recursion OK? */
#define RECURSIONOK(c)		(((c)->query.attributes & \
				  NS_QUERYATTR_RECURSIONOK) != 0)
/*% Recursing? */
#define RECURSING(c)		(((c)->query.attributes & \
				  NS_QUERYATTR_RECURSING) != 0)
/*% Cache glue ok? */
#define CACHEGLUEOK(c)		(((c)->query.attributes & \
				  NS_QUERYATTR_CACHEGLUEOK) != 0)
/*% Want Recursion? */
#define WANTRECURSION(c)	(((c)->query.attributes & \
				  NS_QUERYATTR_WANTRECURSION) != 0)
/*% Is TCP? */
#define TCP(c)                  (((c)->attributes & NS_CLIENTATTR_TCP) != 0)

/*% Want DNSSEC? */
#define WANTDNSSEC(c)		(((c)->attributes & \
				  NS_CLIENTATTR_WANTDNSSEC) != 0)
/*% Want WANTAD? */
#define WANTAD(c)		(((c)->attributes & \
				  NS_CLIENTATTR_WANTAD) != 0)
#ifdef ISC_PLATFORM_USESIT
/*% Client presented a valid Source Identity Token. */
#define HAVESIT(c)		(((c)->attributes & \
				  NS_CLIENTATTR_HAVESIT) != 0)
#else
#define HAVESIT(c)		(0)
#endif

/*% No authority? */
#define NOAUTHORITY(c)		(((c)->query.attributes & \
				  NS_QUERYATTR_NOAUTHORITY) != 0)
/*% No additional? */
#define NOADDITIONAL(c)		(((c)->query.attributes & \
				  NS_QUERYATTR_NOADDITIONAL) != 0)
/*% Secure? */
#define SECURE(c)		(((c)->query.attributes & \
				  NS_QUERYATTR_SECURE) != 0)
/*% DNS64 A lookup? */
#define DNS64(c)		(((c)->query.attributes & \
				  NS_QUERYATTR_DNS64) != 0)

#define DNS64EXCLUDE(c)		(((c)->query.attributes & \
				  NS_QUERYATTR_DNS64EXCLUDE) != 0)

/*% No QNAME Proof? */
#define NOQNAME(r)		(((r)->attributes & \
				  DNS_RDATASETATTR_NOQNAME) != 0)

#ifdef WANT_QUERYTRACE
static inline void
client_trace(ns_client_t *client, int level, const char *message) {
	if (client != NULL && client->query.qname != NULL) {
		if (isc_log_wouldlog(ns_g_lctx, level)) {
			char qbuf[DNS_NAME_FORMATSIZE];
			char tbuf[DNS_RDATATYPE_FORMATSIZE];
			dns_name_format(client->query.qname,
					qbuf, sizeof(qbuf));
			dns_rdatatype_format(client->query.qtype,
					     tbuf, sizeof(tbuf));
			isc_log_write(ns_g_lctx,
				      NS_LOGCATEGORY_CLIENT,
				      NS_LOGMODULE_QUERY, level,
				      "query client=%p thread=0x%lx "
				      "(%s/%s): %s",
				      client,
				      (unsigned long) isc_thread_self(),
				      qbuf, tbuf, message);
		}
	 } else {
		isc_log_write(ns_g_lctx,
			      NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_QUERY, level,
			      "query client=%p thread=0x%lx "
			      "(<unknown-query>): %s",
			      client,
			      (unsigned long) isc_thread_self(),
			      message);
	}
}
#define CTRACE(l,m)	  client_trace(client, l, m)
#else
#define CTRACE(l,m) ((void)m)
#endif /* WANT_QUERYTRACE */


#define DNS_GETDB_NOEXACT 0x01U
#define DNS_GETDB_NOLOG 0x02U
#define DNS_GETDB_PARTIAL 0x04U
#define DNS_GETDB_IGNOREACL 0x08U

#define PENDINGOK(x)	(((x) & DNS_DBFIND_PENDINGOK) != 0)

typedef struct client_additionalctx {
	ns_client_t *client;
	dns_rdataset_t *rdataset;
} client_additionalctx_t;

static isc_result_t
query_find(ns_client_t *client, dns_fetchevent_t *event, dns_rdatatype_t qtype);

static isc_boolean_t
validate(ns_client_t *client, dns_db_t *db, dns_name_t *name,
	 dns_rdataset_t *rdataset, dns_rdataset_t *sigrdataset);

static void
query_findclosestnsec3(dns_name_t *qname, dns_db_t *db,
		       dns_dbversion_t *version, ns_client_t *client,
		       dns_rdataset_t *rdataset, dns_rdataset_t *sigrdataset,
		       dns_name_t *fname, isc_boolean_t exact,
		       dns_name_t *found);

static inline void
log_queryerror(ns_client_t *client, isc_result_t result, int line, int level);

static void
rpz_st_clear(ns_client_t *client);

static isc_boolean_t
rpz_ck_dnssec(ns_client_t *client, isc_result_t qresult,
	      dns_rdataset_t *rdataset, dns_rdataset_t *sigrdataset);

/*%
 * Increment query statistics counters.
 */
static inline void
inc_stats(ns_client_t *client, isc_statscounter_t counter) {
	dns_zone_t *zone = client->query.authzone;
	dns_rdatatype_t qtype;
	dns_rdataset_t *rdataset;
	isc_stats_t *zonestats;
	dns_stats_t *querystats = NULL;

	isc_stats_increment(ns_g_server->nsstats, counter);

	if (zone == NULL)
		return;

	/* Do regular response type stats */
	zonestats = dns_zone_getrequeststats(zone);

	if (zonestats != NULL)
		isc_stats_increment(zonestats, counter);

	/* Do query type statistics
	 *
	 * We only increment per-type if we're using the authoritative
	 * answer counter, preventing double-counting.
	 */
	if (counter == dns_nsstatscounter_authans) {
		querystats = dns_zone_getrcvquerystats(zone);
		if (querystats != NULL) {
			rdataset = ISC_LIST_HEAD(client->query.qname->list);
			if (rdataset != NULL) {
				qtype = rdataset->type;
				dns_rdatatypestats_increment(querystats, qtype);
			}
		}
	}
}

static void
query_send(ns_client_t *client) {
	isc_statscounter_t counter;

	if ((client->message->flags & DNS_MESSAGEFLAG_AA) == 0)
		inc_stats(client, dns_nsstatscounter_nonauthans);
	else
		inc_stats(client, dns_nsstatscounter_authans);

	if (client->message->rcode == dns_rcode_noerror) {
		dns_section_t answer = DNS_SECTION_ANSWER;
		if (ISC_LIST_EMPTY(client->message->sections[answer])) {
			if (client->query.isreferral)
				counter = dns_nsstatscounter_referral;
			else
				counter = dns_nsstatscounter_nxrrset;
		} else
			counter = dns_nsstatscounter_success;
	} else if (client->message->rcode == dns_rcode_nxdomain)
		counter = dns_nsstatscounter_nxdomain;
	else /* We end up here in case of YXDOMAIN, and maybe others */
		counter = dns_nsstatscounter_failure;

	inc_stats(client, counter);
	ns_client_send(client);
}

static void
query_error(ns_client_t *client, isc_result_t result, int line) {
	int loglevel = ISC_LOG_DEBUG(3);

	switch (result) {
	case DNS_R_SERVFAIL:
		loglevel = ISC_LOG_DEBUG(1);
		inc_stats(client, dns_nsstatscounter_servfail);
		break;
	case DNS_R_FORMERR:
		inc_stats(client, dns_nsstatscounter_formerr);
		break;
	default:
		inc_stats(client, dns_nsstatscounter_failure);
		break;
	}

	log_queryerror(client, result, line, loglevel);

	ns_client_error(client, result);
}

static void
query_next(ns_client_t *client, isc_result_t result) {
	if (result == DNS_R_DUPLICATE)
		inc_stats(client, dns_nsstatscounter_duplicate);
	else if (result == DNS_R_DROP)
		inc_stats(client, dns_nsstatscounter_dropped);
	else
		inc_stats(client, dns_nsstatscounter_failure);
	ns_client_next(client, result);
}

static inline void
query_freefreeversions(ns_client_t *client, isc_boolean_t everything) {
	ns_dbversion_t *dbversion, *dbversion_next;
	unsigned int i;

	for (dbversion = ISC_LIST_HEAD(client->query.freeversions), i = 0;
	     dbversion != NULL;
	     dbversion = dbversion_next, i++)
	{
		dbversion_next = ISC_LIST_NEXT(dbversion, link);
		/*
		 * If we're not freeing everything, we keep the first three
		 * dbversions structures around.
		 */
		if (i > 3 || everything) {
			ISC_LIST_UNLINK(client->query.freeversions, dbversion,
					link);
			isc_mem_put(client->mctx, dbversion,
				    sizeof(*dbversion));
		}
	}
}

void
ns_query_cancel(ns_client_t *client) {
	LOCK(&client->query.fetchlock);
	if (client->query.fetch != NULL) {
		dns_resolver_cancelfetch(client->query.fetch);

		client->query.fetch = NULL;
	}
	UNLOCK(&client->query.fetchlock);
}

static inline void
query_putrdataset(ns_client_t *client, dns_rdataset_t **rdatasetp) {
	dns_rdataset_t *rdataset = *rdatasetp;

	CTRACE(ISC_LOG_DEBUG(3), "query_putrdataset");
	if (rdataset != NULL) {
		if (dns_rdataset_isassociated(rdataset))
			dns_rdataset_disassociate(rdataset);
		dns_message_puttemprdataset(client->message, rdatasetp);
	}
	CTRACE(ISC_LOG_DEBUG(3), "query_putrdataset: done");
}

static inline void
query_reset(ns_client_t *client, isc_boolean_t everything) {
	isc_buffer_t *dbuf, *dbuf_next;
	ns_dbversion_t *dbversion, *dbversion_next;

	CTRACE(ISC_LOG_DEBUG(3), "query_reset");

	/*%
	 * Reset the query state of a client to its default state.
	 */

	/*
	 * Cancel the fetch if it's running.
	 */
	ns_query_cancel(client);

	/*
	 * Cleanup any active versions.
	 */
	for (dbversion = ISC_LIST_HEAD(client->query.activeversions);
	     dbversion != NULL;
	     dbversion = dbversion_next) {
		dbversion_next = ISC_LIST_NEXT(dbversion, link);
		dns_db_closeversion(dbversion->db, &dbversion->version,
				    ISC_FALSE);
		dns_db_detach(&dbversion->db);
		ISC_LIST_INITANDAPPEND(client->query.freeversions,
				      dbversion, link);
	}
	ISC_LIST_INIT(client->query.activeversions);

	if (client->query.authdb != NULL)
		dns_db_detach(&client->query.authdb);
	if (client->query.authzone != NULL)
		dns_zone_detach(&client->query.authzone);

	if (client->query.dns64_aaaa != NULL)
		query_putrdataset(client, &client->query.dns64_aaaa);
	if (client->query.dns64_sigaaaa != NULL)
		query_putrdataset(client, &client->query.dns64_sigaaaa);
	if (client->query.dns64_aaaaok != NULL) {
		isc_mem_put(client->mctx, client->query.dns64_aaaaok,
			    client->query.dns64_aaaaoklen *
			    sizeof(isc_boolean_t));
		client->query.dns64_aaaaok =  NULL;
		client->query.dns64_aaaaoklen =  0;
	}

	query_freefreeversions(client, everything);

	for (dbuf = ISC_LIST_HEAD(client->query.namebufs);
	     dbuf != NULL;
	     dbuf = dbuf_next) {
		dbuf_next = ISC_LIST_NEXT(dbuf, link);
		if (dbuf_next != NULL || everything) {
			ISC_LIST_UNLINK(client->query.namebufs, dbuf, link);
			isc_buffer_free(&dbuf);
		}
	}

	if (client->query.restarts > 0) {
		/*
		 * client->query.qname was dynamically allocated.
		 */
		dns_message_puttempname(client->message,
					&client->query.qname);
	}
	client->query.qname = NULL;
	client->query.attributes = (NS_QUERYATTR_RECURSIONOK |
				    NS_QUERYATTR_CACHEOK |
				    NS_QUERYATTR_SECURE);
	client->query.restarts = 0;
	client->query.timerset = ISC_FALSE;
	if (client->query.rpz_st != NULL) {
		rpz_st_clear(client);
		if (everything) {
			isc_mem_put(client->mctx, client->query.rpz_st,
				    sizeof(*client->query.rpz_st));
			client->query.rpz_st = NULL;
		}
	}
	client->query.origqname = NULL;
	client->query.dboptions = 0;
	client->query.fetchoptions = 0;
	client->query.gluedb = NULL;
	client->query.authdbset = ISC_FALSE;
	client->query.isreferral = ISC_FALSE;
	client->query.dns64_options = 0;
	client->query.dns64_ttl = ISC_UINT32_MAX;
}

static void
query_next_callback(ns_client_t *client) {
	query_reset(client, ISC_FALSE);
}

void
ns_query_free(ns_client_t *client) {
	query_reset(client, ISC_TRUE);
}

static inline isc_result_t
query_newnamebuf(ns_client_t *client) {
	isc_buffer_t *dbuf;
	isc_result_t result;

	CTRACE(ISC_LOG_DEBUG(3), "query_newnamebuf");
	/*%
	 * Allocate a name buffer.
	 */

	dbuf = NULL;
	result = isc_buffer_allocate(client->mctx, &dbuf, 1024);
	if (result != ISC_R_SUCCESS) {
		CTRACE(ISC_LOG_DEBUG(3),
		       "query_newnamebuf: isc_buffer_allocate failed: done");
		return (result);
	}
	ISC_LIST_APPEND(client->query.namebufs, dbuf, link);

	CTRACE(ISC_LOG_DEBUG(3), "query_newnamebuf: done");
	return (ISC_R_SUCCESS);
}

static inline isc_buffer_t *
query_getnamebuf(ns_client_t *client) {
	isc_buffer_t *dbuf;
	isc_result_t result;
	isc_region_t r;

	CTRACE(ISC_LOG_DEBUG(3), "query_getnamebuf");
	/*%
	 * Return a name buffer with space for a maximal name, allocating
	 * a new one if necessary.
	 */

	if (ISC_LIST_EMPTY(client->query.namebufs)) {
		result = query_newnamebuf(client);
		if (result != ISC_R_SUCCESS) {
		    CTRACE(ISC_LOG_DEBUG(3),
			   "query_getnamebuf: query_newnamebuf failed: done");
			return (NULL);
		}
	}

	dbuf = ISC_LIST_TAIL(client->query.namebufs);
	INSIST(dbuf != NULL);
	isc_buffer_availableregion(dbuf, &r);
	if (r.length < DNS_NAME_MAXWIRE) {
		result = query_newnamebuf(client);
		if (result != ISC_R_SUCCESS) {
		    CTRACE(ISC_LOG_DEBUG(3),
			   "query_getnamebuf: query_newnamebuf failed: done");
			return (NULL);

		}
		dbuf = ISC_LIST_TAIL(client->query.namebufs);
		isc_buffer_availableregion(dbuf, &r);
		INSIST(r.length >= 255);
	}
	CTRACE(ISC_LOG_DEBUG(3), "query_getnamebuf: done");
	return (dbuf);
}

static inline void
query_keepname(ns_client_t *client, dns_name_t *name, isc_buffer_t *dbuf) {
	isc_region_t r;

	CTRACE(ISC_LOG_DEBUG(3), "query_keepname");
	/*%
	 * 'name' is using space in 'dbuf', but 'dbuf' has not yet been
	 * adjusted to take account of that.  We do the adjustment.
	 */

	REQUIRE((client->query.attributes & NS_QUERYATTR_NAMEBUFUSED) != 0);

	dns_name_toregion(name, &r);
	isc_buffer_add(dbuf, r.length);
	dns_name_setbuffer(name, NULL);
	client->query.attributes &= ~NS_QUERYATTR_NAMEBUFUSED;
}

static inline void
query_releasename(ns_client_t *client, dns_name_t **namep) {
	dns_name_t *name = *namep;

	/*%
	 * 'name' is no longer needed.  Return it to our pool of temporary
	 * names.  If it is using a name buffer, relinquish its exclusive
	 * rights on the buffer.
	 */

	CTRACE(ISC_LOG_DEBUG(3), "query_releasename");
	if (dns_name_hasbuffer(name)) {
		INSIST((client->query.attributes & NS_QUERYATTR_NAMEBUFUSED)
		       != 0);
		client->query.attributes &= ~NS_QUERYATTR_NAMEBUFUSED;
	}
	dns_message_puttempname(client->message, namep);
	CTRACE(ISC_LOG_DEBUG(3), "query_releasename: done");
}

static inline dns_name_t *
query_newname(ns_client_t *client, isc_buffer_t *dbuf,
	      isc_buffer_t *nbuf)
{
	dns_name_t *name;
	isc_region_t r;
	isc_result_t result;

	REQUIRE((client->query.attributes & NS_QUERYATTR_NAMEBUFUSED) == 0);

	CTRACE(ISC_LOG_DEBUG(3), "query_newname");
	name = NULL;
	result = dns_message_gettempname(client->message, &name);
	if (result != ISC_R_SUCCESS) {
		CTRACE(ISC_LOG_DEBUG(3),
		       "query_newname: dns_message_gettempname failed: done");
		return (NULL);
	}
	isc_buffer_availableregion(dbuf, &r);
	isc_buffer_init(nbuf, r.base, r.length);
	dns_name_init(name, NULL);
	dns_name_setbuffer(name, nbuf);
	client->query.attributes |= NS_QUERYATTR_NAMEBUFUSED;

	CTRACE(ISC_LOG_DEBUG(3), "query_newname: done");
	return (name);
}

static inline dns_rdataset_t *
query_newrdataset(ns_client_t *client) {
	dns_rdataset_t *rdataset;
	isc_result_t result;

	CTRACE(ISC_LOG_DEBUG(3), "query_newrdataset");
	rdataset = NULL;
	result = dns_message_gettemprdataset(client->message, &rdataset);
	if (result != ISC_R_SUCCESS) {
	  CTRACE(ISC_LOG_DEBUG(3),
		 "query_newrdataset: "
		 "dns_message_gettemprdataset failed: done");
		return (NULL);
	}

	CTRACE(ISC_LOG_DEBUG(3), "query_newrdataset: done");
	return (rdataset);
}

static inline isc_result_t
query_newdbversion(ns_client_t *client, unsigned int n) {
	unsigned int i;
	ns_dbversion_t *dbversion;

	for (i = 0; i < n; i++) {
		dbversion = isc_mem_get(client->mctx, sizeof(*dbversion));
		if (dbversion != NULL) {
			dbversion->db = NULL;
			dbversion->version = NULL;
			ISC_LIST_INITANDAPPEND(client->query.freeversions,
					      dbversion, link);
		} else {
			/*
			 * We only return ISC_R_NOMEMORY if we couldn't
			 * allocate anything.
			 */
			if (i == 0)
				return (ISC_R_NOMEMORY);
			else
				return (ISC_R_SUCCESS);
		}
	}

	return (ISC_R_SUCCESS);
}

static inline ns_dbversion_t *
query_getdbversion(ns_client_t *client) {
	isc_result_t result;
	ns_dbversion_t *dbversion;

	if (ISC_LIST_EMPTY(client->query.freeversions)) {
		result = query_newdbversion(client, 1);
		if (result != ISC_R_SUCCESS)
			return (NULL);
	}
	dbversion = ISC_LIST_HEAD(client->query.freeversions);
	INSIST(dbversion != NULL);
	ISC_LIST_UNLINK(client->query.freeversions, dbversion, link);

	return (dbversion);
}

isc_result_t
ns_query_init(ns_client_t *client) {
	isc_result_t result;

	ISC_LIST_INIT(client->query.namebufs);
	ISC_LIST_INIT(client->query.activeversions);
	ISC_LIST_INIT(client->query.freeversions);
	client->query.restarts = 0;
	client->query.timerset = ISC_FALSE;
	client->query.rpz_st = NULL;
	client->query.qname = NULL;
	/*
	 * This mutex is destroyed when the client is destroyed in
	 * exit_check().
	 */
	result = isc_mutex_init(&client->query.fetchlock);
	if (result != ISC_R_SUCCESS)
		return (result);
	client->query.fetch = NULL;
	client->query.prefetch = NULL;
	client->query.authdb = NULL;
	client->query.authzone = NULL;
	client->query.authdbset = ISC_FALSE;
	client->query.isreferral = ISC_FALSE;
	client->query.dns64_aaaa = NULL;
	client->query.dns64_sigaaaa = NULL;
	client->query.dns64_aaaaok = NULL;
	client->query.dns64_aaaaoklen = 0;
	query_reset(client, ISC_FALSE);
	result = query_newdbversion(client, 3);
	if (result != ISC_R_SUCCESS) {
		DESTROYLOCK(&client->query.fetchlock);
		return (result);
	}
	result = query_newnamebuf(client);
	if (result != ISC_R_SUCCESS) {
		query_freefreeversions(client, ISC_TRUE);
		DESTROYLOCK(&client->query.fetchlock);
	}

	return (result);
}

static inline ns_dbversion_t *
query_findversion(ns_client_t *client, dns_db_t *db) {
	ns_dbversion_t *dbversion;

	/*%
	 * We may already have done a query related to this
	 * database.  If so, we must be sure to make subsequent
	 * queries from the same version.
	 */
	for (dbversion = ISC_LIST_HEAD(client->query.activeversions);
	     dbversion != NULL;
	     dbversion = ISC_LIST_NEXT(dbversion, link)) {
		if (dbversion->db == db)
			break;
	}

	if (dbversion == NULL) {
		/*
		 * This is a new zone for this query.  Add it to
		 * the active list.
		 */
		dbversion = query_getdbversion(client);
		if (dbversion == NULL)
			return (NULL);
		dns_db_attach(db, &dbversion->db);
		dns_db_currentversion(db, &dbversion->version);
		dbversion->acl_checked = ISC_FALSE;
		dbversion->queryok = ISC_FALSE;
		ISC_LIST_APPEND(client->query.activeversions,
				dbversion, link);
	}

	return (dbversion);
}

static inline isc_result_t
query_validatezonedb(ns_client_t *client, dns_name_t *name,
		     dns_rdatatype_t qtype, unsigned int options,
		     dns_zone_t *zone, dns_db_t *db,
		     dns_dbversion_t **versionp)
{
	isc_result_t result;
	dns_acl_t *queryacl, *queryonacl;
	ns_dbversion_t *dbversion;

	REQUIRE(zone != NULL);
	REQUIRE(db != NULL);

	/*
	 * This limits our searching to the zone where the first name
	 * (the query target) was looked for.  This prevents following
	 * CNAMES or DNAMES into other zones and prevents returning
	 * additional data from other zones.
	 */
	if (!client->view->additionalfromauth &&
	    client->query.authdbset &&
	    db != client->query.authdb)
		return (DNS_R_REFUSED);

	/*
	 * Non recursive query to a static-stub zone is prohibited; its
	 * zone content is not public data, but a part of local configuration
	 * and should not be disclosed.
	 */
	if (dns_zone_gettype(zone) == dns_zone_staticstub &&
	    !RECURSIONOK(client)) {
		return (DNS_R_REFUSED);
	}

	/*
	 * If the zone has an ACL, we'll check it, otherwise
	 * we use the view's "allow-query" ACL.  Each ACL is only checked
	 * once per query.
	 *
	 * Also, get the database version to use.
	 */

	/*
	 * Get the current version of this database.
	 */
	dbversion = query_findversion(client, db);
	if (dbversion == NULL) {
		CTRACE(ISC_LOG_ERROR, "unable to get db version");
		return (DNS_R_SERVFAIL);
	}

	if ((options & DNS_GETDB_IGNOREACL) != 0)
		goto approved;
	if (dbversion->acl_checked) {
		if (!dbversion->queryok)
			return (DNS_R_REFUSED);
		goto approved;
	}

	queryacl = dns_zone_getqueryacl(zone);
	if (queryacl == NULL) {
		queryacl = client->view->queryacl;
		if ((client->query.attributes &
		     NS_QUERYATTR_QUERYOKVALID) != 0) {
			/*
			 * We've evaluated the view's queryacl already.  If
			 * NS_QUERYATTR_QUERYOK is set, then the client is
			 * allowed to make queries, otherwise the query should
			 * be refused.
			 */
			dbversion->acl_checked = ISC_TRUE;
			if ((client->query.attributes &
			     NS_QUERYATTR_QUERYOK) == 0) {
				dbversion->queryok = ISC_FALSE;
				return (DNS_R_REFUSED);
			}
			dbversion->queryok = ISC_TRUE;
			goto approved;
		}
	}

	result = ns_client_checkaclsilent(client, NULL, queryacl, ISC_TRUE);
	if (result != ISC_R_SUCCESS)
		pfilter_notify(result, client, "validatezonedb");
	if ((options & DNS_GETDB_NOLOG) == 0) {
		char msg[NS_CLIENT_ACLMSGSIZE("query")];
		if (result == ISC_R_SUCCESS) {
			if (isc_log_wouldlog(ns_g_lctx, ISC_LOG_DEBUG(3))) {
				ns_client_aclmsg("query", name, qtype,
						 client->view->rdclass,
						 msg, sizeof(msg));
				ns_client_log(client,
					      DNS_LOGCATEGORY_SECURITY,
					      NS_LOGMODULE_QUERY,
					      ISC_LOG_DEBUG(3),
					      "%s approved", msg);
			}
		} else {
			ns_client_aclmsg("query", name, qtype,
					 client->view->rdclass,
					 msg, sizeof(msg));
			ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
				      NS_LOGMODULE_QUERY, ISC_LOG_INFO,
				      "%s denied", msg);
		}
	}

	if (queryacl == client->view->queryacl) {
		if (result == ISC_R_SUCCESS) {
			/*
			 * We were allowed by the default
			 * "allow-query" ACL.  Remember this so we
			 * don't have to check again.
			 */
			client->query.attributes |= NS_QUERYATTR_QUERYOK;
		}
		/*
		 * We've now evaluated the view's query ACL, and
		 * the NS_QUERYATTR_QUERYOK attribute is now valid.
		 */
		client->query.attributes |= NS_QUERYATTR_QUERYOKVALID;
	}

	/* If and only if we've gotten this far, check allow-query-on too */
	if (result == ISC_R_SUCCESS) {
		queryonacl = dns_zone_getqueryonacl(zone);
		if (queryonacl == NULL)
			queryonacl = client->view->queryonacl;

		result = ns_client_checkaclsilent(client, &client->destaddr,
						  queryonacl, ISC_TRUE);
		if ((options & DNS_GETDB_NOLOG) == 0 &&
		    result != ISC_R_SUCCESS)
			ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
				      NS_LOGMODULE_QUERY, ISC_LOG_INFO,
				      "query-on denied");
	}

	dbversion->acl_checked = ISC_TRUE;
	if (result != ISC_R_SUCCESS) {
		dbversion->queryok = ISC_FALSE;
		return (DNS_R_REFUSED);
	}
	dbversion->queryok = ISC_TRUE;

 approved:
	/* Transfer ownership, if necessary. */
	if (versionp != NULL)
		*versionp = dbversion->version;
	return (ISC_R_SUCCESS);
}

static inline isc_result_t
query_getzonedb(ns_client_t *client, dns_name_t *name, dns_rdatatype_t qtype,
		unsigned int options, dns_zone_t **zonep, dns_db_t **dbp,
		dns_dbversion_t **versionp)
{
	isc_result_t result;
	unsigned int ztoptions;
	dns_zone_t *zone = NULL;
	dns_db_t *db = NULL;
	isc_boolean_t partial = ISC_FALSE;

	REQUIRE(zonep != NULL && *zonep == NULL);
	REQUIRE(dbp != NULL && *dbp == NULL);

	/*%
	 * Find a zone database to answer the query.
	 */
	ztoptions = ((options & DNS_GETDB_NOEXACT) != 0) ?
		DNS_ZTFIND_NOEXACT : 0;

	result = dns_zt_find(client->view->zonetable, name, ztoptions, NULL,
			     &zone);

	if (result == DNS_R_PARTIALMATCH)
		partial = ISC_TRUE;
	if (result == ISC_R_SUCCESS || result == DNS_R_PARTIALMATCH)
		result = dns_zone_getdb(zone, &db);

	if (result != ISC_R_SUCCESS)
		goto fail;

	result = query_validatezonedb(client, name, qtype, options, zone, db,
				      versionp);

	if (result != ISC_R_SUCCESS)
		goto fail;

	/* Transfer ownership. */
	*zonep = zone;
	*dbp = db;

	if (partial && (options & DNS_GETDB_PARTIAL) != 0)
		return (DNS_R_PARTIALMATCH);
	return (ISC_R_SUCCESS);

 fail:
	if (zone != NULL)
		dns_zone_detach(&zone);
	if (db != NULL)
		dns_db_detach(&db);

	return (result);
}

static void
rpz_log_rewrite(ns_client_t *client, isc_boolean_t disabled,
		dns_rpz_policy_t policy, dns_rpz_type_t type,
		dns_zone_t *p_zone, dns_name_t *p_name)
{
	isc_stats_t *zonestats;
	char qname_buf[DNS_NAME_FORMATSIZE];
	char p_name_buf[DNS_NAME_FORMATSIZE];

	/*
	 * Count enabled rewrites in the global counter.
	 * Count both enabled and disabled rewrites for each zone.
	 */
	if (!disabled && policy != DNS_RPZ_POLICY_PASSTHRU) {
		isc_stats_increment(ns_g_server->nsstats,
				    dns_nsstatscounter_rpz_rewrites);
	}
	if (p_zone != NULL) {
		zonestats = dns_zone_getrequeststats(p_zone);
		if (zonestats != NULL)
			isc_stats_increment(zonestats,
					    dns_nsstatscounter_rpz_rewrites);
	}

	if (!isc_log_wouldlog(ns_g_lctx, DNS_RPZ_INFO_LEVEL))
		return;

	dns_name_format(client->query.qname, qname_buf, sizeof(qname_buf));
	dns_name_format(p_name, p_name_buf, sizeof(p_name_buf));

	ns_client_log(client, DNS_LOGCATEGORY_RPZ, NS_LOGMODULE_QUERY,
		      DNS_RPZ_INFO_LEVEL, "%srpz %s %s rewrite %s via %s",
		      disabled ? "disabled " : "",
		      dns_rpz_type2str(type), dns_rpz_policy2str(policy),
		      qname_buf, p_name_buf);
}

static void
rpz_log_fail(ns_client_t *client, int level, dns_name_t *p_name,
	     dns_rpz_type_t rpz_type, const char *str, isc_result_t result)
{
	char qnamebuf[DNS_NAME_FORMATSIZE];
	char p_namebuf[DNS_NAME_FORMATSIZE];
	const char *failed;

	if (!isc_log_wouldlog(ns_g_lctx, level))
		return;

	/*
	 * bin/tests/system/rpz/tests.sh looks for "rpz.*failed" for problems.
	 */
	if (level <= DNS_RPZ_DEBUG_LEVEL1)
		failed = "failed: ";
	else
		failed = ": ";
	dns_name_format(client->query.qname, qnamebuf, sizeof(qnamebuf));
	dns_name_format(p_name, p_namebuf, sizeof(p_namebuf));
	ns_client_log(client, NS_LOGCATEGORY_QUERY_ERRORS,
		      NS_LOGMODULE_QUERY, level,
		      "rpz %s rewrite %s via %s%s%s%s",
		      dns_rpz_type2str(rpz_type),
		      qnamebuf, p_namebuf,
		      str, failed, isc_result_totext(result));
}

/*
 * Get a policy rewrite zone database.
 */
static isc_result_t
rpz_getdb(ns_client_t *client, dns_name_t *p_name, dns_rpz_type_t rpz_type,
	  dns_zone_t **zonep, dns_db_t **dbp, dns_dbversion_t **versionp)
{
	char qnamebuf[DNS_NAME_FORMATSIZE];
	char p_namebuf[DNS_NAME_FORMATSIZE];
	dns_dbversion_t *rpz_version = NULL;
	isc_result_t result;

	CTRACE(ISC_LOG_DEBUG(3), "rpz_getdb");

	result = query_getzonedb(client, p_name, dns_rdatatype_any,
				 DNS_GETDB_IGNOREACL, zonep, dbp, &rpz_version);
	if (result == ISC_R_SUCCESS) {
		if (isc_log_wouldlog(ns_g_lctx, DNS_RPZ_DEBUG_LEVEL2)) {
			dns_name_format(client->query.qname, qnamebuf,
					sizeof(qnamebuf));
			dns_name_format(p_name, p_namebuf, sizeof(p_namebuf));
			ns_client_log(client, DNS_LOGCATEGORY_RPZ,
				      NS_LOGMODULE_QUERY, DNS_RPZ_DEBUG_LEVEL2,
				      "try rpz %s rewrite %s via %s",
				      dns_rpz_type2str(rpz_type),
				      qnamebuf, p_namebuf);
		}
		*versionp = rpz_version;
		return (ISC_R_SUCCESS);
	}
	rpz_log_fail(client, DNS_RPZ_ERROR_LEVEL, p_name, rpz_type,
		     " query_getzonedb()", result);
	return (result);
}

static inline isc_result_t
query_getcachedb(ns_client_t *client, dns_name_t *name, dns_rdatatype_t qtype,
		 dns_db_t **dbp, unsigned int options)
{
	isc_result_t result;
	isc_boolean_t check_acl;
	dns_db_t *db = NULL;

	REQUIRE(dbp != NULL && *dbp == NULL);

	/*%
	 * Find a cache database to answer the query.
	 * This may fail with DNS_R_REFUSED if the client
	 * is not allowed to use the cache.
	 */

	if (!USECACHE(client))
		return (DNS_R_REFUSED);
	dns_db_attach(client->view->cachedb, &db);

	if ((client->query.attributes & NS_QUERYATTR_CACHEACLOKVALID) != 0) {
		/*
		 * We've evaluated the view's cacheacl already.  If
		 * NS_QUERYATTR_CACHEACLOK is set, then the client is
		 * allowed to make queries, otherwise the query should
		 * be refused.
		 */
		check_acl = ISC_FALSE;
		if ((client->query.attributes & NS_QUERYATTR_CACHEACLOK) == 0)
			goto refuse;
	} else {
		/*
		 * We haven't evaluated the view's queryacl yet.
		 */
		check_acl = ISC_TRUE;
	}

	if (check_acl) {
		isc_boolean_t log = ISC_TF((options & DNS_GETDB_NOLOG) == 0);
		char msg[NS_CLIENT_ACLMSGSIZE("query (cache)")];

		result = ns_client_checkaclsilent(client, NULL,
						  client->view->cacheacl,
						  ISC_TRUE);
		if (result == ISC_R_SUCCESS)
			pfilter_notify(result, client, "cachedb");
		if (result == ISC_R_SUCCESS) {
			/*
			 * We were allowed by the "allow-query-cache" ACL.
			 * Remember this so we don't have to check again.
			 */
			client->query.attributes |=
				NS_QUERYATTR_CACHEACLOK;
			if (log && isc_log_wouldlog(ns_g_lctx,
						     ISC_LOG_DEBUG(3)))
			{
				ns_client_aclmsg("query (cache)", name, qtype,
						 client->view->rdclass,
						 msg, sizeof(msg));
				ns_client_log(client,
					      DNS_LOGCATEGORY_SECURITY,
					      NS_LOGMODULE_QUERY,
					      ISC_LOG_DEBUG(3),
					      "%s approved", msg);
			}
		} else if (log) {
			ns_client_aclmsg("query (cache)", name, qtype,
					 client->view->rdclass, msg,
					 sizeof(msg));
			ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
				      NS_LOGMODULE_QUERY, ISC_LOG_INFO,
				      "%s denied", msg);
		}
		/*
		 * We've now evaluated the view's query ACL, and
		 * the NS_QUERYATTR_CACHEACLOKVALID attribute is now valid.
		 */
		client->query.attributes |= NS_QUERYATTR_CACHEACLOKVALID;

		if (result != ISC_R_SUCCESS)
			goto refuse;
	}

	/* Approved. */

	/* Transfer ownership. */
	*dbp = db;

	return (ISC_R_SUCCESS);

 refuse:
	result = DNS_R_REFUSED;

	if (db != NULL)
		dns_db_detach(&db);

	return (result);
}


static inline isc_result_t
query_getdb(ns_client_t *client, dns_name_t *name, dns_rdatatype_t qtype,
	    unsigned int options, dns_zone_t **zonep, dns_db_t **dbp,
	    dns_dbversion_t **versionp, isc_boolean_t *is_zonep)
{
	isc_result_t result;

	isc_result_t tresult;
	unsigned int namelabels;
	unsigned int zonelabels;
	dns_zone_t *zone = NULL;

	REQUIRE(zonep != NULL && *zonep == NULL);

	/* Calculate how many labels are in name. */
	namelabels = dns_name_countlabels(name);
	zonelabels = 0;

	/* Try to find name in bind's standard database. */
	result = query_getzonedb(client, name, qtype, options, &zone,
				 dbp, versionp);

	/* See how many labels are in the zone's name.	  */
	if (result == ISC_R_SUCCESS && zone != NULL)
		zonelabels = dns_name_countlabels(dns_zone_getorigin(zone));

	/*
	 * If # zone labels < # name labels, try to find an even better match
	 * Only try if DLZ drivers are loaded for this view
	 */
	if (ISC_UNLIKELY(zonelabels < namelabels &&
			 !ISC_LIST_EMPTY(client->view->dlz_searched)))
	{
		dns_clientinfomethods_t cm;
		dns_clientinfo_t ci;
		dns_db_t *tdbp;

		dns_clientinfomethods_init(&cm, ns_client_sourceip);
		dns_clientinfo_init(&ci, client);

		tdbp = NULL;
		tresult = dns_view_searchdlz(client->view, name,
					     zonelabels, &cm, &ci, &tdbp);
		 /* If we successful, we found a better match. */
		if (tresult == ISC_R_SUCCESS) {
			/*
			 * If the previous search returned a zone, detach it.
			 */
			if (zone != NULL)
				dns_zone_detach(&zone);

			/*
			 * If the previous search returned a database,
			 * detach it.
			 */
			if (*dbp != NULL)
				dns_db_detach(dbp);

			/*
			 * If the previous search returned a version, clear it.
			 */
			*versionp = NULL;

			/*
			 * Get our database version.
			 */
			dns_db_currentversion(tdbp, versionp);

			/*
			 * Be sure to return our database.
			 */
			*dbp = tdbp;

			/*
			 * We return a null zone, No stats for DLZ zones.
			 */
			zone = NULL;
			result = tresult;
		}
	}

	/* If successful, Transfer ownership of zone. */
	if (result == ISC_R_SUCCESS) {
		*zonep = zone;
		/*
		 * If neither attempt above succeeded, return the cache instead
		 */
		*is_zonep = ISC_TRUE;
	} else if (result == ISC_R_NOTFOUND) {
		result = query_getcachedb(client, name, qtype, dbp, options);
		*is_zonep = ISC_FALSE;
	}
	return (result);
}

static inline isc_boolean_t
query_isduplicate(ns_client_t *client, dns_name_t *name,
		  dns_rdatatype_t type, dns_name_t **mnamep)
{
	dns_section_t section;
	dns_name_t *mname = NULL;
	isc_result_t result;

	CTRACE(ISC_LOG_DEBUG(3), "query_isduplicate");

	for (section = DNS_SECTION_ANSWER;
	     section <= DNS_SECTION_ADDITIONAL;
	     section++) {
		result = dns_message_findname(client->message, section,
					      name, type, 0, &mname, NULL);
		if (result == ISC_R_SUCCESS) {
			/*
			 * We've already got this RRset in the response.
			 */
			CTRACE(ISC_LOG_DEBUG(3),
			       "query_isduplicate: true: done");
			return (ISC_TRUE);
		} else if (result == DNS_R_NXRRSET) {
			/*
			 * The name exists, but the rdataset does not.
			 */
			if (section == DNS_SECTION_ADDITIONAL)
				break;
		} else
			RUNTIME_CHECK(result == DNS_R_NXDOMAIN);
		mname = NULL;
	}

	if (mnamep != NULL)
		*mnamep = mname;

	CTRACE(ISC_LOG_DEBUG(3), "query_isduplicate: false: done");
	return (ISC_FALSE);
}

static isc_result_t
query_addadditional(void *arg, dns_name_t *name, dns_rdatatype_t qtype) {
	ns_client_t *client = arg;
	isc_result_t result, eresult;
	dns_dbnode_t *node;
	dns_db_t *db;
	dns_name_t *fname, *mname;
	dns_rdataset_t *rdataset, *sigrdataset, *trdataset;
	isc_buffer_t *dbuf;
	isc_buffer_t b;
	dns_dbversion_t *version;
	isc_boolean_t added_something, need_addname;
	dns_zone_t *zone;
	dns_rdatatype_t type;
	dns_clientinfomethods_t cm;
	dns_clientinfo_t ci;

	REQUIRE(NS_CLIENT_VALID(client));
	REQUIRE(qtype != dns_rdatatype_any);

	if (!WANTDNSSEC(client) && dns_rdatatype_isdnssec(qtype))
		return (ISC_R_SUCCESS);

	CTRACE(ISC_LOG_DEBUG(3), "query_addadditional");

	/*
	 * Initialization.
	 */
	eresult = ISC_R_SUCCESS;
	fname = NULL;
	rdataset = NULL;
	sigrdataset = NULL;
	trdataset = NULL;
	db = NULL;
	version = NULL;
	node = NULL;
	added_something = ISC_FALSE;
	need_addname = ISC_FALSE;
	zone = NULL;

	dns_clientinfomethods_init(&cm, ns_client_sourceip);
	dns_clientinfo_init(&ci, client);

	/*
	 * We treat type A additional section processing as if it
	 * were "any address type" additional section processing.
	 * To avoid multiple lookups, we do an 'any' database
	 * lookup and iterate over the node.
	 */
	if (qtype == dns_rdatatype_a)
		type = dns_rdatatype_any;
	else
		type = qtype;

	/*
	 * Get some resources.
	 */
	dbuf = query_getnamebuf(client);
	if (dbuf == NULL)
		goto cleanup;
	fname = query_newname(client, dbuf, &b);
	rdataset = query_newrdataset(client);
	if (fname == NULL || rdataset == NULL)
		goto cleanup;
	if (WANTDNSSEC(client)) {
		sigrdataset = query_newrdataset(client);
		if (sigrdataset == NULL)
			goto cleanup;
	}

	/*
	 * Look for a zone database that might contain authoritative
	 * additional data.
	 */
	result = query_getzonedb(client, name, qtype, DNS_GETDB_NOLOG,
				 &zone, &db, &version);
	if (result != ISC_R_SUCCESS)
		goto try_cache;

	CTRACE(ISC_LOG_DEBUG(3), "query_addadditional: db_find");

	/*
	 * Since we are looking for authoritative data, we do not set
	 * the GLUEOK flag.  Glue will be looked for later, but not
	 * necessarily in the same database.
	 */
	node = NULL;
	result = dns_db_findext(db, name, version, type,
				client->query.dboptions,
				client->now, &node, fname, &cm, &ci,
				rdataset, sigrdataset);
	if (result == ISC_R_SUCCESS) {
		if (sigrdataset != NULL && !dns_db_issecure(db) &&
		    dns_rdataset_isassociated(sigrdataset))
			dns_rdataset_disassociate(sigrdataset);
		goto found;
	}

	if (dns_rdataset_isassociated(rdataset))
		dns_rdataset_disassociate(rdataset);
	if (sigrdataset != NULL && dns_rdataset_isassociated(sigrdataset))
		dns_rdataset_disassociate(sigrdataset);
	if (node != NULL)
		dns_db_detachnode(db, &node);
	version = NULL;
	dns_db_detach(&db);

	/*
	 * No authoritative data was found.  The cache is our next best bet.
	 */

 try_cache:
	result = query_getcachedb(client, name, qtype, &db, DNS_GETDB_NOLOG);
	if (result != ISC_R_SUCCESS)
		/*
		 * Most likely the client isn't allowed to query the cache.
		 */
		goto try_glue;
	/*
	 * Attempt to validate glue.
	 */
	if (sigrdataset == NULL) {
		sigrdataset = query_newrdataset(client);
		if (sigrdataset == NULL)
			goto cleanup;
	}
	result = dns_db_findext(db, name, version, type,
				client->query.dboptions |
				 DNS_DBFIND_GLUEOK | DNS_DBFIND_ADDITIONALOK,
				client->now, &node, fname, &cm, &ci,
				rdataset, sigrdataset);

	dns_cache_updatestats(client->view->cache, result);
	if (result == DNS_R_GLUE &&
	    validate(client, db, fname, rdataset, sigrdataset))
		result = ISC_R_SUCCESS;
	if (!WANTDNSSEC(client))
		query_putrdataset(client, &sigrdataset);
	if (result == ISC_R_SUCCESS)
		goto found;


	if (dns_rdataset_isassociated(rdataset))
		dns_rdataset_disassociate(rdataset);
	if (sigrdataset != NULL && dns_rdataset_isassociated(sigrdataset))
		dns_rdataset_disassociate(sigrdataset);
	if (node != NULL)
		dns_db_detachnode(db, &node);
	dns_db_detach(&db);

 try_glue:
	/*
	 * No cached data was found.  Glue is our last chance.
	 * RFC1035 sayeth:
	 *
	 *	NS records cause both the usual additional section
	 *	processing to locate a type A record, and, when used
	 *	in a referral, a special search of the zone in which
	 *	they reside for glue information.
	 *
	 * This is the "special search".  Note that we must search
	 * the zone where the NS record resides, not the zone it
	 * points to, and that we only do the search in the delegation
	 * case (identified by client->query.gluedb being set).
	 */

	if (client->query.gluedb == NULL)
		goto cleanup;

	/*
	 * Don't poison caches using the bailiwick protection model.
	 */
	if (!dns_name_issubdomain(name, dns_db_origin(client->query.gluedb)))
		goto cleanup;

	dns_db_attach(client->query.gluedb, &db);
	result = dns_db_findext(db, name, version, type,
				client->query.dboptions | DNS_DBFIND_GLUEOK,
				client->now, &node, fname, &cm, &ci,
				rdataset, sigrdataset);
	if (!(result == ISC_R_SUCCESS ||
	      result == DNS_R_ZONECUT ||
	      result == DNS_R_GLUE))
		goto cleanup;

 found:
	/*
	 * We have found a potential additional data rdataset, or
	 * at least a node to iterate over.
	 */
	query_keepname(client, fname, dbuf);

	/*
	 * If we have an rdataset, add it to the additional data
	 * section.
	 */
	mname = NULL;
	if (dns_rdataset_isassociated(rdataset) &&
	    !query_isduplicate(client, fname, type, &mname)) {
		if (mname != NULL) {
			INSIST(mname != fname);
			query_releasename(client, &fname);
			fname = mname;
		} else
			need_addname = ISC_TRUE;
		ISC_LIST_APPEND(fname->list, rdataset, link);
		trdataset = rdataset;
		rdataset = NULL;
		added_something = ISC_TRUE;
		/*
		 * Note: we only add SIGs if we've added the type they cover,
		 * so we do not need to check if the SIG rdataset is already
		 * in the response.
		 */
		if (sigrdataset != NULL &&
		    dns_rdataset_isassociated(sigrdataset))
		{
			ISC_LIST_APPEND(fname->list, sigrdataset, link);
			sigrdataset = NULL;
		}
	}

	if (qtype == dns_rdatatype_a) {
#ifdef ALLOW_FILTER_AAAA
		isc_boolean_t have_a = ISC_FALSE;
#endif

		/*
		 * We now go looking for A and AAAA records, along with
		 * their signatures.
		 *
		 * XXXRTH  This code could be more efficient.
		 */
		if (rdataset != NULL) {
			if (dns_rdataset_isassociated(rdataset))
				dns_rdataset_disassociate(rdataset);
		} else {
			rdataset = query_newrdataset(client);
			if (rdataset == NULL)
				goto addname;
		}
		if (sigrdataset != NULL) {
			if (dns_rdataset_isassociated(sigrdataset))
				dns_rdataset_disassociate(sigrdataset);
		} else if (WANTDNSSEC(client)) {
			sigrdataset = query_newrdataset(client);
			if (sigrdataset == NULL)
				goto addname;
		}
		if (query_isduplicate(client, fname, dns_rdatatype_a, NULL))
			goto aaaa_lookup;
		result = dns_db_findrdataset(db, node, version,
					     dns_rdatatype_a, 0,
					     client->now,
					     rdataset, sigrdataset);
		if (result == DNS_R_NCACHENXDOMAIN)
			goto addname;
		if (result == DNS_R_NCACHENXRRSET) {
			dns_rdataset_disassociate(rdataset);
			if (sigrdataset != NULL &&
			    dns_rdataset_isassociated(sigrdataset))
				dns_rdataset_disassociate(sigrdataset);
		}
		if (result == ISC_R_SUCCESS) {
			mname = NULL;
#ifdef ALLOW_FILTER_AAAA
			have_a = ISC_TRUE;
#endif
			if (!query_isduplicate(client, fname,
					       dns_rdatatype_a, &mname)) {
				if (mname != fname) {
					if (mname != NULL) {
						query_releasename(client, &fname);
						fname = mname;
					} else
						need_addname = ISC_TRUE;
				}
				ISC_LIST_APPEND(fname->list, rdataset, link);
				added_something = ISC_TRUE;
				if (sigrdataset != NULL &&
				    dns_rdataset_isassociated(sigrdataset))
				{
					ISC_LIST_APPEND(fname->list,
							sigrdataset, link);
					sigrdataset =
						query_newrdataset(client);
				}
				rdataset = query_newrdataset(client);
				if (rdataset == NULL)
					goto addname;
				if (WANTDNSSEC(client) && sigrdataset == NULL)
					goto addname;
			} else {
				dns_rdataset_disassociate(rdataset);
				if (sigrdataset != NULL &&
				    dns_rdataset_isassociated(sigrdataset))
					dns_rdataset_disassociate(sigrdataset);
			}
		}
  aaaa_lookup:
		if (query_isduplicate(client, fname, dns_rdatatype_aaaa, NULL))
			goto addname;
		result = dns_db_findrdataset(db, node, version,
					     dns_rdatatype_aaaa, 0,
					     client->now,
					     rdataset, sigrdataset);
		if (result == DNS_R_NCACHENXDOMAIN)
			goto addname;
		if (result == DNS_R_NCACHENXRRSET) {
			dns_rdataset_disassociate(rdataset);
			if (sigrdataset != NULL &&
			    dns_rdataset_isassociated(sigrdataset))
				dns_rdataset_disassociate(sigrdataset);
		}
		if (result == ISC_R_SUCCESS) {
			mname = NULL;
			/*
			 * There's an A; check whether we're filtering AAAA
			 */
#ifdef ALLOW_FILTER_AAAA
			if (have_a &&
			    (client->filter_aaaa == dns_aaaa_break_dnssec ||
			    (client->filter_aaaa == dns_aaaa_filter &&
			     (!WANTDNSSEC(client) || sigrdataset == NULL ||
			      !dns_rdataset_isassociated(sigrdataset)))))
				goto addname;
#endif
			if (!query_isduplicate(client, fname,
					       dns_rdatatype_aaaa, &mname)) {
				if (mname != fname) {
					if (mname != NULL) {
						query_releasename(client, &fname);
						fname = mname;
					} else
						need_addname = ISC_TRUE;
				}
				ISC_LIST_APPEND(fname->list, rdataset, link);
				added_something = ISC_TRUE;
				if (sigrdataset != NULL &&
				    dns_rdataset_isassociated(sigrdataset))
				{
					ISC_LIST_APPEND(fname->list,
							sigrdataset, link);
					sigrdataset = NULL;
				}
				rdataset = NULL;
			}
		}
	}

 addname:
	CTRACE(ISC_LOG_DEBUG(3), "query_addadditional: addname");
	/*
	 * If we haven't added anything, then we're done.
	 */
	if (!added_something)
		goto cleanup;

	/*
	 * We may have added our rdatasets to an existing name, if so, then
	 * need_addname will be ISC_FALSE.  Whether we used an existing name
	 * or a new one, we must set fname to NULL to prevent cleanup.
	 */
	if (need_addname)
		dns_message_addname(client->message, fname,
				    DNS_SECTION_ADDITIONAL);
	fname = NULL;

	/*
	 * In a few cases, we want to add additional data for additional
	 * data.  It's simpler to just deal with special cases here than
	 * to try to create a general purpose mechanism and allow the
	 * rdata implementations to do it themselves.
	 *
	 * This involves recursion, but the depth is limited.  The
	 * most complex case is adding a SRV rdataset, which involves
	 * recursing to add address records, which in turn can cause
	 * recursion to add KEYs.
	 */
	if (type == dns_rdatatype_srv && trdataset != NULL) {
		/*
		 * If we're adding SRV records to the additional data
		 * section, it's helpful if we add the SRV additional data
		 * as well.
		 */
		eresult = dns_rdataset_additionaldata(trdataset,
						      query_addadditional,
						      client);
	}

 cleanup:
	CTRACE(ISC_LOG_DEBUG(3), "query_addadditional: cleanup");
	query_putrdataset(client, &rdataset);
	if (sigrdataset != NULL)
		query_putrdataset(client, &sigrdataset);
	if (fname != NULL)
		query_releasename(client, &fname);
	if (node != NULL)
		dns_db_detachnode(db, &node);
	if (db != NULL)
		dns_db_detach(&db);
	if (zone != NULL)
		dns_zone_detach(&zone);

	CTRACE(ISC_LOG_DEBUG(3), "query_addadditional: done");
	return (eresult);
}

static inline void
query_discardcache(ns_client_t *client, dns_rdataset_t *rdataset_base,
		   dns_rdatasetadditional_t additionaltype,
		   dns_rdatatype_t type, dns_zone_t **zonep, dns_db_t **dbp,
		   dns_dbversion_t **versionp, dns_dbnode_t **nodep,
		   dns_name_t *fname)
{
	dns_rdataset_t *rdataset;

	while  ((rdataset = ISC_LIST_HEAD(fname->list)) != NULL) {
		ISC_LIST_UNLINK(fname->list, rdataset, link);
		query_putrdataset(client, &rdataset);
	}
	if (*versionp != NULL)
		dns_db_closeversion(*dbp, versionp, ISC_FALSE);
	if (*nodep != NULL)
		dns_db_detachnode(*dbp, nodep);
	if (*dbp != NULL)
		dns_db_detach(dbp);
	if (*zonep != NULL)
		dns_zone_detach(zonep);
	(void)dns_rdataset_putadditional(client->view->acache, rdataset_base,
					 additionaltype, type);
}

static inline isc_result_t
query_iscachevalid(dns_zone_t *zone, dns_db_t *db, dns_db_t *db0,
		   dns_dbversion_t *version)
{
	isc_result_t result = ISC_R_SUCCESS;
	dns_dbversion_t *version_current = NULL;
	dns_db_t *db_current = db0;

	if (db_current == NULL) {
		result = dns_zone_getdb(zone, &db_current);
		if (result != ISC_R_SUCCESS)
			return (result);
	}
	dns_db_currentversion(db_current, &version_current);
	if (db_current != db || version_current != version) {
		result = ISC_R_FAILURE;
		goto cleanup;
	}

 cleanup:
	dns_db_closeversion(db_current, &version_current, ISC_FALSE);
	if (db0 == NULL && db_current != NULL)
		dns_db_detach(&db_current);

	return (result);
}

static isc_result_t
query_addadditional2(void *arg, dns_name_t *name, dns_rdatatype_t qtype) {
	client_additionalctx_t *additionalctx = arg;
	dns_rdataset_t *rdataset_base;
	ns_client_t *client;
	isc_result_t result, eresult;
	dns_dbnode_t *node, *cnode;
	dns_db_t *db, *cdb;
	dns_name_t *fname, *mname0, cfname;
	dns_rdataset_t *rdataset, *sigrdataset;
	dns_rdataset_t *crdataset, *crdataset_next;
	isc_buffer_t *dbuf;
	isc_buffer_t b;
	dns_dbversion_t *version, *cversion;
	isc_boolean_t added_something, need_addname, needadditionalcache;
	isc_boolean_t need_sigrrset;
	dns_zone_t *zone;
	dns_rdatatype_t type;
	dns_rdatasetadditional_t additionaltype;
	dns_clientinfomethods_t cm;
	dns_clientinfo_t ci;

	/*
	 * If we don't have an additional cache call query_addadditional.
	 */
	client = additionalctx->client;
	REQUIRE(NS_CLIENT_VALID(client));

	if (qtype != dns_rdatatype_a || client->view->acache == NULL) {
		/*
		 * This function is optimized for "address" types.  For other
		 * types, use a generic routine.
		 * XXX: ideally, this function should be generic enough.
		 */
		return (query_addadditional(additionalctx->client,
					    name, qtype));
	}

	/*
	 * Initialization.
	 */
	rdataset_base = additionalctx->rdataset;
	eresult = ISC_R_SUCCESS;
	fname = NULL;
	rdataset = NULL;
	sigrdataset = NULL;
	db = NULL;
	cdb = NULL;
	version = NULL;
	cversion = NULL;
	node = NULL;
	cnode = NULL;
	added_something = ISC_FALSE;
	need_addname = ISC_FALSE;
	zone = NULL;
	needadditionalcache = ISC_FALSE;
	POST(needadditionalcache);
	additionaltype = dns_rdatasetadditional_fromauth;
	dns_name_init(&cfname, NULL);
	dns_clientinfomethods_init(&cm, ns_client_sourceip);
	dns_clientinfo_init(&ci, client);

	CTRACE(ISC_LOG_DEBUG(3), "query_addadditional2");

	/*
	 * We treat type A additional section processing as if it
	 * were "any address type" additional section processing.
	 * To avoid multiple lookups, we do an 'any' database
	 * lookup and iterate over the node.
	 * XXXJT: this approach can cause a suboptimal result when the cache
	 * DB only has partial address types and the glue DB has remaining
	 * ones.
	 */
	type = dns_rdatatype_any;

	/*
	 * Get some resources.
	 */
	dbuf = query_getnamebuf(client);
	if (dbuf == NULL)
		goto cleanup;
	fname = query_newname(client, dbuf, &b);
	if (fname == NULL)
		goto cleanup;
	dns_name_setbuffer(&cfname, &b); /* share the buffer */

	/* Check additional cache */
	result = dns_rdataset_getadditional(rdataset_base, additionaltype,
					    type, client->view->acache, &zone,
					    &cdb, &cversion, &cnode, &cfname,
					    client->message, client->now);
	if (result != ISC_R_SUCCESS)
		goto findauthdb;
	if (zone == NULL) {
		CTRACE(ISC_LOG_DEBUG(3),
		       "query_addadditional2: auth zone not found");
		goto try_cache;
	}

	/* Is the cached DB up-to-date? */
	result = query_iscachevalid(zone, cdb, NULL, cversion);
	if (result != ISC_R_SUCCESS) {
		CTRACE(ISC_LOG_DEBUG(3),
		       "query_addadditional2: old auth additional cache");
		query_discardcache(client, rdataset_base, additionaltype,
				   type, &zone, &cdb, &cversion, &cnode,
				   &cfname);
		goto findauthdb;
	}

	if (cnode == NULL) {
		/*
		 * We have a negative cache.  We don't have to check the zone
		 * ACL, since the result (not using this zone) would be same
		 * regardless of the result.
		 */
		CTRACE(ISC_LOG_DEBUG(3),
		       "query_addadditional2: negative auth additional cache");
		dns_db_closeversion(cdb, &cversion, ISC_FALSE);
		dns_db_detach(&cdb);
		dns_zone_detach(&zone);
		goto try_cache;
	}

	result = query_validatezonedb(client, name, qtype, DNS_GETDB_NOLOG,
				      zone, cdb, NULL);
	if (result != ISC_R_SUCCESS) {
		query_discardcache(client, rdataset_base, additionaltype,
				   type, &zone, &cdb, &cversion, &cnode,
				   &cfname);
		goto try_cache;
	}

	/* We've got an active cache. */
	CTRACE(ISC_LOG_DEBUG(3),
	       "query_addadditional2: auth additional cache");
	dns_db_closeversion(cdb, &cversion, ISC_FALSE);
	db = cdb;
	node = cnode;
	dns_name_clone(&cfname, fname);
	query_keepname(client, fname, dbuf);
	goto foundcache;

	/*
	 * Look for a zone database that might contain authoritative
	 * additional data.
	 */
 findauthdb:
	result = query_getzonedb(client, name, qtype, DNS_GETDB_NOLOG,
				 &zone, &db, &version);
	if (result != ISC_R_SUCCESS) {
		/* Cache the negative result */
		(void)dns_rdataset_setadditional(rdataset_base, additionaltype,
						 type, client->view->acache,
						 NULL, NULL, NULL, NULL,
						 NULL);
		goto try_cache;
	}

	CTRACE(ISC_LOG_DEBUG(3), "query_addadditional2: db_find");

	/*
	 * Since we are looking for authoritative data, we do not set
	 * the GLUEOK flag.  Glue will be looked for later, but not
	 * necessarily in the same database.
	 */
	node = NULL;
	result = dns_db_findext(db, name, version, type,
				client->query.dboptions,
				client->now, &node, fname, &cm, &ci,
				NULL, NULL);
	if (result == ISC_R_SUCCESS)
		goto found;

	/* Cache the negative result */
	(void)dns_rdataset_setadditional(rdataset_base, additionaltype,
					 type, client->view->acache, zone, db,
					 version, NULL, fname);

	if (node != NULL)
		dns_db_detachnode(db, &node);
	version = NULL;
	dns_db_detach(&db);

	/*
	 * No authoritative data was found.  The cache is our next best bet.
	 */

 try_cache:
	additionaltype = dns_rdatasetadditional_fromcache;
	result = query_getcachedb(client, name, qtype, &db, DNS_GETDB_NOLOG);
	if (result != ISC_R_SUCCESS)
		/*
		 * Most likely the client isn't allowed to query the cache.
		 */
		goto try_glue;

	result = dns_db_findext(db, name, version, type,
				client->query.dboptions |
				 DNS_DBFIND_GLUEOK | DNS_DBFIND_ADDITIONALOK,
				client->now, &node, fname, &cm, &ci,
				NULL, NULL);
	if (result == ISC_R_SUCCESS)
		goto found;

	if (node != NULL)
		dns_db_detachnode(db, &node);
	dns_db_detach(&db);

 try_glue:
	/*
	 * No cached data was found.  Glue is our last chance.
	 * RFC1035 sayeth:
	 *
	 *	NS records cause both the usual additional section
	 *	processing to locate a type A record, and, when used
	 *	in a referral, a special search of the zone in which
	 *	they reside for glue information.
	 *
	 * This is the "special search".  Note that we must search
	 * the zone where the NS record resides, not the zone it
	 * points to, and that we only do the search in the delegation
	 * case (identified by client->query.gluedb being set).
	 */
	if (client->query.gluedb == NULL)
		goto cleanup;

	/*
	 * Don't poison caches using the bailiwick protection model.
	 */
	if (!dns_name_issubdomain(name, dns_db_origin(client->query.gluedb)))
		goto cleanup;

	/* Check additional cache */
	additionaltype = dns_rdatasetadditional_fromglue;
	result = dns_rdataset_getadditional(rdataset_base, additionaltype,
					    type, client->view->acache, NULL,
					    &cdb, &cversion, &cnode, &cfname,
					    client->message, client->now);
	if (result != ISC_R_SUCCESS)
		goto findglue;

	result = query_iscachevalid(zone, cdb, client->query.gluedb, cversion);
	if (result != ISC_R_SUCCESS) {
		CTRACE(ISC_LOG_DEBUG(3),
		       "query_addadditional2: old glue additional cache");
		query_discardcache(client, rdataset_base, additionaltype,
				   type, &zone, &cdb, &cversion, &cnode,
				   &cfname);
		goto findglue;
	}

	if (cnode == NULL) {
		/* We have a negative cache. */
		CTRACE(ISC_LOG_DEBUG(3),
		       "query_addadditional2: negative glue additional cache");
		dns_db_closeversion(cdb, &cversion, ISC_FALSE);
		dns_db_detach(&cdb);
		goto cleanup;
	}

	/* Cache hit. */
	CTRACE(ISC_LOG_DEBUG(3), "query_addadditional2: glue additional cache");
	dns_db_closeversion(cdb, &cversion, ISC_FALSE);
	db = cdb;
	node = cnode;
	dns_name_clone(&cfname, fname);
	query_keepname(client, fname, dbuf);
	goto foundcache;

 findglue:
	dns_db_attach(client->query.gluedb, &db);
	result = dns_db_findext(db, name, version, type,
				client->query.dboptions | DNS_DBFIND_GLUEOK,
				client->now, &node, fname, &cm, &ci,
				NULL, NULL);
	if (!(result == ISC_R_SUCCESS ||
	      result == DNS_R_ZONECUT ||
	      result == DNS_R_GLUE)) {
		/* cache the negative result */
		(void)dns_rdataset_setadditional(rdataset_base, additionaltype,
						 type, client->view->acache,
						 NULL, db, version, NULL,
						 fname);
		goto cleanup;
	}

 found:
	/*
	 * We have found a DB node to iterate over from a DB.
	 * We are going to look for address RRsets (i.e., A and AAAA) in the DB
	 * node we've just found.  We'll then store the complete information
	 * in the additional data cache.
	 */
	dns_name_clone(fname, &cfname);
	query_keepname(client, fname, dbuf);
	needadditionalcache = ISC_TRUE;

	rdataset = query_newrdataset(client);
	if (rdataset == NULL)
		goto cleanup;

	sigrdataset = query_newrdataset(client);
	if (sigrdataset == NULL)
		goto cleanup;

	if (additionaltype == dns_rdatasetadditional_fromcache &&
	    query_isduplicate(client, fname, dns_rdatatype_a, NULL))
		goto aaaa_lookup;
	/*
	 * Find A RRset with sig RRset.  Even if we don't find a sig RRset
	 * for a client using DNSSEC, we'll continue the process to make a
	 * complete list to be cached.  However, we need to cancel the
	 * caching when something unexpected happens, in order to avoid
	 * caching incomplete information.
	 */
	result = dns_db_findrdataset(db, node, version, dns_rdatatype_a, 0,
				     client->now, rdataset, sigrdataset);
	/*
	 * If we can't promote glue/pending from the cache to secure
	 * then drop it.
	 */
	if (result == ISC_R_SUCCESS &&
	    additionaltype == dns_rdatasetadditional_fromcache &&
	    (DNS_TRUST_PENDING(rdataset->trust) ||
	     DNS_TRUST_GLUE(rdataset->trust)) &&
	    !validate(client, db, fname, rdataset, sigrdataset)) {
		dns_rdataset_disassociate(rdataset);
		if (dns_rdataset_isassociated(sigrdataset))
			dns_rdataset_disassociate(sigrdataset);
		result = ISC_R_NOTFOUND;
	}
	if (result == DNS_R_NCACHENXDOMAIN)
		goto setcache;
	if (result == DNS_R_NCACHENXRRSET) {
		dns_rdataset_disassociate(rdataset);
		if (dns_rdataset_isassociated(sigrdataset))
			dns_rdataset_disassociate(sigrdataset);
	}
	if (result == ISC_R_SUCCESS) {
		/* Remember the result as a cache */
		ISC_LIST_APPEND(cfname.list, rdataset, link);
		if (dns_rdataset_isassociated(sigrdataset)) {
			ISC_LIST_APPEND(cfname.list, sigrdataset, link);
			sigrdataset = query_newrdataset(client);
		}
		rdataset = query_newrdataset(client);
		if (sigrdataset == NULL || rdataset == NULL) {
			/* do not cache incomplete information */
			goto foundcache;
		}
	}

 aaaa_lookup:
	if (additionaltype == dns_rdatasetadditional_fromcache &&
	    query_isduplicate(client, fname, dns_rdatatype_aaaa, NULL))
		goto foundcache;
	/* Find AAAA RRset with sig RRset */
	result = dns_db_findrdataset(db, node, version, dns_rdatatype_aaaa,
				     0, client->now, rdataset, sigrdataset);
	/*
	 * If we can't promote glue/pending from the cache to secure
	 * then drop it.
	 */
	if (result == ISC_R_SUCCESS &&
	    additionaltype == dns_rdatasetadditional_fromcache &&
	    (DNS_TRUST_PENDING(rdataset->trust) ||
	     DNS_TRUST_GLUE(rdataset->trust)) &&
	    !validate(client, db, fname, rdataset, sigrdataset)) {
		dns_rdataset_disassociate(rdataset);
		if (dns_rdataset_isassociated(sigrdataset))
			dns_rdataset_disassociate(sigrdataset);
		result = ISC_R_NOTFOUND;
	}
	if (result == ISC_R_SUCCESS) {
		ISC_LIST_APPEND(cfname.list, rdataset, link);
		rdataset = NULL;
		if (dns_rdataset_isassociated(sigrdataset)) {
			ISC_LIST_APPEND(cfname.list, sigrdataset, link);
			sigrdataset = NULL;
		}
	}

 setcache:
	/*
	 * Set the new result in the cache if required.  We do not support
	 * caching additional data from a cache DB.
	 */
	if (needadditionalcache == ISC_TRUE &&
	    (additionaltype == dns_rdatasetadditional_fromauth ||
	     additionaltype == dns_rdatasetadditional_fromglue)) {
		(void)dns_rdataset_setadditional(rdataset_base, additionaltype,
						 type, client->view->acache,
						 zone, db, version, node,
						 &cfname);
	}

 foundcache:
	need_sigrrset = ISC_FALSE;
	mname0 = NULL;
	for (crdataset = ISC_LIST_HEAD(cfname.list);
	     crdataset != NULL;
	     crdataset = crdataset_next) {
		dns_name_t *mname;

		crdataset_next = ISC_LIST_NEXT(crdataset, link);

		mname = NULL;
		if (crdataset->type == dns_rdatatype_a ||
		    crdataset->type == dns_rdatatype_aaaa) {
			if (!query_isduplicate(client, fname, crdataset->type,
					       &mname)) {
				if (mname != fname) {
					if (mname != NULL) {
						/*
						 * A different type of this name is
						 * already stored in the additional
						 * section.  We'll reuse the name.
						 * Note that this should happen at most
						 * once.  Otherwise, fname->link could
						 * leak below.
						 */
						INSIST(mname0 == NULL);

						query_releasename(client, &fname);
						fname = mname;
						mname0 = mname;
					} else
						need_addname = ISC_TRUE;
				}
				ISC_LIST_UNLINK(cfname.list, crdataset, link);
				ISC_LIST_APPEND(fname->list, crdataset, link);
				added_something = ISC_TRUE;
				need_sigrrset = ISC_TRUE;
			} else
				need_sigrrset = ISC_FALSE;
		} else if (crdataset->type == dns_rdatatype_rrsig &&
			   need_sigrrset && WANTDNSSEC(client)) {
			ISC_LIST_UNLINK(cfname.list, crdataset, link);
			ISC_LIST_APPEND(fname->list, crdataset, link);
			added_something = ISC_TRUE; /* just in case */
			need_sigrrset = ISC_FALSE;
		}
	}

	CTRACE(ISC_LOG_DEBUG(3), "query_addadditional2: addname");

	/*
	 * If we haven't added anything, then we're done.
	 */
	if (!added_something)
		goto cleanup;

	/*
	 * We may have added our rdatasets to an existing name, if so, then
	 * need_addname will be ISC_FALSE.  Whether we used an existing name
	 * or a new one, we must set fname to NULL to prevent cleanup.
	 */
	if (need_addname)
		dns_message_addname(client->message, fname,
				    DNS_SECTION_ADDITIONAL);
	fname = NULL;

 cleanup:
	CTRACE(ISC_LOG_DEBUG(3), "query_addadditional2: cleanup");

	if (rdataset != NULL)
		query_putrdataset(client, &rdataset);
	if (sigrdataset != NULL)
		query_putrdataset(client, &sigrdataset);
	while  ((crdataset = ISC_LIST_HEAD(cfname.list)) != NULL) {
		ISC_LIST_UNLINK(cfname.list, crdataset, link);
		query_putrdataset(client, &crdataset);
	}
	if (fname != NULL)
		query_releasename(client, &fname);
	if (node != NULL)
		dns_db_detachnode(db, &node);
	if (db != NULL)
		dns_db_detach(&db);
	if (zone != NULL)
		dns_zone_detach(&zone);

	CTRACE(ISC_LOG_DEBUG(3), "query_addadditional2: done");
	return (eresult);
}

static inline void
query_addrdataset(ns_client_t *client, dns_name_t *fname,
		  dns_rdataset_t *rdataset)
{
	client_additionalctx_t additionalctx;

	/*
	 * Add 'rdataset' and any pertinent additional data to
	 * 'fname', a name in the response message for 'client'.
	 */

	CTRACE(ISC_LOG_DEBUG(3), "query_addrdataset");

	ISC_LIST_APPEND(fname->list, rdataset, link);

	if (client->view->order != NULL)
		rdataset->attributes |= dns_order_find(client->view->order,
						       fname, rdataset->type,
						       rdataset->rdclass);
	rdataset->attributes |= DNS_RDATASETATTR_LOADORDER;

	if (NOADDITIONAL(client))
		return;

	/*
	 * Add additional data.
	 *
	 * We don't care if dns_rdataset_additionaldata() fails.
	 */
	additionalctx.client = client;
	additionalctx.rdataset = rdataset;
	(void)dns_rdataset_additionaldata(rdataset, query_addadditional2,
					  &additionalctx);
	CTRACE(ISC_LOG_DEBUG(3), "query_addrdataset: done");
}

static isc_result_t
query_dns64(ns_client_t *client, dns_name_t **namep, dns_rdataset_t *rdataset,
	    dns_rdataset_t *sigrdataset, isc_buffer_t *dbuf,
	    dns_section_t section)
{
	dns_name_t *name, *mname;
	dns_rdata_t *dns64_rdata;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdatalist_t *dns64_rdatalist;
	dns_rdataset_t *dns64_rdataset;
	dns_rdataset_t *mrdataset;
	isc_buffer_t *buffer;
	isc_region_t r;
	isc_result_t result;
	dns_view_t *view = client->view;
	isc_netaddr_t netaddr;
	dns_dns64_t *dns64;
	unsigned int flags = 0;

	/*%
	 * To the current response for 'client', add the answer RRset
	 * '*rdatasetp' and an optional signature set '*sigrdatasetp', with
	 * owner name '*namep', to section 'section', unless they are
	 * already there.  Also add any pertinent additional data.
	 *
	 * If 'dbuf' is not NULL, then '*namep' is the name whose data is
	 * stored in 'dbuf'.  In this case, query_addrrset() guarantees that
	 * when it returns the name will either have been kept or released.
	 */
	CTRACE(ISC_LOG_DEBUG(3), "query_dns64");
	name = *namep;
	mname = NULL;
	mrdataset = NULL;
	buffer = NULL;
	dns64_rdata = NULL;
	dns64_rdataset = NULL;
	dns64_rdatalist = NULL;
	result = dns_message_findname(client->message, section,
				      name, dns_rdatatype_aaaa,
				      rdataset->covers,
				      &mname, &mrdataset);
	if (result == ISC_R_SUCCESS) {
		/*
		 * We've already got an RRset of the given name and type.
		 * There's nothing else to do;
		 */
		CTRACE(ISC_LOG_DEBUG(3),
		       "query_dns64: dns_message_findname succeeded: done");
		if (dbuf != NULL)
			query_releasename(client, namep);
		return (ISC_R_SUCCESS);
	} else if (result == DNS_R_NXDOMAIN) {
		/*
		 * The name doesn't exist.
		 */
		if (dbuf != NULL)
			query_keepname(client, name, dbuf);
		dns_message_addname(client->message, name, section);
		*namep = NULL;
		mname = name;
	} else {
		RUNTIME_CHECK(result == DNS_R_NXRRSET);
		if (dbuf != NULL)
			query_releasename(client, namep);
	}

	if (rdataset->trust != dns_trust_secure &&
	    (section == DNS_SECTION_ANSWER ||
	     section == DNS_SECTION_AUTHORITY))
		client->query.attributes &= ~NS_QUERYATTR_SECURE;

	isc_netaddr_fromsockaddr(&netaddr, &client->peeraddr);

	result = isc_buffer_allocate(client->mctx, &buffer, view->dns64cnt *
				     16 * dns_rdataset_count(rdataset));
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	result = dns_message_gettemprdataset(client->message, &dns64_rdataset);
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	result = dns_message_gettemprdatalist(client->message,
					      &dns64_rdatalist);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	dns_rdatalist_init(dns64_rdatalist);
	dns64_rdatalist->rdclass = dns_rdataclass_in;
	dns64_rdatalist->type = dns_rdatatype_aaaa;
	if (client->query.dns64_ttl != ISC_UINT32_MAX)
		dns64_rdatalist->ttl = ISC_MIN(rdataset->ttl,
					       client->query.dns64_ttl);
	else
		dns64_rdatalist->ttl = ISC_MIN(rdataset->ttl, 600);

	if (RECURSIONOK(client))
		flags |= DNS_DNS64_RECURSIVE;

	/*
	 * We use the signatures from the A lookup to set DNS_DNS64_DNSSEC
	 * as this provides a easy way to see if the answer was signed.
	 */
	if (sigrdataset != NULL && dns_rdataset_isassociated(sigrdataset))
		flags |= DNS_DNS64_DNSSEC;

	for (result = dns_rdataset_first(rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(rdataset)) {
		for (dns64 = ISC_LIST_HEAD(client->view->dns64);
		     dns64 != NULL; dns64 = dns_dns64_next(dns64)) {

			dns_rdataset_current(rdataset, &rdata);
			isc_buffer_availableregion(buffer, &r);
			INSIST(r.length >= 16);
			result = dns_dns64_aaaafroma(dns64, &netaddr,
						     client->signer,
						     &ns_g_server->aclenv,
						     flags, rdata.data, r.base);
			if (result != ISC_R_SUCCESS) {
				dns_rdata_reset(&rdata);
				continue;
			}
			isc_buffer_add(buffer, 16);
			isc_buffer_remainingregion(buffer, &r);
			isc_buffer_forward(buffer, 16);
			result = dns_message_gettemprdata(client->message,
							  &dns64_rdata);
			if (result != ISC_R_SUCCESS)
				goto cleanup;
			dns_rdata_init(dns64_rdata);
			dns_rdata_fromregion(dns64_rdata, dns_rdataclass_in,
					     dns_rdatatype_aaaa, &r);
			ISC_LIST_APPEND(dns64_rdatalist->rdata, dns64_rdata,
					link);
			dns64_rdata = NULL;
			dns_rdata_reset(&rdata);
		}
	}
	if (result != ISC_R_NOMORE)
		goto cleanup;

	if (ISC_LIST_EMPTY(dns64_rdatalist->rdata))
		goto cleanup;

	result = dns_rdatalist_tordataset(dns64_rdatalist, dns64_rdataset);
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	client->query.attributes |= NS_QUERYATTR_NOADDITIONAL;
	dns64_rdataset->trust = rdataset->trust;
	query_addrdataset(client, mname, dns64_rdataset);
	dns64_rdataset = NULL;
	dns64_rdatalist = NULL;
	dns_message_takebuffer(client->message, &buffer);
	inc_stats(client, dns_nsstatscounter_dns64);
	result = ISC_R_SUCCESS;

 cleanup:
	if (buffer != NULL)
		isc_buffer_free(&buffer);

	if (dns64_rdata != NULL)
		dns_message_puttemprdata(client->message, &dns64_rdata);

	if (dns64_rdataset != NULL)
		dns_message_puttemprdataset(client->message, &dns64_rdataset);

	if (dns64_rdatalist != NULL) {
		for (dns64_rdata = ISC_LIST_HEAD(dns64_rdatalist->rdata);
		     dns64_rdata != NULL;
		     dns64_rdata = ISC_LIST_HEAD(dns64_rdatalist->rdata))
		{
			ISC_LIST_UNLINK(dns64_rdatalist->rdata,
					dns64_rdata, link);
			dns_message_puttemprdata(client->message, &dns64_rdata);
		}
		dns_message_puttemprdatalist(client->message, &dns64_rdatalist);
	}

	CTRACE(ISC_LOG_DEBUG(3), "query_dns64: done");
	return (result);
}

static void
query_filter64(ns_client_t *client, dns_name_t **namep,
	       dns_rdataset_t *rdataset, isc_buffer_t *dbuf,
	       dns_section_t section)
{
	dns_name_t *name, *mname;
	dns_rdata_t *myrdata;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdatalist_t *myrdatalist;
	dns_rdataset_t *myrdataset;
	isc_buffer_t *buffer;
	isc_region_t r;
	isc_result_t result;
	unsigned int i;

	CTRACE(ISC_LOG_DEBUG(3), "query_filter64");

	INSIST(client->query.dns64_aaaaok != NULL);
	INSIST(client->query.dns64_aaaaoklen == dns_rdataset_count(rdataset));

	name = *namep;
	mname = NULL;
	buffer = NULL;
	myrdata = NULL;
	myrdataset = NULL;
	myrdatalist = NULL;
	result = dns_message_findname(client->message, section,
				      name, dns_rdatatype_aaaa,
				      rdataset->covers,
				      &mname, &myrdataset);
	if (result == ISC_R_SUCCESS) {
		/*
		 * We've already got an RRset of the given name and type.
		 * There's nothing else to do;
		 */
		CTRACE(ISC_LOG_DEBUG(3),
		       "query_filter64: dns_message_findname succeeded: done");
		if (dbuf != NULL)
			query_releasename(client, namep);
		return;
	} else if (result == DNS_R_NXDOMAIN) {
		mname = name;
		*namep = NULL;
	} else {
		RUNTIME_CHECK(result == DNS_R_NXRRSET);
		if (dbuf != NULL)
			query_releasename(client, namep);
		dbuf = NULL;
	}

	if (rdataset->trust != dns_trust_secure &&
	    (section == DNS_SECTION_ANSWER ||
	     section == DNS_SECTION_AUTHORITY))
		client->query.attributes &= ~NS_QUERYATTR_SECURE;

	result = isc_buffer_allocate(client->mctx, &buffer,
				     16 * dns_rdataset_count(rdataset));
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	result = dns_message_gettemprdataset(client->message, &myrdataset);
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	result = dns_message_gettemprdatalist(client->message, &myrdatalist);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	dns_rdatalist_init(myrdatalist);
	myrdatalist->rdclass = dns_rdataclass_in;
	myrdatalist->type = dns_rdatatype_aaaa;
	myrdatalist->ttl = rdataset->ttl;

	i = 0;
	for (result = dns_rdataset_first(rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(rdataset)) {
		if (!client->query.dns64_aaaaok[i++])
			continue;
		dns_rdataset_current(rdataset, &rdata);
		INSIST(rdata.length == 16);
		isc_buffer_putmem(buffer, rdata.data, rdata.length);
		isc_buffer_remainingregion(buffer, &r);
		isc_buffer_forward(buffer, rdata.length);
		result = dns_message_gettemprdata(client->message, &myrdata);
		if (result != ISC_R_SUCCESS)
			goto cleanup;
		dns_rdata_init(myrdata);
		dns_rdata_fromregion(myrdata, dns_rdataclass_in,
				     dns_rdatatype_aaaa, &r);
		ISC_LIST_APPEND(myrdatalist->rdata, myrdata, link);
		myrdata = NULL;
		dns_rdata_reset(&rdata);
	}
	if (result != ISC_R_NOMORE)
		goto cleanup;

	result = dns_rdatalist_tordataset(myrdatalist, myrdataset);
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	client->query.attributes |= NS_QUERYATTR_NOADDITIONAL;
	if (mname == name) {
		if (dbuf != NULL)
			query_keepname(client, name, dbuf);
		dns_message_addname(client->message, name, section);
		dbuf = NULL;
	}
	myrdataset->trust = rdataset->trust;
	query_addrdataset(client, mname, myrdataset);
	myrdataset = NULL;
	myrdatalist = NULL;
	dns_message_takebuffer(client->message, &buffer);

 cleanup:
	if (buffer != NULL)
		isc_buffer_free(&buffer);

	if (myrdata != NULL)
		dns_message_puttemprdata(client->message, &myrdata);

	if (myrdataset != NULL)
		dns_message_puttemprdataset(client->message, &myrdataset);

	if (myrdatalist != NULL) {
		for (myrdata = ISC_LIST_HEAD(myrdatalist->rdata);
		     myrdata != NULL;
		     myrdata = ISC_LIST_HEAD(myrdatalist->rdata))
		{
			ISC_LIST_UNLINK(myrdatalist->rdata, myrdata, link);
			dns_message_puttemprdata(client->message, &myrdata);
		}
		dns_message_puttemprdatalist(client->message, &myrdatalist);
	}
	if (dbuf != NULL)
		query_releasename(client, &name);

	CTRACE(ISC_LOG_DEBUG(3), "query_filter64: done");
}

static void
query_addrrset(ns_client_t *client, dns_name_t **namep,
	       dns_rdataset_t **rdatasetp, dns_rdataset_t **sigrdatasetp,
	       isc_buffer_t *dbuf, dns_section_t section)
{
	dns_name_t *name, *mname;
	dns_rdataset_t *rdataset, *mrdataset, *sigrdataset;
	isc_result_t result;

	/*%
	 * To the current response for 'client', add the answer RRset
	 * '*rdatasetp' and an optional signature set '*sigrdatasetp', with
	 * owner name '*namep', to section 'section', unless they are
	 * already there.  Also add any pertinent additional data.
	 *
	 * If 'dbuf' is not NULL, then '*namep' is the name whose data is
	 * stored in 'dbuf'.  In this case, query_addrrset() guarantees that
	 * when it returns the name will either have been kept or released.
	 */
	CTRACE(ISC_LOG_DEBUG(3), "query_addrrset");
	name = *namep;
	rdataset = *rdatasetp;
	if (sigrdatasetp != NULL)
		sigrdataset = *sigrdatasetp;
	else
		sigrdataset = NULL;
	mname = NULL;
	mrdataset = NULL;
	result = dns_message_findname(client->message, section,
				      name, rdataset->type, rdataset->covers,
				      &mname, &mrdataset);
	if (result == ISC_R_SUCCESS) {
		/*
		 * We've already got an RRset of the given name and type.
		 */
		CTRACE(ISC_LOG_DEBUG(3),
		       "query_addrrset: dns_message_findname succeeded: done");
		if (dbuf != NULL)
			query_releasename(client, namep);
		if ((rdataset->attributes & DNS_RDATASETATTR_REQUIRED) != 0)
			mrdataset->attributes |= DNS_RDATASETATTR_REQUIRED;
		return;
	} else if (result == DNS_R_NXDOMAIN) {
		/*
		 * The name doesn't exist.
		 */
		if (dbuf != NULL)
			query_keepname(client, name, dbuf);
		dns_message_addname(client->message, name, section);
		*namep = NULL;
		mname = name;
	} else {
		RUNTIME_CHECK(result == DNS_R_NXRRSET);
		if (dbuf != NULL)
			query_releasename(client, namep);
	}

	if (rdataset->trust != dns_trust_secure &&
	    (section == DNS_SECTION_ANSWER ||
	     section == DNS_SECTION_AUTHORITY))
		client->query.attributes &= ~NS_QUERYATTR_SECURE;
	/*
	 * Note: we only add SIGs if we've added the type they cover, so
	 * we do not need to check if the SIG rdataset is already in the
	 * response.
	 */
	query_addrdataset(client, mname, rdataset);
	*rdatasetp = NULL;
	if (sigrdataset != NULL && dns_rdataset_isassociated(sigrdataset)) {
		/*
		 * We have a signature.  Add it to the response.
		 */
		ISC_LIST_APPEND(mname->list, sigrdataset, link);
		*sigrdatasetp = NULL;
	}
	CTRACE(ISC_LOG_DEBUG(3), "query_addrrset: done");
}

static inline isc_result_t
query_addsoa(ns_client_t *client, dns_db_t *db, dns_dbversion_t *version,
	     unsigned int override_ttl, isc_boolean_t isassociated,
	     dns_section_t section)
{
	dns_name_t *name;
	dns_dbnode_t *node;
	isc_result_t result, eresult;
	dns_rdataset_t *rdataset = NULL, *sigrdataset = NULL;
	dns_rdataset_t **sigrdatasetp = NULL;
	dns_clientinfomethods_t cm;
	dns_clientinfo_t ci;

	CTRACE(ISC_LOG_DEBUG(3), "query_addsoa");
	/*
	 * Initialization.
	 */
	eresult = ISC_R_SUCCESS;
	name = NULL;
	rdataset = NULL;
	node = NULL;

	dns_clientinfomethods_init(&cm, ns_client_sourceip);
	dns_clientinfo_init(&ci, client);

	/*
	 * Don't add the SOA record for test which set "-T nosoa".
	 */
	if (ns_g_nosoa && (!WANTDNSSEC(client) || !isassociated))
		return (ISC_R_SUCCESS);

	/*
	 * Get resources and make 'name' be the database origin.
	 */
	result = dns_message_gettempname(client->message, &name);
	if (result != ISC_R_SUCCESS)
		return (result);
	dns_name_init(name, NULL);
	dns_name_clone(dns_db_origin(db), name);
	rdataset = query_newrdataset(client);
	if (rdataset == NULL) {
		CTRACE(ISC_LOG_ERROR, "unable to allocate rdataset");
		eresult = DNS_R_SERVFAIL;
		goto cleanup;
	}
	if (WANTDNSSEC(client) && dns_db_issecure(db)) {
		sigrdataset = query_newrdataset(client);
		if (sigrdataset == NULL) {
			CTRACE(ISC_LOG_ERROR, "unable to allocate sigrdataset");
			eresult = DNS_R_SERVFAIL;
			goto cleanup;
		}
	}

	/*
	 * Find the SOA.
	 */
	result = dns_db_getoriginnode(db, &node);
	if (result == ISC_R_SUCCESS) {
		result = dns_db_findrdataset(db, node, version,
					     dns_rdatatype_soa, 0, client->now,
					     rdataset, sigrdataset);
	} else {
		dns_fixedname_t foundname;
		dns_name_t *fname;

		dns_fixedname_init(&foundname);
		fname = dns_fixedname_name(&foundname);

		result = dns_db_findext(db, name, version, dns_rdatatype_soa,
					client->query.dboptions, 0, &node,
					fname, &cm, &ci, rdataset, sigrdataset);
	}
	if (result != ISC_R_SUCCESS) {
		/*
		 * This is bad.  We tried to get the SOA RR at the zone top
		 * and it didn't work!
		 */
		CTRACE(ISC_LOG_ERROR, "unable to find SOA RR at zone apex");
		eresult = DNS_R_SERVFAIL;
	} else {
		/*
		 * Extract the SOA MINIMUM.
		 */
		dns_rdata_soa_t soa;
		dns_rdata_t rdata = DNS_RDATA_INIT;
		result = dns_rdataset_first(rdataset);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
		dns_rdataset_current(rdataset, &rdata);
		result = dns_rdata_tostruct(&rdata, &soa, NULL);
		if (result != ISC_R_SUCCESS)
			goto cleanup;

		if (override_ttl != ISC_UINT32_MAX &&
		    override_ttl < rdataset->ttl) {
			rdataset->ttl = override_ttl;
			if (sigrdataset != NULL)
				sigrdataset->ttl = override_ttl;
		}

		/*
		 * Add the SOA and its SIG to the response, with the
		 * TTLs adjusted per RFC2308 section 3.
		 */
		if (rdataset->ttl > soa.minimum)
			rdataset->ttl = soa.minimum;
		if (sigrdataset != NULL && sigrdataset->ttl > soa.minimum)
			sigrdataset->ttl = soa.minimum;

		if (sigrdataset != NULL)
			sigrdatasetp = &sigrdataset;
		else
			sigrdatasetp = NULL;

		if (section == DNS_SECTION_ADDITIONAL)
			rdataset->attributes |= DNS_RDATASETATTR_REQUIRED;
		query_addrrset(client, &name, &rdataset, sigrdatasetp, NULL,
			       section);
	}

 cleanup:
	query_putrdataset(client, &rdataset);
	if (sigrdataset != NULL)
		query_putrdataset(client, &sigrdataset);
	if (name != NULL)
		query_releasename(client, &name);
	if (node != NULL)
		dns_db_detachnode(db, &node);

	return (eresult);
}

static inline isc_result_t
query_addns(ns_client_t *client, dns_db_t *db, dns_dbversion_t *version) {
	dns_name_t *name, *fname;
	dns_dbnode_t *node;
	isc_result_t result, eresult;
	dns_fixedname_t foundname;
	dns_rdataset_t *rdataset = NULL, *sigrdataset = NULL;
	dns_rdataset_t **sigrdatasetp = NULL;
	dns_clientinfomethods_t cm;
	dns_clientinfo_t ci;

	CTRACE(ISC_LOG_DEBUG(3), "query_addns");
	/*
	 * Initialization.
	 */
	eresult = ISC_R_SUCCESS;
	name = NULL;
	rdataset = NULL;
	node = NULL;
	dns_fixedname_init(&foundname);
	fname = dns_fixedname_name(&foundname);
	dns_clientinfomethods_init(&cm, ns_client_sourceip);
	dns_clientinfo_init(&ci, client);

	/*
	 * Get resources and make 'name' be the database origin.
	 */
	result = dns_message_gettempname(client->message, &name);
	if (result != ISC_R_SUCCESS) {
		CTRACE(ISC_LOG_DEBUG(3),
		       "query_addns: dns_message_gettempname failed: done");
		return (result);
	}
	dns_name_init(name, NULL);
	dns_name_clone(dns_db_origin(db), name);
	rdataset = query_newrdataset(client);
	if (rdataset == NULL) {
		CTRACE(ISC_LOG_ERROR,
		       "query_addns: query_newrdataset failed");
		eresult = DNS_R_SERVFAIL;
		goto cleanup;
	}
	if (WANTDNSSEC(client) && dns_db_issecure(db)) {
		sigrdataset = query_newrdataset(client);
		if (sigrdataset == NULL) {
			CTRACE(ISC_LOG_ERROR,
			       "query_addns: query_newrdataset failed");
			eresult = DNS_R_SERVFAIL;
			goto cleanup;
		}
	}

	/*
	 * Find the NS rdataset.
	 */
	result = dns_db_getoriginnode(db, &node);
	if (result == ISC_R_SUCCESS) {
		result = dns_db_findrdataset(db, node, version,
					     dns_rdatatype_ns, 0, client->now,
					     rdataset, sigrdataset);
	} else {
		CTRACE(ISC_LOG_DEBUG(3), "query_addns: calling dns_db_find");
		result = dns_db_findext(db, name, NULL, dns_rdatatype_ns,
					client->query.dboptions, 0, &node,
					fname, &cm, &ci, rdataset, sigrdataset);
		CTRACE(ISC_LOG_DEBUG(3), "query_addns: dns_db_find complete");
	}
	if (result != ISC_R_SUCCESS) {
		CTRACE(ISC_LOG_ERROR,
		       "query_addns: "
		       "dns_db_findrdataset or dns_db_find failed");
		/*
		 * This is bad.  We tried to get the NS rdataset at the zone
		 * top and it didn't work!
		 */
		eresult = DNS_R_SERVFAIL;
	} else {
		if (sigrdataset != NULL)
			sigrdatasetp = &sigrdataset;
		else
			sigrdatasetp = NULL;
		query_addrrset(client, &name, &rdataset, sigrdatasetp, NULL,
			       DNS_SECTION_AUTHORITY);
	}

 cleanup:
	CTRACE(ISC_LOG_DEBUG(3), "query_addns: cleanup");
	query_putrdataset(client, &rdataset);
	if (sigrdataset != NULL)
		query_putrdataset(client, &sigrdataset);
	if (name != NULL)
		query_releasename(client, &name);
	if (node != NULL)
		dns_db_detachnode(db, &node);

	CTRACE(ISC_LOG_DEBUG(3), "query_addns: done");
	return (eresult);
}

static isc_result_t
query_add_cname(ns_client_t *client, dns_name_t *qname, dns_name_t *tname,
		dns_trust_t trust, dns_ttl_t ttl)
{
	dns_rdataset_t *rdataset;
	dns_rdatalist_t *rdatalist;
	dns_rdata_t *rdata;
	isc_region_t r;
	dns_name_t *aname;
	isc_result_t result;

	/*
	 * We assume the name data referred to by tname won't go away.
	 */

	aname = NULL;
	result = dns_message_gettempname(client->message, &aname);
	if (result != ISC_R_SUCCESS)
		return (result);
	result = dns_name_dup(qname, client->mctx, aname);
	if (result != ISC_R_SUCCESS) {
		dns_message_puttempname(client->message, &aname);
		return (result);
	}

	rdatalist = NULL;
	result = dns_message_gettemprdatalist(client->message, &rdatalist);
	if (result != ISC_R_SUCCESS) {
		dns_message_puttempname(client->message, &aname);
		return (result);
	}
	rdata = NULL;
	result = dns_message_gettemprdata(client->message, &rdata);
	if (result != ISC_R_SUCCESS) {
		dns_message_puttempname(client->message, &aname);
		dns_message_puttemprdatalist(client->message, &rdatalist);
		return (result);
	}
	rdataset = NULL;
	result = dns_message_gettemprdataset(client->message, &rdataset);
	if (result != ISC_R_SUCCESS) {
		dns_message_puttempname(client->message, &aname);
		dns_message_puttemprdatalist(client->message, &rdatalist);
		dns_message_puttemprdata(client->message, &rdata);
		return (result);
	}
	rdatalist->type = dns_rdatatype_cname;
	rdatalist->rdclass = client->message->rdclass;
	rdatalist->ttl = ttl;

	dns_name_toregion(tname, &r);
	rdata->data = r.base;
	rdata->length = r.length;
	rdata->rdclass = client->message->rdclass;
	rdata->type = dns_rdatatype_cname;

	ISC_LIST_INIT(rdatalist->rdata);
	ISC_LIST_APPEND(rdatalist->rdata, rdata, link);
	RUNTIME_CHECK(dns_rdatalist_tordataset(rdatalist, rdataset)
		      == ISC_R_SUCCESS);
	rdataset->trust = trust;

	query_addrrset(client, &aname, &rdataset, NULL, NULL,
		       DNS_SECTION_ANSWER);
	if (rdataset != NULL) {
		if (dns_rdataset_isassociated(rdataset))
			dns_rdataset_disassociate(rdataset);
		dns_message_puttemprdataset(client->message, &rdataset);
	}
	if (aname != NULL)
		dns_message_puttempname(client->message, &aname);

	return (ISC_R_SUCCESS);
}

/*
 * Mark the RRsets as secure.  Update the cache (db) to reflect the
 * change in trust level.
 */
static void
mark_secure(ns_client_t *client, dns_db_t *db, dns_name_t *name,
	    dns_rdata_rrsig_t *rrsig, dns_rdataset_t *rdataset,
	    dns_rdataset_t *sigrdataset)
{
	isc_result_t result;
	dns_dbnode_t *node = NULL;
	dns_clientinfomethods_t cm;
	dns_clientinfo_t ci;
	isc_stdtime_t now;

	rdataset->trust = dns_trust_secure;
	sigrdataset->trust = dns_trust_secure;
	dns_clientinfomethods_init(&cm, ns_client_sourceip);
	dns_clientinfo_init(&ci, client);

	/*
	 * Save the updated secure state.  Ignore failures.
	 */
	result = dns_db_findnodeext(db, name, ISC_TRUE, &cm, &ci, &node);
	if (result != ISC_R_SUCCESS)
		return;

	isc_stdtime_get(&now);
	dns_rdataset_trimttl(rdataset, sigrdataset, rrsig, now,
			     client->view->acceptexpired);

	(void)dns_db_addrdataset(db, node, NULL, client->now, rdataset,
				 0, NULL);
	(void)dns_db_addrdataset(db, node, NULL, client->now, sigrdataset,
				 0, NULL);
	dns_db_detachnode(db, &node);
}

/*
 * Find the secure key that corresponds to rrsig.
 * Note: 'keyrdataset' maintains state between successive calls,
 * there may be multiple keys with the same keyid.
 * Return ISC_FALSE if we have exhausted all the possible keys.
 */
static isc_boolean_t
get_key(ns_client_t *client, dns_db_t *db, dns_rdata_rrsig_t *rrsig,
	dns_rdataset_t *keyrdataset, dst_key_t **keyp)
{
	isc_result_t result;
	dns_dbnode_t *node = NULL;
	isc_boolean_t secure = ISC_FALSE;
	dns_clientinfomethods_t cm;
	dns_clientinfo_t ci;

	dns_clientinfomethods_init(&cm, ns_client_sourceip);
	dns_clientinfo_init(&ci, client);

	if (!dns_rdataset_isassociated(keyrdataset)) {
		result = dns_db_findnodeext(db, &rrsig->signer, ISC_FALSE,
					    &cm, &ci, &node);
		if (result != ISC_R_SUCCESS)
			return (ISC_FALSE);

		result = dns_db_findrdataset(db, node, NULL,
					     dns_rdatatype_dnskey, 0,
					     client->now, keyrdataset, NULL);
		dns_db_detachnode(db, &node);
		if (result != ISC_R_SUCCESS)
			return (ISC_FALSE);

		if (keyrdataset->trust != dns_trust_secure)
			return (ISC_FALSE);

		result = dns_rdataset_first(keyrdataset);
	} else
		result = dns_rdataset_next(keyrdataset);

	for ( ; result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(keyrdataset)) {
		dns_rdata_t rdata = DNS_RDATA_INIT;
		isc_buffer_t b;

		dns_rdataset_current(keyrdataset, &rdata);
		isc_buffer_init(&b, rdata.data, rdata.length);
		isc_buffer_add(&b, rdata.length);
		result = dst_key_fromdns(&rrsig->signer, rdata.rdclass, &b,
					 client->mctx, keyp);
		if (result != ISC_R_SUCCESS)
			continue;
		if (rrsig->algorithm == (dns_secalg_t)dst_key_alg(*keyp) &&
		    rrsig->keyid == (dns_keytag_t)dst_key_id(*keyp) &&
		    dst_key_iszonekey(*keyp)) {
			secure = ISC_TRUE;
			break;
		}
		dst_key_free(keyp);
	}
	return (secure);
}

static isc_boolean_t
verify(dst_key_t *key, dns_name_t *name, dns_rdataset_t *rdataset,
       dns_rdata_t *rdata, ns_client_t *client)
{
	isc_result_t result;
	dns_fixedname_t fixed;
	isc_boolean_t ignore = ISC_FALSE;

	dns_fixedname_init(&fixed);

again:
	result = dns_dnssec_verify3(name, rdataset, key, ignore,
				    client->view->maxbits, client->mctx,
				    rdata, NULL);
	if (result == DNS_R_SIGEXPIRED && client->view->acceptexpired) {
		ignore = ISC_TRUE;
		goto again;
	}
	if (result == ISC_R_SUCCESS || result == DNS_R_FROMWILDCARD)
		return (ISC_TRUE);
	return (ISC_FALSE);
}

/*
 * Validate the rdataset if possible with available records.
 */
static isc_boolean_t
validate(ns_client_t *client, dns_db_t *db, dns_name_t *name,
	 dns_rdataset_t *rdataset, dns_rdataset_t *sigrdataset)
{
	isc_result_t result;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdata_rrsig_t rrsig;
	dst_key_t *key = NULL;
	dns_rdataset_t keyrdataset;

	if (sigrdataset == NULL || !dns_rdataset_isassociated(sigrdataset))
		return (ISC_FALSE);

	for (result = dns_rdataset_first(sigrdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(sigrdataset)) {

		dns_rdata_reset(&rdata);
		dns_rdataset_current(sigrdataset, &rdata);
		result = dns_rdata_tostruct(&rdata, &rrsig, NULL);
		if (result != ISC_R_SUCCESS)
			return (ISC_FALSE);
		if (!dns_resolver_algorithm_supported(client->view->resolver,
						      name, rrsig.algorithm))
			continue;
		if (!dns_name_issubdomain(name, &rrsig.signer))
			continue;
		dns_rdataset_init(&keyrdataset);
		do {
			if (!get_key(client, db, &rrsig, &keyrdataset, &key))
				break;
			if (verify(key, name, rdataset, &rdata, client)) {
				dst_key_free(&key);
				dns_rdataset_disassociate(&keyrdataset);
				mark_secure(client, db, name, &rrsig,
					    rdataset, sigrdataset);
				return (ISC_TRUE);
			}
			dst_key_free(&key);
		} while (1);
		if (dns_rdataset_isassociated(&keyrdataset))
			dns_rdataset_disassociate(&keyrdataset);
	}
	return (ISC_FALSE);
}

static void
query_addbestns(ns_client_t *client) {
	dns_db_t *db, *zdb;
	dns_dbnode_t *node;
	dns_name_t *fname, *zfname;
	dns_rdataset_t *rdataset, *sigrdataset, *zrdataset, *zsigrdataset;
	isc_boolean_t is_zone, use_zone;
	isc_buffer_t *dbuf;
	isc_result_t result;
	dns_dbversion_t *version;
	dns_zone_t *zone;
	isc_buffer_t b;
	dns_clientinfomethods_t cm;
	dns_clientinfo_t ci;

	CTRACE(ISC_LOG_DEBUG(3), "query_addbestns");
	fname = NULL;
	zfname = NULL;
	rdataset = NULL;
	zrdataset = NULL;
	sigrdataset = NULL;
	zsigrdataset = NULL;
	node = NULL;
	db = NULL;
	zdb = NULL;
	version = NULL;
	zone = NULL;
	is_zone = ISC_FALSE;
	use_zone = ISC_FALSE;

	dns_clientinfomethods_init(&cm, ns_client_sourceip);
	dns_clientinfo_init(&ci, client);

	/*
	 * Find the right database.
	 */
	result = query_getdb(client, client->query.qname, dns_rdatatype_ns, 0,
			     &zone, &db, &version, &is_zone);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

 db_find:
	/*
	 * We'll need some resources...
	 */
	dbuf = query_getnamebuf(client);
	if (dbuf == NULL)
		goto cleanup;
	fname = query_newname(client, dbuf, &b);
	rdataset = query_newrdataset(client);
	if (fname == NULL || rdataset == NULL)
		goto cleanup;
	/*
	 * Get the RRSIGs if the client requested them or if we may
	 * need to validate answers from the cache.
	 */
	if (WANTDNSSEC(client) || !is_zone) {
		sigrdataset = query_newrdataset(client);
		if (sigrdataset == NULL)
			goto cleanup;
	}

	/*
	 * Now look for the zonecut.
	 */
	if (is_zone) {
		result = dns_db_findext(db, client->query.qname, version,
					dns_rdatatype_ns,
					client->query.dboptions,
					client->now, &node, fname,
					&cm, &ci, rdataset, sigrdataset);
		if (result != DNS_R_DELEGATION)
			goto cleanup;
		if (USECACHE(client)) {
			query_keepname(client, fname, dbuf);
			zdb = db;
			zfname = fname;
			fname = NULL;
			zrdataset = rdataset;
			rdataset = NULL;
			zsigrdataset = sigrdataset;
			sigrdataset = NULL;
			dns_db_detachnode(db, &node);
			version = NULL;
			db = NULL;
			dns_db_attach(client->view->cachedb, &db);
			is_zone = ISC_FALSE;
			goto db_find;
		}
	} else {
		result = dns_db_findzonecut(db, client->query.qname,
					    client->query.dboptions,
					    client->now, &node, fname,
					    rdataset, sigrdataset);
		if (result == ISC_R_SUCCESS) {
			if (zfname != NULL &&
			    !dns_name_issubdomain(fname, zfname)) {
				/*
				 * We found a zonecut in the cache, but our
				 * zone delegation is better.
				 */
				use_zone = ISC_TRUE;
			}
		} else if (result == ISC_R_NOTFOUND && zfname != NULL) {
			/*
			 * We didn't find anything in the cache, but we
			 * have a zone delegation, so use it.
			 */
			use_zone = ISC_TRUE;
		} else
			goto cleanup;
	}

	if (use_zone) {
		query_releasename(client, &fname);
		fname = zfname;
		zfname = NULL;
		/*
		 * We've already done query_keepname() on
		 * zfname, so we must set dbuf to NULL to
		 * prevent query_addrrset() from trying to
		 * call query_keepname() again.
		 */
		dbuf = NULL;
		query_putrdataset(client, &rdataset);
		if (sigrdataset != NULL)
			query_putrdataset(client, &sigrdataset);
		rdataset = zrdataset;
		zrdataset = NULL;
		sigrdataset = zsigrdataset;
		zsigrdataset = NULL;
	}

	/*
	 * Attempt to validate RRsets that are pending or that are glue.
	 */
	if ((DNS_TRUST_PENDING(rdataset->trust) ||
	     (sigrdataset != NULL && DNS_TRUST_PENDING(sigrdataset->trust)))
	    && !validate(client, db, fname, rdataset, sigrdataset) &&
	    !PENDINGOK(client->query.dboptions))
		goto cleanup;

	if ((DNS_TRUST_GLUE(rdataset->trust) ||
	     (sigrdataset != NULL && DNS_TRUST_GLUE(sigrdataset->trust))) &&
	    !validate(client, db, fname, rdataset, sigrdataset) &&
	    SECURE(client) && WANTDNSSEC(client))
		goto cleanup;

	/*
	 * If the answer is secure only add NS records if they are secure
	 * when the client may be looking for AD in the response.
	 */
	if (SECURE(client) && (WANTDNSSEC(client) || WANTAD(client)) &&
	    ((rdataset->trust != dns_trust_secure) ||
	    (sigrdataset != NULL && sigrdataset->trust != dns_trust_secure)))
		goto cleanup;

	/*
	 * If the client doesn't want DNSSEC we can discard the sigrdataset
	 * now.
	 */
	if (!WANTDNSSEC(client))
		query_putrdataset(client, &sigrdataset);
	query_addrrset(client, &fname, &rdataset, &sigrdataset, dbuf,
		       DNS_SECTION_AUTHORITY);

 cleanup:
	if (rdataset != NULL)
		query_putrdataset(client, &rdataset);
	if (sigrdataset != NULL)
		query_putrdataset(client, &sigrdataset);
	if (fname != NULL)
		query_releasename(client, &fname);
	if (node != NULL)
		dns_db_detachnode(db, &node);
	if (db != NULL)
		dns_db_detach(&db);
	if (zone != NULL)
		dns_zone_detach(&zone);
	if (zdb != NULL) {
		query_putrdataset(client, &zrdataset);
		if (zsigrdataset != NULL)
			query_putrdataset(client, &zsigrdataset);
		if (zfname != NULL)
			query_releasename(client, &zfname);
		dns_db_detach(&zdb);
	}
}

static void
fixrdataset(ns_client_t *client, dns_rdataset_t **rdataset) {
	if (*rdataset == NULL)
		*rdataset = query_newrdataset(client);
	else  if (dns_rdataset_isassociated(*rdataset))
		dns_rdataset_disassociate(*rdataset);
}

static void
fixfname(ns_client_t *client, dns_name_t **fname, isc_buffer_t **dbuf,
	 isc_buffer_t *nbuf)
{
	if (*fname == NULL) {
		*dbuf = query_getnamebuf(client);
		if (*dbuf == NULL)
			return;
		*fname = query_newname(client, *dbuf, nbuf);
	}
}

static void
query_addds(ns_client_t *client, dns_db_t *db, dns_dbnode_t *node,
	    dns_dbversion_t *version, dns_name_t *name)
{
	dns_fixedname_t fixed;
	dns_name_t *fname = NULL;
	dns_name_t *rname;
	dns_rdataset_t *rdataset, *sigrdataset;
	isc_buffer_t *dbuf, b;
	isc_result_t result;
	unsigned int count;

	CTRACE(ISC_LOG_DEBUG(3), "query_addds");
	rname = NULL;
	rdataset = NULL;
	sigrdataset = NULL;

	/*
	 * We'll need some resources...
	 */
	rdataset = query_newrdataset(client);
	sigrdataset = query_newrdataset(client);
	if (rdataset == NULL || sigrdataset == NULL)
		goto cleanup;

	/*
	 * Look for the DS record, which may or may not be present.
	 */
	result = dns_db_findrdataset(db, node, version, dns_rdatatype_ds, 0,
				     client->now, rdataset, sigrdataset);
	/*
	 * If we didn't find it, look for an NSEC.
	 */
	if (result == ISC_R_NOTFOUND)
		result = dns_db_findrdataset(db, node, version,
					     dns_rdatatype_nsec, 0, client->now,
					     rdataset, sigrdataset);
	if (result != ISC_R_SUCCESS && result != ISC_R_NOTFOUND)
		goto addnsec3;
	if (!dns_rdataset_isassociated(rdataset) ||
	    !dns_rdataset_isassociated(sigrdataset))
		goto addnsec3;

	/*
	 * We've already added the NS record, so if the name's not there,
	 * we have other problems.  Use this name rather than calling
	 * query_addrrset().
	 */
	result = dns_message_firstname(client->message, DNS_SECTION_AUTHORITY);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	rname = NULL;
	dns_message_currentname(client->message, DNS_SECTION_AUTHORITY,
				&rname);
	result = dns_message_findtype(rname, dns_rdatatype_ns, 0, NULL);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	ISC_LIST_APPEND(rname->list, rdataset, link);
	ISC_LIST_APPEND(rname->list, sigrdataset, link);
	rdataset = NULL;
	sigrdataset = NULL;
	return;

   addnsec3:
	if (!dns_db_iszone(db))
		goto cleanup;
	/*
	 * Add the NSEC3 which proves the DS does not exist.
	 */
	dbuf = query_getnamebuf(client);
	if (dbuf == NULL)
		goto cleanup;
	fname = query_newname(client, dbuf, &b);
	dns_fixedname_init(&fixed);
	if (dns_rdataset_isassociated(rdataset))
		dns_rdataset_disassociate(rdataset);
	if (dns_rdataset_isassociated(sigrdataset))
		dns_rdataset_disassociate(sigrdataset);
	query_findclosestnsec3(name, db, version, client, rdataset,
			       sigrdataset, fname, ISC_TRUE,
			       dns_fixedname_name(&fixed));
	if (!dns_rdataset_isassociated(rdataset))
		goto cleanup;
	query_addrrset(client, &fname, &rdataset, &sigrdataset, dbuf,
		       DNS_SECTION_AUTHORITY);
	/*
	 * Did we find the closest provable encloser instead?
	 * If so add the nearest to the closest provable encloser.
	 */
	if (!dns_name_equal(name, dns_fixedname_name(&fixed))) {
		count = dns_name_countlabels(dns_fixedname_name(&fixed)) + 1;
		dns_name_getlabelsequence(name,
					  dns_name_countlabels(name) - count,
					  count, dns_fixedname_name(&fixed));
		fixfname(client, &fname, &dbuf, &b);
		fixrdataset(client, &rdataset);
		fixrdataset(client, &sigrdataset);
		if (fname == NULL || rdataset == NULL || sigrdataset == NULL)
				goto cleanup;
		query_findclosestnsec3(dns_fixedname_name(&fixed), db, version,
				       client, rdataset, sigrdataset, fname,
				       ISC_FALSE, NULL);
		if (!dns_rdataset_isassociated(rdataset))
			goto cleanup;
		query_addrrset(client, &fname, &rdataset, &sigrdataset, dbuf,
			       DNS_SECTION_AUTHORITY);
	}

 cleanup:
	if (rdataset != NULL)
		query_putrdataset(client, &rdataset);
	if (sigrdataset != NULL)
		query_putrdataset(client, &sigrdataset);
	if (fname != NULL)
		query_releasename(client, &fname);
}

static void
query_addwildcardproof(ns_client_t *client, dns_db_t *db,
		       dns_dbversion_t *version, dns_name_t *name,
		       isc_boolean_t ispositive, isc_boolean_t nodata)
{
	isc_buffer_t *dbuf, b;
	dns_name_t *fname;
	dns_rdataset_t *rdataset, *sigrdataset;
	dns_fixedname_t wfixed;
	dns_name_t *wname;
	dns_dbnode_t *node;
	unsigned int options;
	unsigned int olabels, nlabels, labels;
	isc_result_t result;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdata_nsec_t nsec;
	isc_boolean_t have_wname;
	int order;
	dns_fixedname_t cfixed;
	dns_name_t *cname;
	dns_clientinfomethods_t cm;
	dns_clientinfo_t ci;

	CTRACE(ISC_LOG_DEBUG(3), "query_addwildcardproof");
	fname = NULL;
	rdataset = NULL;
	sigrdataset = NULL;
	node = NULL;

	dns_clientinfomethods_init(&cm, ns_client_sourceip);
	dns_clientinfo_init(&ci, client);

	/*
	 * Get the NOQNAME proof then if !ispositive
	 * get the NOWILDCARD proof.
	 *
	 * DNS_DBFIND_NOWILD finds the NSEC records that covers the
	 * name ignoring any wildcard.  From the owner and next names
	 * of this record you can compute which wildcard (if it exists)
	 * will match by finding the longest common suffix of the
	 * owner name and next names with the qname and prefixing that
	 * with the wildcard label.
	 *
	 * e.g.
	 *   Given:
	 *	example SOA
	 *	example NSEC b.example
	 *	b.example A
	 *	b.example NSEC a.d.example
	 *	a.d.example A
	 *	a.d.example NSEC g.f.example
	 *	g.f.example A
	 *	g.f.example NSEC z.i.example
	 *	z.i.example A
	 *	z.i.example NSEC example
	 *
	 *   QNAME:
	 *   a.example -> example NSEC b.example
	 *	owner common example
	 *	next common example
	 *	wild *.example
	 *   d.b.example -> b.example NSEC a.d.example
	 *	owner common b.example
	 *	next common example
	 *	wild *.b.example
	 *   a.f.example -> a.d.example NSEC g.f.example
	 *	owner common example
	 *	next common f.example
	 *	wild *.f.example
	 *  j.example -> z.i.example NSEC example
	 *	owner common example
	 *	next common example
	 *	wild *.example
	 */
	options = client->query.dboptions | DNS_DBFIND_NOWILD;
	dns_fixedname_init(&wfixed);
	wname = dns_fixedname_name(&wfixed);
 again:
	have_wname = ISC_FALSE;
	/*
	 * We'll need some resources...
	 */
	dbuf = query_getnamebuf(client);
	if (dbuf == NULL)
		goto cleanup;
	fname = query_newname(client, dbuf, &b);
	rdataset = query_newrdataset(client);
	sigrdataset = query_newrdataset(client);
	if (fname == NULL || rdataset == NULL || sigrdataset == NULL)
		goto cleanup;

	result = dns_db_findext(db, name, version, dns_rdatatype_nsec,
				options, 0, &node, fname, &cm, &ci,
				rdataset, sigrdataset);
	if (node != NULL)
		dns_db_detachnode(db, &node);

	if (!dns_rdataset_isassociated(rdataset)) {
		/*
		 * No NSEC proof available, return NSEC3 proofs instead.
		 */
		dns_fixedname_init(&cfixed);
		cname = dns_fixedname_name(&cfixed);
		/*
		 * Find the closest encloser.
		 */
		dns_name_copy(name, cname, NULL);
		while (result == DNS_R_NXDOMAIN) {
			labels = dns_name_countlabels(cname) - 1;
			/*
			 * Sanity check.
			 */
			if (labels == 0U)
				goto cleanup;
			dns_name_split(cname, labels, NULL, cname);
			result = dns_db_findext(db, cname, version,
						dns_rdatatype_nsec,
						options, 0, NULL, fname,
						&cm, &ci, NULL, NULL);
		}
		/*
		 * Add closest (provable) encloser NSEC3.
		 */
		query_findclosestnsec3(cname, db, NULL, client, rdataset,
				       sigrdataset, fname, ISC_TRUE, cname);
		if (!dns_rdataset_isassociated(rdataset))
			goto cleanup;
		if (!ispositive)
			query_addrrset(client, &fname, &rdataset, &sigrdataset,
				       dbuf, DNS_SECTION_AUTHORITY);

		/*
		 * Replace resources which were consumed by query_addrrset.
		 */
		if (fname == NULL) {
			dbuf = query_getnamebuf(client);
			if (dbuf == NULL)
				goto cleanup;
			fname = query_newname(client, dbuf, &b);
		}

		if (rdataset == NULL)
			rdataset = query_newrdataset(client);
		else if (dns_rdataset_isassociated(rdataset))
			dns_rdataset_disassociate(rdataset);

		if (sigrdataset == NULL)
			sigrdataset = query_newrdataset(client);
		else if (dns_rdataset_isassociated(sigrdataset))
			dns_rdataset_disassociate(sigrdataset);

		if (fname == NULL || rdataset == NULL || sigrdataset == NULL)
			goto cleanup;
		/*
		 * Add no qname proof.
		 */
		labels = dns_name_countlabels(cname) + 1;
		if (dns_name_countlabels(name) == labels)
			dns_name_copy(name, wname, NULL);
		else
			dns_name_split(name, labels, NULL, wname);

		query_findclosestnsec3(wname, db, NULL, client, rdataset,
				       sigrdataset, fname, ISC_FALSE, NULL);
		if (!dns_rdataset_isassociated(rdataset))
			goto cleanup;
		query_addrrset(client, &fname, &rdataset, &sigrdataset,
			       dbuf, DNS_SECTION_AUTHORITY);

		if (ispositive)
			goto cleanup;

		/*
		 * Replace resources which were consumed by query_addrrset.
		 */
		if (fname == NULL) {
			dbuf = query_getnamebuf(client);
			if (dbuf == NULL)
				goto cleanup;
			fname = query_newname(client, dbuf, &b);
		}

		if (rdataset == NULL)
			rdataset = query_newrdataset(client);
		else if (dns_rdataset_isassociated(rdataset))
			dns_rdataset_disassociate(rdataset);

		if (sigrdataset == NULL)
			sigrdataset = query_newrdataset(client);
		else if (dns_rdataset_isassociated(sigrdataset))
			dns_rdataset_disassociate(sigrdataset);

		if (fname == NULL || rdataset == NULL || sigrdataset == NULL)
			goto cleanup;
		/*
		 * Add the no wildcard proof.
		 */
		result = dns_name_concatenate(dns_wildcardname,
					      cname, wname, NULL);
		if (result != ISC_R_SUCCESS)
			goto cleanup;

		query_findclosestnsec3(wname, db, NULL, client, rdataset,
				       sigrdataset, fname, nodata, NULL);
		if (!dns_rdataset_isassociated(rdataset))
			goto cleanup;
		query_addrrset(client, &fname, &rdataset, &sigrdataset,
			       dbuf, DNS_SECTION_AUTHORITY);

		goto cleanup;
	} else if (result == DNS_R_NXDOMAIN) {
		if (!ispositive)
			result = dns_rdataset_first(rdataset);
		if (result == ISC_R_SUCCESS) {
			dns_rdataset_current(rdataset, &rdata);
			result = dns_rdata_tostruct(&rdata, &nsec, NULL);
		}
		if (result == ISC_R_SUCCESS) {
			(void)dns_name_fullcompare(name, fname, &order,
						   &olabels);
			(void)dns_name_fullcompare(name, &nsec.next, &order,
						   &nlabels);
			/*
			 * Check for a pathological condition created when
			 * serving some malformed signed zones and bail out.
			 */
			if (dns_name_countlabels(name) == nlabels)
				goto cleanup;

			if (olabels > nlabels)
				dns_name_split(name, olabels, NULL, wname);
			else
				dns_name_split(name, nlabels, NULL, wname);
			result = dns_name_concatenate(dns_wildcardname,
						      wname, wname, NULL);
			if (result == ISC_R_SUCCESS)
				have_wname = ISC_TRUE;
			dns_rdata_freestruct(&nsec);
		}
		query_addrrset(client, &fname, &rdataset, &sigrdataset,
			       dbuf, DNS_SECTION_AUTHORITY);
	}
	if (rdataset != NULL)
		query_putrdataset(client, &rdataset);
	if (sigrdataset != NULL)
		query_putrdataset(client, &sigrdataset);
	if (fname != NULL)
		query_releasename(client, &fname);
	if (have_wname) {
		ispositive = ISC_TRUE;	/* prevent loop */
		if (!dns_name_equal(name, wname)) {
			name = wname;
			goto again;
		}
	}
 cleanup:
	if (rdataset != NULL)
		query_putrdataset(client, &rdataset);
	if (sigrdataset != NULL)
		query_putrdataset(client, &sigrdataset);
	if (fname != NULL)
		query_releasename(client, &fname);
}

static void
query_addnxrrsetnsec(ns_client_t *client, dns_db_t *db,
		     dns_dbversion_t *version, dns_name_t **namep,
		     dns_rdataset_t **rdatasetp, dns_rdataset_t **sigrdatasetp)
{
	dns_name_t *name;
	dns_rdataset_t *sigrdataset;
	dns_rdata_t sigrdata;
	dns_rdata_rrsig_t sig;
	unsigned int labels;
	isc_buffer_t *dbuf, b;
	dns_name_t *fname;
	isc_result_t result;

	name = *namep;
	if ((name->attributes & DNS_NAMEATTR_WILDCARD) == 0) {
		query_addrrset(client, namep, rdatasetp, sigrdatasetp,
			       NULL, DNS_SECTION_AUTHORITY);
		return;
	}

	if (sigrdatasetp == NULL)
		return;

	sigrdataset = *sigrdatasetp;
	if (sigrdataset == NULL || !dns_rdataset_isassociated(sigrdataset))
		return;
	result = dns_rdataset_first(sigrdataset);
	if (result != ISC_R_SUCCESS)
		return;
	dns_rdata_init(&sigrdata);
	dns_rdataset_current(sigrdataset, &sigrdata);
	result = dns_rdata_tostruct(&sigrdata, &sig, NULL);
	if (result != ISC_R_SUCCESS)
		return;

	labels = dns_name_countlabels(name);
	if ((unsigned int)sig.labels + 1 >= labels)
		return;

	/* XXX */
	query_addwildcardproof(client, db, version, client->query.qname,
			       ISC_TRUE, ISC_FALSE);

	/*
	 * We'll need some resources...
	 */
	dbuf = query_getnamebuf(client);
	if (dbuf == NULL)
		return;
	fname = query_newname(client, dbuf, &b);
	if (fname == NULL)
		return;
	dns_name_split(name, sig.labels + 1, NULL, fname);
	/* This will succeed, since we've stripped labels. */
	RUNTIME_CHECK(dns_name_concatenate(dns_wildcardname, fname, fname,
					   NULL) == ISC_R_SUCCESS);
	query_addrrset(client, &fname, rdatasetp, sigrdatasetp,
		       dbuf, DNS_SECTION_AUTHORITY);
}

static void
query_resume(isc_task_t *task, isc_event_t *event) {
	dns_fetchevent_t *devent = (dns_fetchevent_t *)event;
	dns_fetch_t *fetch;
	ns_client_t *client;
	isc_boolean_t fetch_canceled, client_shuttingdown;
	isc_result_t result;
	isc_logcategory_t *logcategory = NS_LOGCATEGORY_QUERY_ERRORS;
	int errorloglevel;

	/*
	 * Resume a query after recursion.
	 */

	UNUSED(task);

	REQUIRE(event->ev_type == DNS_EVENT_FETCHDONE);
	client = devent->ev_arg;
	REQUIRE(NS_CLIENT_VALID(client));
	REQUIRE(task == client->task);
	REQUIRE(RECURSING(client));

	LOCK(&client->query.fetchlock);
	if (client->query.fetch != NULL) {
		/*
		 * This is the fetch we've been waiting for.
		 */
		INSIST(devent->fetch == client->query.fetch);
		client->query.fetch = NULL;
		fetch_canceled = ISC_FALSE;
		/*
		 * Update client->now.
		 */
		isc_stdtime_get(&client->now);
	} else {
		/*
		 * This is a fetch completion event for a canceled fetch.
		 * Clean up and don't resume the find.
		 */
		fetch_canceled = ISC_TRUE;
	}
	UNLOCK(&client->query.fetchlock);
	INSIST(client->query.fetch == NULL);

	client->query.attributes &= ~NS_QUERYATTR_RECURSING;
	fetch = devent->fetch;
	devent->fetch = NULL;

	/*
	 * If this client is shutting down, or this transaction
	 * has timed out, do not resume the find.
	 */
	client_shuttingdown = ns_client_shuttingdown(client);
	if (fetch_canceled || client_shuttingdown) {
		if (devent->node != NULL)
			dns_db_detachnode(devent->db, &devent->node);
		if (devent->db != NULL)
			dns_db_detach(&devent->db);
		query_putrdataset(client, &devent->rdataset);
		if (devent->sigrdataset != NULL)
			query_putrdataset(client, &devent->sigrdataset);
		isc_event_free(&event);
		if (fetch_canceled) {
			CTRACE(ISC_LOG_ERROR, "fetch cancelled");
			query_error(client, DNS_R_SERVFAIL, __LINE__);
		} else
			query_next(client, ISC_R_CANCELED);
		/*
		 * This may destroy the client.
		 */
		ns_client_detach(&client);
	} else {
		result = query_find(client, devent, 0);
		if (result != ISC_R_SUCCESS) {
			if (result == DNS_R_SERVFAIL)
				errorloglevel = ISC_LOG_DEBUG(2);
			else
				errorloglevel = ISC_LOG_DEBUG(4);
			if (isc_log_wouldlog(ns_g_lctx, errorloglevel)) {
				dns_resolver_logfetch(fetch, ns_g_lctx,
						      logcategory,
						      NS_LOGMODULE_QUERY,
						      errorloglevel, ISC_FALSE);
			}
		}
	}

	dns_resolver_destroyfetch(&fetch);
}

static void
prefetch_done(isc_task_t *task, isc_event_t *event) {
	dns_fetchevent_t *devent = (dns_fetchevent_t *)event;
	ns_client_t *client;

	UNUSED(task);

	REQUIRE(event->ev_type == DNS_EVENT_FETCHDONE);
	client = devent->ev_arg;
	REQUIRE(NS_CLIENT_VALID(client));
	REQUIRE(task == client->task);

	LOCK(&client->query.fetchlock);
	if (client->query.prefetch != NULL) {
		INSIST(devent->fetch == client->query.prefetch);
		client->query.prefetch = NULL;
	}
	UNLOCK(&client->query.fetchlock);
	if (devent->fetch != NULL)
		dns_resolver_destroyfetch(&devent->fetch);
	if (devent->node != NULL)
		dns_db_detachnode(devent->db, &devent->node);
	if (devent->db != NULL)
		dns_db_detach(&devent->db);
	query_putrdataset(client, &devent->rdataset);
	isc_event_free(&event);
	ns_client_detach(&client);
}

static void
query_prefetch(ns_client_t *client, dns_name_t *qname,
	       dns_rdataset_t *rdataset)
{
	isc_result_t result;
	isc_sockaddr_t *peeraddr;
	dns_rdataset_t *tmprdataset;
	ns_client_t *dummy = NULL;
	unsigned int options;

	if (client->query.prefetch != NULL ||
	    client->view->prefetch_trigger == 0U ||
	    rdataset->ttl > client->view->prefetch_trigger ||
	    (rdataset->attributes & DNS_RDATASETATTR_PREFETCH) == 0)
		return;

	if (client->recursionquota == NULL) {
		result = isc_quota_attach(&ns_g_server->recursionquota,
					  &client->recursionquota);
		if (result == ISC_R_SUCCESS && !client->mortal && !TCP(client))
			result = ns_client_replace(client);
		if (result != ISC_R_SUCCESS)
			return;
		isc_stats_increment(ns_g_server->nsstats,
				    dns_nsstatscounter_recursclients);
	}

	tmprdataset = query_newrdataset(client);
	if (tmprdataset == NULL)
		return;
	if ((client->attributes & NS_CLIENTATTR_TCP) == 0)
		peeraddr = &client->peeraddr;
	else
		peeraddr = NULL;
	ns_client_attach(client, &dummy);
	options = client->query.fetchoptions | DNS_FETCHOPT_PREFETCH;
	result = dns_resolver_createfetch3(client->view->resolver,
					   qname, rdataset->type, NULL, NULL,
					   NULL, peeraddr, client->message->id,
					   options, 0, NULL, client->task,
					   prefetch_done, client,
					   tmprdataset, NULL,
					   &client->query.prefetch);
	if (result != ISC_R_SUCCESS) {
		query_putrdataset(client, &tmprdataset);
		ns_client_detach(&dummy);
	}
	dns_rdataset_clearprefetch(rdataset);
}

static isc_result_t
query_recurse(ns_client_t *client, dns_rdatatype_t qtype, dns_name_t *qname,
	      dns_name_t *qdomain, dns_rdataset_t *nameservers,
	      isc_boolean_t resuming)
{
	isc_result_t result;
	dns_rdataset_t *rdataset, *sigrdataset;
	isc_sockaddr_t *peeraddr;

	if (!resuming)
		inc_stats(client, dns_nsstatscounter_recursion);

	/*
	 * We are about to recurse, which means that this client will
	 * be unavailable for serving new requests for an indeterminate
	 * amount of time.  If this client is currently responsible
	 * for handling incoming queries, set up a new client
	 * object to handle them while we are waiting for a
	 * response.  There is no need to replace TCP clients
	 * because those have already been replaced when the
	 * connection was accepted (if allowed by the TCP quota).
	 */
	if (client->recursionquota == NULL) {
		result = isc_quota_attach(&ns_g_server->recursionquota,
					  &client->recursionquota);

		isc_stats_increment(ns_g_server->nsstats,
				    dns_nsstatscounter_recursclients);

		if  (result == ISC_R_SOFTQUOTA) {
			static isc_stdtime_t last = 0;
			isc_stdtime_t now;
			isc_stdtime_get(&now);
			if (now != last) {
				last = now;
				ns_client_log(client, NS_LOGCATEGORY_CLIENT,
					      NS_LOGMODULE_QUERY,
					      ISC_LOG_WARNING,
					      "recursive-clients soft limit "
					      "exceeded (%d/%d/%d), "
					      "aborting oldest query",
					      client->recursionquota->used,
					      client->recursionquota->soft,
					      client->recursionquota->max);
			}
			ns_client_killoldestquery(client);
			result = ISC_R_SUCCESS;
		} else if (result == ISC_R_QUOTA) {
			static isc_stdtime_t last = 0;
			isc_stdtime_t now;
			isc_stdtime_get(&now);
			if (now != last) {
				last = now;
				ns_client_log(client, NS_LOGCATEGORY_CLIENT,
					      NS_LOGMODULE_QUERY,
					      ISC_LOG_WARNING,
					      "no more recursive clients "
					      "(%d/%d/%d): %s",
					      ns_g_server->recursionquota.used,
					      ns_g_server->recursionquota.soft,
					      ns_g_server->recursionquota.max,
					      isc_result_totext(result));
			}
			ns_client_killoldestquery(client);
		}
		if (result == ISC_R_SUCCESS && !client->mortal &&
		    (client->attributes & NS_CLIENTATTR_TCP) == 0) {
			result = ns_client_replace(client);
			if (result != ISC_R_SUCCESS) {
				ns_client_log(client, NS_LOGCATEGORY_CLIENT,
					      NS_LOGMODULE_QUERY,
					      ISC_LOG_WARNING,
					      "ns_client_replace() failed: %s",
					      isc_result_totext(result));
				isc_quota_detach(&client->recursionquota);
				isc_stats_decrement(ns_g_server->nsstats,
				    dns_nsstatscounter_recursclients);
			}
		}
		if (result != ISC_R_SUCCESS)
			return (result);
		ns_client_recursing(client);
	}

	/*
	 * Invoke the resolver.
	 */
	REQUIRE(nameservers == NULL || nameservers->type == dns_rdatatype_ns);
	REQUIRE(client->query.fetch == NULL);

	rdataset = query_newrdataset(client);
	if (rdataset == NULL)
		return (ISC_R_NOMEMORY);
	if (WANTDNSSEC(client)) {
		sigrdataset = query_newrdataset(client);
		if (sigrdataset == NULL) {
			query_putrdataset(client, &rdataset);
			return (ISC_R_NOMEMORY);
		}
	} else
		sigrdataset = NULL;

	if (client->query.timerset == ISC_FALSE)
		ns_client_settimeout(client, 60);
	if ((client->attributes & NS_CLIENTATTR_TCP) == 0)
		peeraddr = &client->peeraddr;
	else
		peeraddr = NULL;
	result = dns_resolver_createfetch3(client->view->resolver,
					   qname, qtype, qdomain, nameservers,
					   NULL, peeraddr, client->message->id,
					   client->query.fetchoptions, 0, NULL,
					   client->task, query_resume, client,
					   rdataset, sigrdataset,
					   &client->query.fetch);

	if (result == ISC_R_SUCCESS) {
		/*
		 * Record that we're waiting for an event.  A client which
		 * is shutting down will not be destroyed until all the
		 * events have been received.
		 */
	} else {
		query_putrdataset(client, &rdataset);
		if (sigrdataset != NULL)
			query_putrdataset(client, &sigrdataset);
	}

	return (result);
}

static inline void
rpz_clean(dns_zone_t **zonep, dns_db_t **dbp, dns_dbnode_t **nodep,
	  dns_rdataset_t **rdatasetp)
{
	if (nodep != NULL && *nodep != NULL) {
		REQUIRE(dbp != NULL && *dbp != NULL);
		dns_db_detachnode(*dbp, nodep);
	}
	if (dbp != NULL && *dbp != NULL)
		dns_db_detach(dbp);
	if (zonep != NULL && *zonep != NULL)
		dns_zone_detach(zonep);
	if (rdatasetp != NULL && *rdatasetp != NULL &&
	    dns_rdataset_isassociated(*rdatasetp))
		dns_rdataset_disassociate(*rdatasetp);
}

static inline void
rpz_match_clear(dns_rpz_st_t *st) {
	rpz_clean(&st->m.zone, &st->m.db, &st->m.node, &st->m.rdataset);
	st->m.version = NULL;
}

static inline isc_result_t
rpz_ready(ns_client_t *client, dns_rdataset_t **rdatasetp) {
	REQUIRE(rdatasetp != NULL);

	CTRACE(ISC_LOG_DEBUG(3), "rpz_ready");

	if (*rdatasetp == NULL) {
		*rdatasetp = query_newrdataset(client);
		if (*rdatasetp == NULL) {
			CTRACE(ISC_LOG_ERROR,
			       "rpz_ready: query_newrdataset failed");
			return (DNS_R_SERVFAIL);
		}
	} else if (dns_rdataset_isassociated(*rdatasetp)) {
		dns_rdataset_disassociate(*rdatasetp);
	}
	return (ISC_R_SUCCESS);
}

static void
rpz_st_clear(ns_client_t *client) {
	dns_rpz_st_t *st = client->query.rpz_st;

	CTRACE(ISC_LOG_DEBUG(3), "rpz_st_clear");

	if (st->m.rdataset != NULL)
		query_putrdataset(client, &st->m.rdataset);
	rpz_match_clear(st);

	rpz_clean(NULL, &st->r.db, NULL, NULL);
	if (st->r.ns_rdataset != NULL)
		query_putrdataset(client, &st->r.ns_rdataset);
	if (st->r.r_rdataset != NULL)
		query_putrdataset(client, &st->r.r_rdataset);

	rpz_clean(&st->q.zone, &st->q.db, &st->q.node, NULL);
	if (st->q.rdataset != NULL)
		query_putrdataset(client, &st->q.rdataset);
	if (st->q.sigrdataset != NULL)
		query_putrdataset(client, &st->q.sigrdataset);
	st->state = 0;
	st->m.type = DNS_RPZ_TYPE_BAD;
	st->m.policy = DNS_RPZ_POLICY_MISS;
}

static dns_rpz_zbits_t
rpz_get_zbits(ns_client_t *client,
	      dns_rdatatype_t ip_type, dns_rpz_type_t rpz_type)
{
	dns_rpz_st_t *st;
	dns_rpz_zbits_t zbits;

	REQUIRE(client != NULL);
	REQUIRE(client->query.rpz_st != NULL);

	st = client->query.rpz_st;

	switch (rpz_type) {
	case DNS_RPZ_TYPE_CLIENT_IP:
		zbits = st->have.client_ip;
		break;
	case DNS_RPZ_TYPE_QNAME:
		zbits = st->have.qname;
		break;
	case DNS_RPZ_TYPE_IP:
		if (ip_type == dns_rdatatype_a) {
			zbits = st->have.ipv4;
		} else if (ip_type == dns_rdatatype_aaaa) {
			zbits = st->have.ipv6;
		} else {
			zbits = st->have.ip;
		}
		break;
	case DNS_RPZ_TYPE_NSDNAME:
		zbits = st->have.nsdname;
		break;
	case DNS_RPZ_TYPE_NSIP:
		if (ip_type == dns_rdatatype_a) {
			zbits = st->have.nsipv4;
		} else if (ip_type == dns_rdatatype_aaaa) {
			zbits = st->have.nsipv6;
		} else {
			zbits = st->have.nsip;
		}
		break;
	default:
		INSIST(0);
		break;
	}

	/*
	 * Choose
	 *	the earliest configured policy zone (rpz->num)
	 *	QNAME over IP over NSDNAME over NSIP (rpz_type)
	 *	the smallest name,
	 *	the longest IP address prefix,
	 *	the lexically smallest address.
	 */
	if (st->m.policy != DNS_RPZ_POLICY_MISS) {
		if (st->m.type >= rpz_type) {
			zbits &= DNS_RPZ_ZMASK(st->m.rpz->num);
		} else{
			zbits &= DNS_RPZ_ZMASK(st->m.rpz->num) >> 1;
		}
	}

	/*
	 * If the client wants recursion, allow only compatible policies.
	 */
	if (!RECURSIONOK(client))
		zbits &= st->popt.no_rd_ok;

	return (zbits);
}

/*
 * Get an NS, A, or AAAA rrset related to the response for the client
 * to check the contents of that rrset for hits by eligible policy zones.
 */
static isc_result_t
rpz_rrset_find(ns_client_t *client, dns_name_t *name, dns_rdatatype_t type,
	       dns_rpz_type_t rpz_type, dns_db_t **dbp, dns_dbversion_t *version,
	       dns_rdataset_t **rdatasetp, isc_boolean_t resuming)
{
	dns_rpz_st_t *st;
	isc_boolean_t is_zone;
	dns_dbnode_t *node;
	dns_fixedname_t fixed;
	dns_name_t *found;
	isc_result_t result;
	dns_clientinfomethods_t cm;
	dns_clientinfo_t ci;

	CTRACE(ISC_LOG_DEBUG(3), "rpz_rrset_find");

	st = client->query.rpz_st;
	if ((st->state & DNS_RPZ_RECURSING) != 0) {
		INSIST(st->r.r_type == type);
		INSIST(dns_name_equal(name, st->r_name));
		INSIST(*rdatasetp == NULL ||
		       !dns_rdataset_isassociated(*rdatasetp));
		INSIST(*dbp == NULL);
		st->state &= ~DNS_RPZ_RECURSING;
		*dbp = st->r.db;
		st->r.db = NULL;
		if (*rdatasetp != NULL)
			query_putrdataset(client, rdatasetp);
		*rdatasetp = st->r.r_rdataset;
		st->r.r_rdataset = NULL;
		result = st->r.r_result;
		if (result == DNS_R_DELEGATION) {
			CTRACE(ISC_LOG_ERROR, "RPZ recursing");
			rpz_log_fail(client, DNS_RPZ_ERROR_LEVEL, name,
				     rpz_type, " rpz_rrset_find(1)", result);
			st->m.policy = DNS_RPZ_POLICY_ERROR;
			result = DNS_R_SERVFAIL;
		}
		return (result);
	}

	result = rpz_ready(client, rdatasetp);
	if (result != ISC_R_SUCCESS) {
		st->m.policy = DNS_RPZ_POLICY_ERROR;
		return (result);
	}
	if (*dbp != NULL) {
		is_zone = ISC_FALSE;
	} else {
		dns_zone_t *zone;

		version = NULL;
		zone = NULL;
		result = query_getdb(client, name, type, 0, &zone, dbp,
				     &version, &is_zone);
		if (result != ISC_R_SUCCESS) {
			rpz_log_fail(client, DNS_RPZ_ERROR_LEVEL, name,
				     rpz_type, " rpz_rrset_find(2)", result);
			st->m.policy = DNS_RPZ_POLICY_ERROR;
			if (zone != NULL)
				dns_zone_detach(&zone);
			return (result);
		}
		if (zone != NULL)
			dns_zone_detach(&zone);
	}

	node = NULL;
	dns_fixedname_init(&fixed);
	found = dns_fixedname_name(&fixed);
	dns_clientinfomethods_init(&cm, ns_client_sourceip);
	dns_clientinfo_init(&ci, client);
	result = dns_db_findext(*dbp, name, version, type, DNS_DBFIND_GLUEOK,
				client->now, &node, found,
				&cm, &ci, *rdatasetp, NULL);
	if (result == DNS_R_DELEGATION && is_zone && USECACHE(client)) {
		/*
		 * Try the cache if we're authoritative for an
		 * ancestor but not the domain itself.
		 */
		rpz_clean(NULL, dbp, &node, rdatasetp);
		version = NULL;
		dns_db_attach(client->view->cachedb, dbp);
		result = dns_db_findext(*dbp, name, version, dns_rdatatype_ns,
					0, client->now, &node, found,
					&cm, &ci, *rdatasetp, NULL);
	}
	rpz_clean(NULL, dbp, &node, NULL);
	if (result == DNS_R_DELEGATION) {
		rpz_clean(NULL, NULL, NULL, rdatasetp);
		/*
		 * Recurse for NS rrset or A or AAAA rrset for an NS.
		 * Do not recurse for addresses for the query name.
		 */
		if (rpz_type == DNS_RPZ_TYPE_IP) {
			result = DNS_R_NXRRSET;
		} else {
			dns_name_copy(name, st->r_name, NULL);
			result = query_recurse(client, type, st->r_name,
					       NULL, NULL, resuming);
			if (result == ISC_R_SUCCESS) {
				st->state |= DNS_RPZ_RECURSING;
				result = DNS_R_DELEGATION;
			}
		}
	}
	return (result);
}

/*
 * Compute a policy owner name, p_name, in a policy zone given the needed
 * policy type and the trigger name.
 */
static isc_result_t
rpz_get_p_name(ns_client_t *client, dns_name_t *p_name,
	       dns_rpz_zone_t *rpz, dns_rpz_type_t rpz_type,
	       dns_name_t *trig_name)
{
	dns_offsets_t prefix_offsets;
	dns_name_t prefix, *suffix;
	unsigned int first, labels;
	isc_result_t result;

	CTRACE(ISC_LOG_DEBUG(3), "rpz_get_p_name");

	/*
	 * The policy owner name consists of a suffix depending on the type
	 * and policy zone and a prefix that is the longest possible string
	 * from the trigger name that keesp the resulting policy owner name
	 * from being too long.
	 */
	switch (rpz_type) {
	case DNS_RPZ_TYPE_CLIENT_IP:
		suffix = &rpz->client_ip;
		break;
	case DNS_RPZ_TYPE_QNAME:
		suffix = &rpz->origin;
		break;
	case DNS_RPZ_TYPE_IP:
		suffix = &rpz->ip;
		break;
	case DNS_RPZ_TYPE_NSDNAME:
		suffix = &rpz->nsdname;
		break;
	case DNS_RPZ_TYPE_NSIP:
		suffix = &rpz->nsip;
		break;
	default:
		INSIST(0);
	}

	/*
	 * Start with relative version of the full trigger name,
	 * and trim enough allow the addition of the suffix.
	 */
	dns_name_init(&prefix, prefix_offsets);
	labels = dns_name_countlabels(trig_name);
	first = 0;
	for (;;) {
		dns_name_getlabelsequence(trig_name, first, labels-first-1,
					  &prefix);
		result = dns_name_concatenate(&prefix, suffix, p_name, NULL);
		if (result == ISC_R_SUCCESS)
			break;
		INSIST(result == DNS_R_NAMETOOLONG);
		/*
		 * Trim the trigger name until the combination is not too long.
		 */
		if (labels-first < 2) {
			rpz_log_fail(client, DNS_RPZ_ERROR_LEVEL, suffix,
				     rpz_type, " concatentate()", result);
			return (ISC_R_FAILURE);
		}
		/*
		 * Complain once about trimming the trigger name.
		 */
		if (first == 0) {
			rpz_log_fail(client, DNS_RPZ_DEBUG_LEVEL1, suffix,
				     rpz_type, " concatentate()", result);
		}
		++first;
	}
	return (ISC_R_SUCCESS);
}

/*
 * Look in policy zone rpz for a policy of rpz_type by p_name.
 * The self-name (usually the client qname or an NS name) is compared with
 * the target of a CNAME policy for the old style passthru encoding.
 * If found, the policy is recorded in *zonep, *dbp, *versionp, *nodep,
 * *rdatasetp, and *policyp.
 * The target DNS type, qtype, chooses the best rdataset for *rdatasetp.
 * The caller must decide if the found policy is most suitable, including
 * better than a previously found policy.
 * If it is best, the caller records it in client->query.rpz_st->m.
 */
static isc_result_t
rpz_find_p(ns_client_t *client, dns_name_t *self_name, dns_rdatatype_t qtype,
	   dns_name_t *p_name, dns_rpz_zone_t *rpz, dns_rpz_type_t rpz_type,
	   dns_zone_t **zonep, dns_db_t **dbp, dns_dbversion_t **versionp,
	   dns_dbnode_t **nodep, dns_rdataset_t **rdatasetp,
	   dns_rpz_policy_t *policyp)
{
	dns_fixedname_t foundf;
	dns_name_t *found;
	isc_result_t result;
	dns_clientinfomethods_t cm;
	dns_clientinfo_t ci;

	REQUIRE(nodep != NULL);

	CTRACE(ISC_LOG_DEBUG(3), "rpz_find_p");

	/*
	 * Try to find either a CNAME or the type of record demanded by the
	 * request from the policy zone.
	 */
	rpz_clean(zonep, dbp, nodep, rdatasetp);
	result = rpz_ready(client, rdatasetp);
	if (result != ISC_R_SUCCESS) {
		CTRACE(ISC_LOG_ERROR, "rpz_ready() failed");
		return (DNS_R_SERVFAIL);
	}
	*versionp = NULL;
	result = rpz_getdb(client, p_name, rpz_type, zonep, dbp, versionp);
	if (result != ISC_R_SUCCESS)
		return (DNS_R_NXDOMAIN);
	dns_fixedname_init(&foundf);
	found = dns_fixedname_name(&foundf);
	dns_clientinfomethods_init(&cm, ns_client_sourceip);
	dns_clientinfo_init(&ci, client);
	result = dns_db_findext(*dbp, p_name, *versionp, dns_rdatatype_any, 0,
				client->now, nodep, found, &cm, &ci,
				*rdatasetp, NULL);
	/*
	 * Choose the best rdataset if we found something.
	 */
	if (result == ISC_R_SUCCESS) {
		dns_rdatasetiter_t *rdsiter;

		rdsiter = NULL;
		result = dns_db_allrdatasets(*dbp, *nodep, *versionp, 0,
					     &rdsiter);
		if (result != ISC_R_SUCCESS) {
			rpz_log_fail(client, DNS_RPZ_ERROR_LEVEL, p_name,
				     rpz_type, " allrdatasets()", result);
			CTRACE(ISC_LOG_ERROR,
			       "rpz_find_p: allrdatasets failed");
			return (DNS_R_SERVFAIL);
		}
		for (result = dns_rdatasetiter_first(rdsiter);
		     result == ISC_R_SUCCESS;
		     result = dns_rdatasetiter_next(rdsiter)) {
			dns_rdatasetiter_current(rdsiter, *rdatasetp);
			if ((*rdatasetp)->type == dns_rdatatype_cname ||
			    (*rdatasetp)->type == qtype)
				break;
			dns_rdataset_disassociate(*rdatasetp);
		}
		dns_rdatasetiter_destroy(&rdsiter);
		if (result != ISC_R_SUCCESS) {
			if (result != ISC_R_NOMORE) {
				rpz_log_fail(client, DNS_RPZ_ERROR_LEVEL,
					     p_name, rpz_type,
					     " rdatasetiter", result);
				CTRACE(ISC_LOG_ERROR,
				       "rpz_find_p: rdatasetiter_destroy "
				       "failed");
				return (DNS_R_SERVFAIL);
			}
			/*
			 * Ask again to get the right DNS_R_DNAME/NXRRSET/...
			 * result if there is neither a CNAME nor target type.
			 */
			if (dns_rdataset_isassociated(*rdatasetp))
				dns_rdataset_disassociate(*rdatasetp);
			dns_db_detachnode(*dbp, nodep);

			if (qtype == dns_rdatatype_rrsig ||
			    qtype == dns_rdatatype_sig)
				result = DNS_R_NXRRSET;
			else
				result = dns_db_findext(*dbp, p_name, *versionp,
							qtype, 0, client->now,
							nodep, found, &cm, &ci,
							*rdatasetp, NULL);
		}
	}
	switch (result) {
	case ISC_R_SUCCESS:
		if ((*rdatasetp)->type != dns_rdatatype_cname) {
			*policyp = DNS_RPZ_POLICY_RECORD;
		} else {
			*policyp = dns_rpz_decode_cname(rpz, *rdatasetp,
							self_name);
			if ((*policyp == DNS_RPZ_POLICY_RECORD ||
			     *policyp == DNS_RPZ_POLICY_WILDCNAME) &&
			    qtype != dns_rdatatype_cname &&
			    qtype != dns_rdatatype_any)
				return (DNS_R_CNAME);
		}
		return (ISC_R_SUCCESS);
	case DNS_R_NXRRSET:
		*policyp = DNS_RPZ_POLICY_NODATA;
		return (result);
	case DNS_R_DNAME:
		/*
		 * DNAME policy RRs have very few if any uses that are not
		 * better served with simple wildcards.  Making them work would
		 * require complications to get the number of labels matched
		 * in the name or the found name to the main DNS_R_DNAME case
		 * in query_find().  The domain also does not appear in the
		 * summary database at the right level, so this happens only
		 * with a single policy zone when we have no summary database.
		 * Treat it as a miss.
		 */
	case DNS_R_NXDOMAIN:
	case DNS_R_EMPTYNAME:
		return (DNS_R_NXDOMAIN);
	default:
		rpz_log_fail(client, DNS_RPZ_ERROR_LEVEL, p_name, rpz_type,
			     "", result);
		CTRACE(ISC_LOG_ERROR,
		       "rpz_find_p: unexpected result");
		return (DNS_R_SERVFAIL);
	}
}

static void
rpz_save_p(dns_rpz_st_t *st, dns_rpz_zone_t *rpz, dns_rpz_type_t rpz_type,
	   dns_rpz_policy_t policy, dns_name_t *p_name, dns_rpz_prefix_t prefix,
	   isc_result_t result, dns_zone_t **zonep, dns_db_t **dbp,
	   dns_dbnode_t **nodep, dns_rdataset_t **rdatasetp,
	   dns_dbversion_t *version)
{
	dns_rdataset_t *trdataset;

	rpz_match_clear(st);
	st->m.rpz = rpz;
	st->m.type = rpz_type;
	st->m.policy = policy;
	dns_name_copy(p_name, st->p_name, NULL);
	st->m.prefix = prefix;
	st->m.result = result;
	st->m.zone = *zonep;
	*zonep = NULL;
	st->m.db = *dbp;
	*dbp = NULL;
	st->m.node = *nodep;
	*nodep = NULL;
	if (*rdatasetp != NULL && dns_rdataset_isassociated(*rdatasetp)) {
		/*
		 * Save the replacement rdataset from the policy
		 * and make the previous replacement rdataset scratch.
		 */
		trdataset = st->m.rdataset;
		st->m.rdataset = *rdatasetp;
		*rdatasetp = trdataset;
		st->m.ttl = ISC_MIN(st->m.rdataset->ttl, rpz->max_policy_ttl);
	} else {
		st->m.ttl = ISC_MIN(DNS_RPZ_TTL_DEFAULT, rpz->max_policy_ttl);
	}
	st->m.version = version;
}

/*
 * Check this address in every eligible policy zone.
 */
static isc_result_t
rpz_rewrite_ip(ns_client_t *client, const isc_netaddr_t *netaddr,
	       dns_rdatatype_t qtype, dns_rpz_type_t rpz_type,
	       dns_rpz_zbits_t zbits, dns_rdataset_t **p_rdatasetp)
{
	dns_rpz_zones_t *rpzs;
	dns_rpz_st_t *st;
	dns_rpz_zone_t *rpz;
	dns_rpz_prefix_t prefix;
	dns_rpz_num_t rpz_num;
	dns_fixedname_t ip_namef, p_namef;
	dns_name_t *ip_name, *p_name;
	dns_zone_t *p_zone;
	dns_db_t *p_db;
	dns_dbversion_t *p_version;
	dns_dbnode_t *p_node;
	dns_rpz_policy_t policy;
	isc_result_t result;

	CTRACE(ISC_LOG_DEBUG(3), "rpz_rewrite_ip");

	dns_fixedname_init(&ip_namef);
	ip_name = dns_fixedname_name(&ip_namef);

	p_zone = NULL;
	p_db = NULL;
	p_node = NULL;

	rpzs = client->view->rpzs;
	st = client->query.rpz_st;
	while (zbits != 0) {
		rpz_num = dns_rpz_find_ip(rpzs, rpz_type, zbits, netaddr,
					  ip_name, &prefix);
		if (rpz_num == DNS_RPZ_INVALID_NUM)
			break;
		zbits &= (DNS_RPZ_ZMASK(rpz_num) >> 1);

		/*
		 * Do not try applying policy zones that cannot replace a
		 * previously found policy zone.
		 * Stop looking if the next best choice cannot
		 * replace what we already have.
		 */
		rpz = rpzs->zones[rpz_num];
		if (st->m.policy != DNS_RPZ_POLICY_MISS) {
			if (st->m.rpz->num < rpz->num)
				break;
			if (st->m.rpz->num == rpz->num &&
			    (st->m.type < rpz_type ||
			     st->m.prefix > prefix))
				break;
		}

		/*
		 * Get the policy for a prefix at least as long
		 * as the prefix of the entry we had before.
		 */
		dns_fixedname_init(&p_namef);
		p_name = dns_fixedname_name(&p_namef);
		result = rpz_get_p_name(client, p_name, rpz, rpz_type, ip_name);
		if (result != ISC_R_SUCCESS)
			continue;
		result = rpz_find_p(client, ip_name, qtype,
				    p_name, rpz, rpz_type,
				    &p_zone, &p_db, &p_version, &p_node,
				    p_rdatasetp, &policy);
		switch (result) {
		case DNS_R_NXDOMAIN:
			/*
			 * Continue after a policy record that is missing
			 * contrary to the summary data.  The summary
			 * data can out of date during races with and among
			 * policy zone updates.
			 */
			CTRACE(ISC_LOG_ERROR,
			       "rpz_rewrite_ip: mismatched summary data; "
			       "continuing");
			continue;
		case DNS_R_SERVFAIL:
			rpz_clean(&p_zone, &p_db, &p_node, p_rdatasetp);
			st->m.policy = DNS_RPZ_POLICY_ERROR;
			return (DNS_R_SERVFAIL);
		default:
			/*
			 * Forget this policy if it is not preferable
			 * to the previously found policy.
			 * If this policy is not good, then stop looking
			 * because none of the later policy zones would work.
			 *
			 * With more than one applicable policy, prefer
			 * the earliest configured policy,
			 * client-IP over QNAME over IP over NSDNAME over NSIP,
			 * the longest prefix
			 * the lexically smallest address.
			 * dns_rpz_find_ip() ensures st->m.rpz->num >= rpz->num.
			 * We can compare new and current p_name because
			 * both are of the same type and in the same zone.
			 * The tests above eliminate other reasons to
			 * reject this policy.  If this policy can't work,
			 * then neither can later zones.
			 */
			if (st->m.policy != DNS_RPZ_POLICY_MISS &&
			    rpz->num == st->m.rpz->num &&
			     (st->m.type == rpz_type &&
			      st->m.prefix == prefix &&
			      0 > dns_name_rdatacompare(st->p_name, p_name)))
				break;

			/*
			 * Stop checking after saving an enabled hit in this
			 * policy zone.  The radix tree in the policy zone
			 * ensures that we found the longest match.
			 */
			if (rpz->policy != DNS_RPZ_POLICY_DISABLED) {
				CTRACE(ISC_LOG_DEBUG(3),
				       "rpz_rewrite_ip: rpz_save_p");
				rpz_save_p(st, rpz, rpz_type,
					   policy, p_name, prefix, result,
					   &p_zone, &p_db, &p_node,
					   p_rdatasetp, p_version);
				break;
			}

			/*
			 * Log DNS_RPZ_POLICY_DISABLED zones
			 * and try the next eligible policy zone.
			 */
			rpz_log_rewrite(client, ISC_TRUE, policy, rpz_type,
					p_zone, p_name);
		}
	}

	rpz_clean(&p_zone, &p_db, &p_node, p_rdatasetp);
	return (ISC_R_SUCCESS);
}

/*
 * Check the IP addresses in the A or AAAA rrsets for name against
 * all eligible rpz_type (IP or NSIP) response policy rewrite rules.
 */
static isc_result_t
rpz_rewrite_ip_rrset(ns_client_t *client,
		     dns_name_t *name, dns_rdatatype_t qtype,
		     dns_rpz_type_t rpz_type, dns_rdatatype_t ip_type,
		     dns_db_t **ip_dbp, dns_dbversion_t *ip_version,
		     dns_rdataset_t **ip_rdatasetp,
		     dns_rdataset_t **p_rdatasetp, isc_boolean_t resuming)
{
	dns_rpz_zbits_t zbits;
	isc_netaddr_t netaddr;
	struct in_addr ina;
	struct in6_addr in6a;
	isc_result_t result;

	CTRACE(ISC_LOG_DEBUG(3), "rpz_rewrite_ip_rrset");

	zbits = rpz_get_zbits(client, ip_type, rpz_type);
	if (zbits == 0)
		return (ISC_R_SUCCESS);

	/*
	 * Get the A or AAAA rdataset.
	 */
	result = rpz_rrset_find(client, name, ip_type, rpz_type, ip_dbp,
				ip_version, ip_rdatasetp, resuming);
	switch (result) {
	case ISC_R_SUCCESS:
	case DNS_R_GLUE:
	case DNS_R_ZONECUT:
		break;
	case DNS_R_EMPTYNAME:
	case DNS_R_EMPTYWILD:
	case DNS_R_NXDOMAIN:
	case DNS_R_NCACHENXDOMAIN:
	case DNS_R_NXRRSET:
	case DNS_R_NCACHENXRRSET:
	case ISC_R_NOTFOUND:
		return (ISC_R_SUCCESS);
	case DNS_R_DELEGATION:
	case DNS_R_DUPLICATE:
	case DNS_R_DROP:
		return (result);
	case DNS_R_CNAME:
	case DNS_R_DNAME:
		rpz_log_fail(client, DNS_RPZ_DEBUG_LEVEL1, name, rpz_type,
			     " NS address rewrite rrset", result);
		return (ISC_R_SUCCESS);
	default:
		if (client->query.rpz_st->m.policy != DNS_RPZ_POLICY_ERROR) {
			client->query.rpz_st->m.policy = DNS_RPZ_POLICY_ERROR;
			rpz_log_fail(client, DNS_RPZ_ERROR_LEVEL, name,
				     rpz_type, " NS address rewrite rrset",
				     result);
		}
		CTRACE(ISC_LOG_ERROR,
		       "rpz_rewrite_ip_rrset: unexpected result");
		return (DNS_R_SERVFAIL);
	}

	/*
	 * Check all of the IP addresses in the rdataset.
	 */
	for (result = dns_rdataset_first(*ip_rdatasetp);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(*ip_rdatasetp)) {

		dns_rdata_t rdata = DNS_RDATA_INIT;
		dns_rdataset_current(*ip_rdatasetp, &rdata);
		switch (rdata.type) {
		case dns_rdatatype_a:
			INSIST(rdata.length == 4);
			memmove(&ina.s_addr, rdata.data, 4);
			isc_netaddr_fromin(&netaddr, &ina);
			break;
		case dns_rdatatype_aaaa:
			INSIST(rdata.length == 16);
			memmove(in6a.s6_addr, rdata.data, 16);
			isc_netaddr_fromin6(&netaddr, &in6a);
			break;
		default:
			continue;
		}

		result = rpz_rewrite_ip(client, &netaddr, qtype, rpz_type,
					zbits, p_rdatasetp);
		if (result != ISC_R_SUCCESS)
			return (result);
	}

	return (ISC_R_SUCCESS);
}

/*
 * Look for IP addresses in A and AAAA rdatasets
 * that trigger all eligible IP or NSIP policy rules.
 */
static isc_result_t
rpz_rewrite_ip_rrsets(ns_client_t *client, dns_name_t *name,
		      dns_rdatatype_t qtype, dns_rpz_type_t rpz_type,
		      dns_rdataset_t **ip_rdatasetp, isc_boolean_t resuming)
{
	dns_rpz_st_t *st;
	dns_dbversion_t *ip_version;
	dns_db_t *ip_db;
	dns_rdataset_t *p_rdataset;
	isc_result_t result;

	CTRACE(ISC_LOG_DEBUG(3), "rpz_rewrite_ip_rrsets");

	st = client->query.rpz_st;
	ip_version = NULL;
	ip_db = NULL;
	p_rdataset = NULL;
	if ((st->state & DNS_RPZ_DONE_IPv4) == 0 &&
	    (qtype == dns_rdatatype_a ||
	     qtype == dns_rdatatype_any ||
	     rpz_type == DNS_RPZ_TYPE_NSIP)) {
		/*
		 * Rewrite based on an IPv4 address that will appear
		 * in the ANSWER section or if we are checking IP addresses.
		 */
		result = rpz_rewrite_ip_rrset(client, name, qtype,
					      rpz_type, dns_rdatatype_a,
					      &ip_db, ip_version, ip_rdatasetp,
					      &p_rdataset, resuming);
		if (result == ISC_R_SUCCESS)
			st->state |= DNS_RPZ_DONE_IPv4;
	} else {
		result = ISC_R_SUCCESS;
	}
	if (result == ISC_R_SUCCESS &&
	    (qtype == dns_rdatatype_aaaa ||
	     qtype == dns_rdatatype_any ||
	     rpz_type == DNS_RPZ_TYPE_NSIP)) {
		/*
		 * Rewrite based on IPv6 addresses that will appear
		 * in the ANSWER section or if we are checking IP addresses.
		 */
		result = rpz_rewrite_ip_rrset(client, name,  qtype,
					      rpz_type, dns_rdatatype_aaaa,
					      &ip_db, ip_version, ip_rdatasetp,
					      &p_rdataset, resuming);
	}
	if (ip_db != NULL)
		dns_db_detach(&ip_db);
	query_putrdataset(client, &p_rdataset);
	return (result);
}

/*
 * Try to rewrite a request for a qtype rdataset based on the trigger name
 * trig_name and rpz_type (DNS_RPZ_TYPE_QNAME or DNS_RPZ_TYPE_NSDNAME).
 * Record the results including the replacement rdataset if any
 * in client->query.rpz_st.
 * *rdatasetp is a scratch rdataset.
 */
static isc_result_t
rpz_rewrite_name(ns_client_t *client, dns_name_t *trig_name,
		 dns_rdatatype_t qtype, dns_rpz_type_t rpz_type,
		 dns_rpz_zbits_t allowed_zbits, dns_rdataset_t **rdatasetp)
{
	dns_rpz_zones_t *rpzs;
	dns_rpz_zone_t *rpz;
	dns_rpz_st_t *st;
	dns_fixedname_t p_namef;
	dns_name_t *p_name;
	dns_rpz_zbits_t zbits;
	dns_rpz_num_t rpz_num;
	dns_zone_t *p_zone;
	dns_db_t *p_db;
	dns_dbversion_t *p_version;
	dns_dbnode_t *p_node;
	dns_rpz_policy_t policy;
	isc_result_t result;

	CTRACE(ISC_LOG_DEBUG(3), "rpz_rewrite_name");

	zbits = rpz_get_zbits(client, qtype, rpz_type);
	zbits &= allowed_zbits;
	if (zbits == 0)
		return (ISC_R_SUCCESS);

	rpzs = client->view->rpzs;

	/*
	 * Use the summary database to find the bit mask of policy zones
	 * with policies for this trigger name. We do this even if there
	 * is only one eligible policy zone so that wildcard triggers
	 * are matched correctly, and not into their parent.
	 */
	zbits = dns_rpz_find_name(rpzs, rpz_type, zbits, trig_name);
	if (zbits == 0)
		return (ISC_R_SUCCESS);

	dns_fixedname_init(&p_namef);
	p_name = dns_fixedname_name(&p_namef);

	p_zone = NULL;
	p_db = NULL;
	p_node = NULL;

	st = client->query.rpz_st;

	/*
	 * Check the trigger name in every policy zone that the summary data
	 * says has a hit for the trigger name.
	 * Most of the time there are no eligible zones and the summary data
	 * keeps us from getting this far.
	 * We check the most eligible zone first and so usually check only
	 * one policy zone.
	 */
	for (rpz_num = 0; zbits != 0; ++rpz_num, zbits >>= 1) {
		if ((zbits & 1) == 0)
			continue;

		/*
		 * Do not check policy zones that cannot replace a previously
		 * found policy.
		 */
		rpz = rpzs->zones[rpz_num];
		if (st->m.policy != DNS_RPZ_POLICY_MISS) {
			if (st->m.rpz->num < rpz->num)
				break;
			if (st->m.rpz->num == rpz->num &&
			    st->m.type < rpz_type)
				break;
		}

		/*
		 * Get the next policy zone's record for this trigger name.
		 */
		result = rpz_get_p_name(client, p_name, rpz, rpz_type,
					trig_name);
		if (result != ISC_R_SUCCESS)
			continue;
		result = rpz_find_p(client, trig_name, qtype, p_name,
				    rpz, rpz_type,
				    &p_zone, &p_db, &p_version, &p_node,
				    rdatasetp, &policy);
		switch (result) {
		case DNS_R_NXDOMAIN:
			/*
			 * Continue after a missing policy record
			 * contrary to the summary data.  The summary
			 * data can out of date during races with and among
			 * policy zone updates.
			 */
			CTRACE(ISC_LOG_ERROR,
			       "rpz_rewrite_name: mismatched summary data; "
			       "continuing");
			continue;
		case DNS_R_SERVFAIL:
			rpz_clean(&p_zone, &p_db, &p_node, rdatasetp);
			st->m.policy = DNS_RPZ_POLICY_ERROR;
			return (DNS_R_SERVFAIL);
		default:
			/*
			 * With more than one applicable policy, prefer
			 * the earliest configured policy,
			 * client-IP over QNAME over IP over NSDNAME over NSIP,
			 * and the smallest name.
			 * We known st->m.rpz->num >= rpz->num  and either
			 * st->m.rpz->num > rpz->num or st->m.type >= rpz_type
			 */
			if (st->m.policy != DNS_RPZ_POLICY_MISS &&
			    rpz->num == st->m.rpz->num &&
			    (st->m.type < rpz_type ||
			     (st->m.type == rpz_type &&
			      0 >= dns_name_compare(p_name, st->p_name))))
				continue;
#if 0
			/*
			 * This code would block a customer reported information
			 * leak of rpz rules by rewriting requests in the
			 * rpz-ip, rpz-nsip, rpz-nsdname,and rpz-passthru TLDs.
			 * Without this code, a bad guy could request
			 * 24.0.3.2.10.rpz-ip. to find the policy rule for
			 * 10.2.3.0/14.  It is an insignificant leak and this
			 * code is not worth its cost, because the bad guy
			 * could publish "evil.com A 10.2.3.4" and request
			 * evil.com to get the same information.
			 * Keep code with "#if 0" in case customer demand
			 * is irresistible.
			 *
			 * We have the less frequent case of a triggered
			 * policy.  Check that we have not trigger on one
			 * of the pretend RPZ TLDs.
			 * This test would make it impossible to rewrite
			 * names in TLDs that start with "rpz-" should
			 * ICANN ever allow such TLDs.
			 */
			unsigned int labels;
			labels = dns_name_countlabels(trig_name);
			if (labels >= 2) {
				dns_label_t label;

				dns_name_getlabel(trig_name, labels-2, &label);
				if (label.length >= sizeof(DNS_RPZ_PREFIX)-1 &&
				    strncasecmp((const char *)label.base+1,
						DNS_RPZ_PREFIX,
						sizeof(DNS_RPZ_PREFIX)-1) == 0)
					continue;
			}
#endif
			if (rpz->policy != DNS_RPZ_POLICY_DISABLED) {
				CTRACE(ISC_LOG_DEBUG(3),
				       "rpz_rewrite_name: rpz_save_p");
				rpz_save_p(st, rpz, rpz_type,
					   policy, p_name, 0, result,
					   &p_zone, &p_db, &p_node,
					   rdatasetp, p_version);
				/*
				 * After a hit, higher numbered policy zones
				 * are irrelevant
				 */
				rpz_clean(&p_zone, &p_db, &p_node, rdatasetp);
				return (ISC_R_SUCCESS);
			}
			/*
			 * Log DNS_RPZ_POLICY_DISABLED zones
			 * and try the next eligible policy zone.
			 */
			rpz_log_rewrite(client, ISC_TRUE, policy, rpz_type,
					p_zone, p_name);
			break;
		}
	}

	rpz_clean(&p_zone, &p_db, &p_node, rdatasetp);
	return (ISC_R_SUCCESS);
}

static void
rpz_rewrite_ns_skip(ns_client_t *client, dns_name_t *nsname,
		    isc_result_t result, int level, const char *str)
{
	dns_rpz_st_t *st;

	CTRACE(ISC_LOG_DEBUG(3), "rpz_rewrite_ns_skip");

	st = client->query.rpz_st;

	if (str != NULL)
		rpz_log_fail(client, level, nsname, DNS_RPZ_TYPE_NSIP,
			     str, result);
	if (st->r.ns_rdataset != NULL &&
	    dns_rdataset_isassociated(st->r.ns_rdataset))
		dns_rdataset_disassociate(st->r.ns_rdataset);

	st->r.label--;
}

/*
 * Look for response policy zone QNAME, NSIP, and NSDNAME rewriting.
 */
static isc_result_t
rpz_rewrite(ns_client_t *client, dns_rdatatype_t qtype,
	    isc_result_t qresult, isc_boolean_t resuming,
	    dns_rdataset_t *ordataset, dns_rdataset_t *osigset)
{
	dns_rpz_zones_t *rpzs;
	dns_rpz_st_t *st;
	dns_rdataset_t *rdataset;
	dns_fixedname_t nsnamef;
	dns_name_t *nsname;
	int qresult_type;
	dns_rpz_zbits_t zbits;
	isc_result_t result = ISC_R_SUCCESS;
	dns_rpz_have_t have;
	dns_rpz_popt_t popt;
	int rpz_ver;

	CTRACE(ISC_LOG_DEBUG(3), "rpz_rewrite");

	rpzs = client->view->rpzs;
	st = client->query.rpz_st;

	if (rpzs == NULL ||
	    (st != NULL && (st->state & DNS_RPZ_REWRITTEN) != 0))
		return (DNS_R_DISALLOWED);

	RWLOCK(&rpzs->search_lock, isc_rwlocktype_read);
	if (rpzs->p.num_zones == 0 ||
	    (!RECURSIONOK(client) && rpzs->p.no_rd_ok == 0) ||
	    !rpz_ck_dnssec(client, qresult, ordataset, osigset))
	{
		RWUNLOCK(&rpzs->search_lock, isc_rwlocktype_read);
		return (DNS_R_DISALLOWED);
	}
	have = rpzs->have;
	popt = rpzs->p;
	rpz_ver = rpzs->rpz_ver;
	RWUNLOCK(&rpzs->search_lock, isc_rwlocktype_read);

	if (st == NULL) {
		st = isc_mem_get(client->mctx, sizeof(*st));
		if (st == NULL)
			return (ISC_R_NOMEMORY);
		st->state = 0;
	}
	if (st->state == 0) {
		st->state |= DNS_RPZ_ACTIVE;
		memset(&st->m, 0, sizeof(st->m));
		st->m.type = DNS_RPZ_TYPE_BAD;
		st->m.policy = DNS_RPZ_POLICY_MISS;
		st->m.ttl = ~0;
		memset(&st->r, 0, sizeof(st->r));
		memset(&st->q, 0, sizeof(st->q));
		dns_fixedname_init(&st->_p_namef);
		dns_fixedname_init(&st->_r_namef);
		dns_fixedname_init(&st->_fnamef);
		st->p_name = dns_fixedname_name(&st->_p_namef);
		st->r_name = dns_fixedname_name(&st->_r_namef);
		st->fname = dns_fixedname_name(&st->_fnamef);
		st->have = have;
		st->popt = popt;
		st->rpz_ver = rpz_ver;
		client->query.rpz_st = st;
	}

	/*
	 * There is nothing to rewrite if the main query failed.
	 */
	switch (qresult) {
	case ISC_R_SUCCESS:
	case DNS_R_GLUE:
	case DNS_R_ZONECUT:
		qresult_type = 0;
		break;
	case DNS_R_EMPTYNAME:
	case DNS_R_NXRRSET:
	case DNS_R_NXDOMAIN:
	case DNS_R_EMPTYWILD:
	case DNS_R_NCACHENXDOMAIN:
	case DNS_R_NCACHENXRRSET:
	case DNS_R_CNAME:
	case DNS_R_DNAME:
		qresult_type = 1;
		break;
	case DNS_R_DELEGATION:
	case ISC_R_NOTFOUND:
		/*
		 * If recursion is on, do only tentative rewriting.
		 * If recursion is off, this the normal and only time we
		 * can rewrite.
		 */
		if (RECURSIONOK(client))
			qresult_type = 2;
		else
			qresult_type = 1;
		break;
	case ISC_R_FAILURE:
	case ISC_R_TIMEDOUT:
	case DNS_R_BROKENCHAIN:
		rpz_log_fail(client, DNS_RPZ_DEBUG_LEVEL3, client->query.qname,
			     DNS_RPZ_TYPE_QNAME,
			     " stop on qresult in rpz_rewrite()", qresult);
		return (ISC_R_SUCCESS);
	default:
		rpz_log_fail(client, DNS_RPZ_DEBUG_LEVEL1, client->query.qname,
			     DNS_RPZ_TYPE_QNAME,
			     " stop on unrecognized qresult in rpz_rewrite()",
			     qresult);
		return (ISC_R_SUCCESS);
	}

	rdataset = NULL;

	if ((st->state & (DNS_RPZ_DONE_CLIENT_IP | DNS_RPZ_DONE_QNAME)) !=
	    (DNS_RPZ_DONE_CLIENT_IP | DNS_RPZ_DONE_QNAME)) {
		isc_netaddr_t netaddr;
		dns_rpz_zbits_t allowed;

		if (qresult_type == 2) {
			/*
			 * This request needs recursion that has not been done.
			 * Get bits for the policy zones that do not need
			 * to wait for the results of recursion.
			 */
			allowed = st->have.qname_skip_recurse;
			if (allowed == 0)
				return (ISC_R_SUCCESS);
		} else {
			allowed = DNS_RPZ_ALL_ZBITS;
		}

		/*
		 * Check once for triggers for the client IP address.
		 */
		if ((st->state & DNS_RPZ_DONE_CLIENT_IP) == 0) {
			zbits = rpz_get_zbits(client, dns_rdatatype_none,
					      DNS_RPZ_TYPE_CLIENT_IP);
			zbits &= allowed;
			if (zbits != 0) {
				isc_netaddr_fromsockaddr(&netaddr,
							&client->peeraddr);
				result = rpz_rewrite_ip(client, &netaddr, qtype,
							DNS_RPZ_TYPE_CLIENT_IP,
							zbits, &rdataset);
				if (result != ISC_R_SUCCESS)
					goto cleanup;
			}
		}

		/*
		 * Check triggers for the query name if this is the first time
		 * for the current qname.
		 * There is a first time for each name in a CNAME chain
		 */
		if ((st->state & DNS_RPZ_DONE_QNAME) == 0) {
			result = rpz_rewrite_name(client, client->query.qname,
						  qtype, DNS_RPZ_TYPE_QNAME,
						  allowed, &rdataset);
			if (result != ISC_R_SUCCESS)
				goto cleanup;

			/*
			 * Check IPv4 addresses in A RRs next.
			 * Reset to the start of the NS names.
			 */
			st->r.label = dns_name_countlabels(client->query.qname);
			st->state &= ~(DNS_RPZ_DONE_QNAME_IP |
				       DNS_RPZ_DONE_IPv4);

		}

		/*
		 * Quit if this was an attempt to find a qname or
		 * client-IP trigger before recursion.
		 * We will be back if no pre-recursion triggers hit.
		 * For example, consider 2 policy zones, both with qname and
		 * IP address triggers.  If the qname misses the 1st zone,
		 * then we cannot know whether a hit for the qname in the
		 * 2nd zone matters until after recursing to get the A RRs and
		 * testing them in the first zone.
		 * Do not bother saving the work from this attempt,
		 * because recusion is so slow.
		 */
		if (qresult_type == 2)
			goto cleanup;

		/*
		 * DNS_RPZ_DONE_QNAME but not DNS_RPZ_DONE_CLIENT_IP
		 * is reset at the end of dealing with each CNAME.
		 */
		st->state |= (DNS_RPZ_DONE_CLIENT_IP | DNS_RPZ_DONE_QNAME);
	}

	/*
	 * Check known IP addresses for the query name if the database
	 * lookup resulted in some addresses (qresult_type == 0)
	 * and if we have not already checked them.
	 * Any recursion required for the query has already happened.
	 * Do not check addresses that will not be in the ANSWER section.
	 */
	if ((st->state & DNS_RPZ_DONE_QNAME_IP) == 0 && qresult_type == 0 &&
	    rpz_get_zbits(client, qtype, DNS_RPZ_TYPE_IP) != 0) {
		result = rpz_rewrite_ip_rrsets(client,
					       client->query.qname, qtype,
					       DNS_RPZ_TYPE_IP,
					       &rdataset, resuming);
		if (result != ISC_R_SUCCESS)
			goto cleanup;
		/*
		 * We are finished checking the IP addresses for the qname.
		 * Start with IPv4 if we will check NS IP addesses.
		 */
		st->state |= DNS_RPZ_DONE_QNAME_IP;
		st->state &= ~DNS_RPZ_DONE_IPv4;
	}

	/*
	 * Stop looking for rules if there are none of the other kinds
	 * that could override what we already have.
	 */
	if (rpz_get_zbits(client, dns_rdatatype_any,
			  DNS_RPZ_TYPE_NSDNAME) == 0 &&
	    rpz_get_zbits(client, dns_rdatatype_any,
			  DNS_RPZ_TYPE_NSIP) == 0) {
		result = ISC_R_SUCCESS;
		goto cleanup;
	}

	dns_fixedname_init(&nsnamef);
	dns_name_clone(client->query.qname, dns_fixedname_name(&nsnamef));
	while (st->r.label > st->popt.min_ns_labels) {
		/*
		 * Get NS rrset for each domain in the current qname.
		 */
		if (st->r.label == dns_name_countlabels(client->query.qname)) {
			nsname = client->query.qname;
		} else {
			nsname = dns_fixedname_name(&nsnamef);
			dns_name_split(client->query.qname, st->r.label,
				       NULL, nsname);
		}
		if (st->r.ns_rdataset == NULL ||
		    !dns_rdataset_isassociated(st->r.ns_rdataset))
		{
			dns_db_t *db = NULL;
			result = rpz_rrset_find(client, nsname,
						dns_rdatatype_ns,
						DNS_RPZ_TYPE_NSDNAME,
						&db, NULL, &st->r.ns_rdataset,
						resuming);
			if (db != NULL)
				dns_db_detach(&db);
			if (st->m.policy == DNS_RPZ_POLICY_ERROR)
				goto cleanup;
			switch (result) {
			case ISC_R_SUCCESS:
				result = dns_rdataset_first(st->r.ns_rdataset);
				if (result != ISC_R_SUCCESS)
					goto cleanup;
				st->state &= ~(DNS_RPZ_DONE_NSDNAME |
					       DNS_RPZ_DONE_IPv4);
				break;
			case DNS_R_DELEGATION:
			case DNS_R_DUPLICATE:
			case DNS_R_DROP:
				goto cleanup;
			case DNS_R_EMPTYNAME:
			case DNS_R_NXRRSET:
			case DNS_R_EMPTYWILD:
			case DNS_R_NXDOMAIN:
			case DNS_R_NCACHENXDOMAIN:
			case DNS_R_NCACHENXRRSET:
			case ISC_R_NOTFOUND:
			case DNS_R_CNAME:
			case DNS_R_DNAME:
				rpz_rewrite_ns_skip(client, nsname, result,
						    0, NULL);
				continue;
			case ISC_R_TIMEDOUT:
			case DNS_R_BROKENCHAIN:
			case ISC_R_FAILURE:
				rpz_rewrite_ns_skip(client, nsname, result,
						DNS_RPZ_DEBUG_LEVEL3,
						" NS rpz_rrset_find() ");
				continue;
			default:
				rpz_rewrite_ns_skip(client, nsname, result,
						DNS_RPZ_INFO_LEVEL,
						" unrecognized NS"
						" rpz_rrset_find() ");
				continue;
			}
		}
		/*
		 * Check all NS names.
		 */
		do {
			dns_rdata_ns_t ns;
			dns_rdata_t nsrdata = DNS_RDATA_INIT;

			dns_rdataset_current(st->r.ns_rdataset, &nsrdata);
			result = dns_rdata_tostruct(&nsrdata, &ns, NULL);
			dns_rdata_reset(&nsrdata);
			if (result != ISC_R_SUCCESS) {
				rpz_log_fail(client, DNS_RPZ_ERROR_LEVEL,
					     nsname, DNS_RPZ_TYPE_NSIP,
					     " rdata_tostruct()", result);
				st->m.policy = DNS_RPZ_POLICY_ERROR;
				goto cleanup;
			}
			/*
			 * Do nothing about "NS ."
			 */
			if (dns_name_equal(&ns.name, dns_rootname)) {
				dns_rdata_freestruct(&ns);
				result = dns_rdataset_next(st->r.ns_rdataset);
				continue;
			}
			/*
			 * Check this NS name if we did not handle it
			 * during a previous recursion.
			 */
			if ((st->state & DNS_RPZ_DONE_NSDNAME) == 0) {
				result = rpz_rewrite_name(client, &ns.name,
							qtype,
							DNS_RPZ_TYPE_NSDNAME,
							DNS_RPZ_ALL_ZBITS,
							&rdataset);
				if (result != ISC_R_SUCCESS) {
					dns_rdata_freestruct(&ns);
					goto cleanup;
				}
				st->state |= DNS_RPZ_DONE_NSDNAME;
			}
			/*
			 * Check all IP addresses for this NS name.
			 */
			result = rpz_rewrite_ip_rrsets(client, &ns.name, qtype,
						       DNS_RPZ_TYPE_NSIP,
						       &rdataset, resuming);
			dns_rdata_freestruct(&ns);
			if (result != ISC_R_SUCCESS)
				goto cleanup;
			st->state &= ~(DNS_RPZ_DONE_NSDNAME |
				       DNS_RPZ_DONE_IPv4);
			result = dns_rdataset_next(st->r.ns_rdataset);
		} while (result == ISC_R_SUCCESS);
		dns_rdataset_disassociate(st->r.ns_rdataset);
		st->r.label--;

		if (rpz_get_zbits(client, dns_rdatatype_any,
				  DNS_RPZ_TYPE_NSDNAME) == 0 &&
		    rpz_get_zbits(client, dns_rdatatype_any,
				  DNS_RPZ_TYPE_NSIP) == 0)
			break;
	}

	/*
	 * Use the best hit, if any.
	 */
	result = ISC_R_SUCCESS;

cleanup:
	if (st->m.policy != DNS_RPZ_POLICY_MISS &&
	    st->m.policy != DNS_RPZ_POLICY_ERROR &&
	    st->m.rpz->policy != DNS_RPZ_POLICY_GIVEN)
		st->m.policy = st->m.rpz->policy;
	if (st->m.policy == DNS_RPZ_POLICY_MISS ||
	    st->m.policy == DNS_RPZ_POLICY_PASSTHRU ||
	    st->m.policy == DNS_RPZ_POLICY_ERROR) {
		if (st->m.policy == DNS_RPZ_POLICY_PASSTHRU &&
		    result != DNS_R_DELEGATION)
			rpz_log_rewrite(client, ISC_FALSE, st->m.policy,
					st->m.type, st->m.zone, st->p_name);
		rpz_match_clear(st);
	}
	if (st->m.policy == DNS_RPZ_POLICY_ERROR) {
		CTRACE(ISC_LOG_ERROR, "SERVFAIL due to RPZ policy");
		st->m.type = DNS_RPZ_TYPE_BAD;
		result = DNS_R_SERVFAIL;
	}
	query_putrdataset(client, &rdataset);
	if ((st->state & DNS_RPZ_RECURSING) == 0)
		rpz_clean(NULL, &st->r.db, NULL, &st->r.ns_rdataset);

	return (result);
}

/*
 * See if response policy zone rewriting is allowed by a lack of interest
 * by the client in DNSSEC or a lack of signatures.
 */
static isc_boolean_t
rpz_ck_dnssec(ns_client_t *client, isc_result_t qresult,
	      dns_rdataset_t *rdataset, dns_rdataset_t *sigrdataset)
{
	dns_fixedname_t fixed;
	dns_name_t *found;
	dns_rdataset_t trdataset;
	dns_rdatatype_t type;
	isc_result_t result;

	CTRACE(ISC_LOG_DEBUG(3), "rpz_ck_dnssec");

	if (client->view->rpzs->p.break_dnssec || !WANTDNSSEC(client))
		return (ISC_TRUE);

	/*
	 * We do not know if there are signatures if we have not recursed
	 * for them.
	 */
	if (qresult == DNS_R_DELEGATION || qresult == ISC_R_NOTFOUND)
		return (ISC_FALSE);

	if (sigrdataset == NULL)
		return (ISC_TRUE);
	if (dns_rdataset_isassociated(sigrdataset))
		return (ISC_FALSE);

	/*
	 * We are happy to rewrite nothing.
	 */
	if (rdataset == NULL || !dns_rdataset_isassociated(rdataset))
		return (ISC_TRUE);
	/*
	 * Do not rewrite if there is any sign of signatures.
	 */
	if (rdataset->type == dns_rdatatype_nsec ||
	    rdataset->type == dns_rdatatype_nsec3 ||
	    rdataset->type == dns_rdatatype_rrsig)
		return (ISC_FALSE);

	/*
	 * Look for a signature in a negative cache rdataset.
	 */
	if ((rdataset->attributes & DNS_RDATASETATTR_NEGATIVE) == 0)
		return (ISC_TRUE);
	dns_fixedname_init(&fixed);
	found = dns_fixedname_name(&fixed);
	dns_rdataset_init(&trdataset);
	for (result = dns_rdataset_first(rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(rdataset)) {
		dns_ncache_current(rdataset, found, &trdataset);
		type = trdataset.type;
		dns_rdataset_disassociate(&trdataset);
		if (type == dns_rdatatype_nsec ||
		    type == dns_rdatatype_nsec3 ||
		    type == dns_rdatatype_rrsig)
			return (ISC_FALSE);
	}
	return (ISC_TRUE);
}

/*
 * Add a CNAME to the query response, including translating foo.evil.com and
 *	*.evil.com CNAME *.example.com
 * to
 *	foo.evil.com CNAME foo.evil.com.example.com
 */
static isc_result_t
rpz_add_cname(ns_client_t *client, dns_rpz_st_t *st,
	      dns_name_t *cname, dns_name_t *fname, isc_buffer_t *dbuf)
{
	dns_fixedname_t prefix, suffix;
	unsigned int labels;
	isc_result_t result;

	CTRACE(ISC_LOG_DEBUG(3), "rpz_add_cname");

	labels = dns_name_countlabels(cname);
	if (labels > 2 && dns_name_iswildcard(cname)) {
		dns_fixedname_init(&prefix);
		dns_name_split(client->query.qname, 1,
			       dns_fixedname_name(&prefix), NULL);
		dns_fixedname_init(&suffix);
		dns_name_split(cname, labels-1,
			       NULL, dns_fixedname_name(&suffix));
		result = dns_name_concatenate(dns_fixedname_name(&prefix),
					      dns_fixedname_name(&suffix),
					      fname, NULL);
		if (result == DNS_R_NAMETOOLONG)
			client->message->rcode = dns_rcode_yxdomain;
	} else {
		result = dns_name_copy(cname, fname, NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
	}
	if (result != ISC_R_SUCCESS)
		return (result);
	query_keepname(client, fname, dbuf);
	result = query_add_cname(client, client->query.qname,
				 fname, dns_trust_authanswer, st->m.ttl);
	if (result != ISC_R_SUCCESS)
		return (result);
	rpz_log_rewrite(client, ISC_FALSE, st->m.policy,
			st->m.type, st->m.zone, st->p_name);
	ns_client_qnamereplace(client, fname);
	/*
	 * Turn off DNSSEC because the results of a
	 * response policy zone cannot verify.
	 */
	client->attributes &= ~(NS_CLIENTATTR_WANTDNSSEC |
				NS_CLIENTATTR_WANTAD);
	return (ISC_R_SUCCESS);
}

#define MAX_RESTARTS 16

#define QUERY_ERROR(r) \
do { \
	eresult = r; \
	want_restart = ISC_FALSE; \
	line = __LINE__; \
} while (/*CONSTCOND*/0)

#define RECURSE_ERROR(r) \
do { \
	if ((r) == DNS_R_DUPLICATE || (r) == DNS_R_DROP) \
		QUERY_ERROR(r); \
	else \
		QUERY_ERROR(DNS_R_SERVFAIL); \
} while (/*CONSTCOND*/0)

/*
 * Extract a network address from the RDATA of an A or AAAA
 * record.
 *
 * Returns:
 *	ISC_R_SUCCESS
 *	ISC_R_NOTIMPLEMENTED	The rdata is not a known address type.
 */
static isc_result_t
rdata_tonetaddr(const dns_rdata_t *rdata, isc_netaddr_t *netaddr) {
	struct in_addr ina;
	struct in6_addr in6a;

	switch (rdata->type) {
	case dns_rdatatype_a:
		INSIST(rdata->length == 4);
		memmove(&ina.s_addr, rdata->data, 4);
		isc_netaddr_fromin(netaddr, &ina);
		return (ISC_R_SUCCESS);
	case dns_rdatatype_aaaa:
		INSIST(rdata->length == 16);
		memmove(in6a.s6_addr, rdata->data, 16);
		isc_netaddr_fromin6(netaddr, &in6a);
		return (ISC_R_SUCCESS);
	default:
		return (ISC_R_NOTIMPLEMENTED);
	}
}

/*
 * Find the sort order of 'rdata' in the topology-like
 * ACL forming the second element in a 2-element top-level
 * sortlist statement.
 */
static int
query_sortlist_order_2element(const dns_rdata_t *rdata, const void *arg) {
	isc_netaddr_t netaddr;

	if (rdata_tonetaddr(rdata, &netaddr) != ISC_R_SUCCESS)
		return (INT_MAX);
	return (ns_sortlist_addrorder2(&netaddr, arg));
}

/*
 * Find the sort order of 'rdata' in the matching element
 * of a 1-element top-level sortlist statement.
 */
static int
query_sortlist_order_1element(const dns_rdata_t *rdata, const void *arg) {
	isc_netaddr_t netaddr;

	if (rdata_tonetaddr(rdata, &netaddr) != ISC_R_SUCCESS)
		return (INT_MAX);
	return (ns_sortlist_addrorder1(&netaddr, arg));
}

/*
 * Find the sortlist statement that applies to 'client' and set up
 * the sortlist info in in client->message appropriately.
 */
static void
setup_query_sortlist(ns_client_t *client) {
	isc_netaddr_t netaddr;
	dns_rdatasetorderfunc_t order = NULL;
	const void *order_arg = NULL;

	isc_netaddr_fromsockaddr(&netaddr, &client->peeraddr);
	switch (ns_sortlist_setup(client->view->sortlist,
			       &netaddr, &order_arg)) {
	case NS_SORTLISTTYPE_1ELEMENT:
		order = query_sortlist_order_1element;
		break;
	case NS_SORTLISTTYPE_2ELEMENT:
		order = query_sortlist_order_2element;
		break;
	case NS_SORTLISTTYPE_NONE:
		order = NULL;
		break;
	default:
		INSIST(0);
		break;
	}
	dns_message_setsortorder(client->message, order, order_arg);
}

static void
query_addnoqnameproof(ns_client_t *client, dns_rdataset_t *rdataset) {
	isc_buffer_t *dbuf, b;
	dns_name_t *fname;
	dns_rdataset_t *neg, *negsig;
	isc_result_t result = ISC_R_NOMEMORY;

	CTRACE(ISC_LOG_DEBUG(3), "query_addnoqnameproof");

	fname = NULL;
	neg = NULL;
	negsig = NULL;

	dbuf = query_getnamebuf(client);
	if (dbuf == NULL)
		goto cleanup;
	fname = query_newname(client, dbuf, &b);
	neg = query_newrdataset(client);
	negsig = query_newrdataset(client);
	if (fname == NULL || neg == NULL || negsig == NULL)
		goto cleanup;

	result = dns_rdataset_getnoqname(rdataset, fname, neg, negsig);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);

	query_addrrset(client, &fname, &neg, &negsig, dbuf,
		       DNS_SECTION_AUTHORITY);

	if ((rdataset->attributes & DNS_RDATASETATTR_CLOSEST) == 0)
		goto cleanup;

	if (fname == NULL) {
		dbuf = query_getnamebuf(client);
		if (dbuf == NULL)
			goto cleanup;
		fname = query_newname(client, dbuf, &b);
	}
	if (neg == NULL)
		neg = query_newrdataset(client);
	else if (dns_rdataset_isassociated(neg))
		dns_rdataset_disassociate(neg);
	if (negsig == NULL)
		negsig = query_newrdataset(client);
	else if (dns_rdataset_isassociated(negsig))
		dns_rdataset_disassociate(negsig);
	if (fname == NULL || neg == NULL || negsig == NULL)
		goto cleanup;
	result = dns_rdataset_getclosest(rdataset, fname, neg, negsig);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);

	query_addrrset(client, &fname, &neg, &negsig, dbuf,
		       DNS_SECTION_AUTHORITY);

 cleanup:
	if (neg != NULL)
		query_putrdataset(client, &neg);
	if (negsig != NULL)
		query_putrdataset(client, &negsig);
	if (fname != NULL)
		query_releasename(client, &fname);
}

static inline void
answer_in_glue(ns_client_t *client, dns_rdatatype_t qtype) {
	dns_name_t *name;
	dns_message_t *msg;
	dns_section_t section = DNS_SECTION_ADDITIONAL;
	dns_rdataset_t *rdataset = NULL;

	msg = client->message;
	for (name = ISC_LIST_HEAD(msg->sections[section]);
	     name != NULL;
	     name = ISC_LIST_NEXT(name, link))
		if (dns_name_equal(name, client->query.qname)) {
			for (rdataset = ISC_LIST_HEAD(name->list);
			     rdataset != NULL;
			     rdataset = ISC_LIST_NEXT(rdataset, link))
				if (rdataset->type == qtype)
					break;
			break;
		}
	if (rdataset != NULL) {
		ISC_LIST_UNLINK(msg->sections[section], name, link);
		ISC_LIST_PREPEND(msg->sections[section], name, link);
		ISC_LIST_UNLINK(name->list, rdataset, link);
		ISC_LIST_PREPEND(name->list, rdataset, link);
		rdataset->attributes |= DNS_RDATASETATTR_REQUIRED;
	}
}

#define NS_NAME_INIT(A,B) \
	 { \
		DNS_NAME_MAGIC, \
		A, sizeof(A), sizeof(B), \
		DNS_NAMEATTR_READONLY | DNS_NAMEATTR_ABSOLUTE, \
		B, NULL, { (void *)-1, (void *)-1}, \
		{NULL, NULL} \
	}

static unsigned char inaddr10_offsets[] = { 0, 3, 11, 16 };
static unsigned char inaddr172_offsets[] = { 0, 3, 7, 15, 20 };
static unsigned char inaddr192_offsets[] = { 0, 4, 8, 16, 21 };

static unsigned char inaddr10[] = "\00210\007IN-ADDR\004ARPA";

static unsigned char inaddr16172[] = "\00216\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr17172[] = "\00217\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr18172[] = "\00218\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr19172[] = "\00219\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr20172[] = "\00220\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr21172[] = "\00221\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr22172[] = "\00222\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr23172[] = "\00223\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr24172[] = "\00224\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr25172[] = "\00225\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr26172[] = "\00226\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr27172[] = "\00227\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr28172[] = "\00228\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr29172[] = "\00229\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr30172[] = "\00230\003172\007IN-ADDR\004ARPA";
static unsigned char inaddr31172[] = "\00231\003172\007IN-ADDR\004ARPA";

static unsigned char inaddr168192[] = "\003168\003192\007IN-ADDR\004ARPA";

static dns_name_t rfc1918names[] = {
	NS_NAME_INIT(inaddr10, inaddr10_offsets),
	NS_NAME_INIT(inaddr16172, inaddr172_offsets),
	NS_NAME_INIT(inaddr17172, inaddr172_offsets),
	NS_NAME_INIT(inaddr18172, inaddr172_offsets),
	NS_NAME_INIT(inaddr19172, inaddr172_offsets),
	NS_NAME_INIT(inaddr20172, inaddr172_offsets),
	NS_NAME_INIT(inaddr21172, inaddr172_offsets),
	NS_NAME_INIT(inaddr22172, inaddr172_offsets),
	NS_NAME_INIT(inaddr23172, inaddr172_offsets),
	NS_NAME_INIT(inaddr24172, inaddr172_offsets),
	NS_NAME_INIT(inaddr25172, inaddr172_offsets),
	NS_NAME_INIT(inaddr26172, inaddr172_offsets),
	NS_NAME_INIT(inaddr27172, inaddr172_offsets),
	NS_NAME_INIT(inaddr28172, inaddr172_offsets),
	NS_NAME_INIT(inaddr29172, inaddr172_offsets),
	NS_NAME_INIT(inaddr30172, inaddr172_offsets),
	NS_NAME_INIT(inaddr31172, inaddr172_offsets),
	NS_NAME_INIT(inaddr168192, inaddr192_offsets)
};


static unsigned char prisoner_data[] = "\010prisoner\004iana\003org";
static unsigned char hostmaster_data[] = "\012hostmaster\014root-servers\003org";

static unsigned char prisoner_offsets[] = { 0, 9, 14, 18 };
static unsigned char hostmaster_offsets[] = { 0, 11, 24, 28 };

static dns_name_t prisoner = NS_NAME_INIT(prisoner_data, prisoner_offsets);
static dns_name_t hostmaster = NS_NAME_INIT(hostmaster_data, hostmaster_offsets);

static void
warn_rfc1918(ns_client_t *client, dns_name_t *fname, dns_rdataset_t *rdataset) {
	unsigned int i;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdata_soa_t soa;
	dns_rdataset_t found;
	isc_result_t result;

	for (i = 0; i < (sizeof(rfc1918names)/sizeof(*rfc1918names)); i++) {
		if (dns_name_issubdomain(fname, &rfc1918names[i])) {
			dns_rdataset_init(&found);
			result = dns_ncache_getrdataset(rdataset,
							&rfc1918names[i],
							dns_rdatatype_soa,
							&found);
			if (result != ISC_R_SUCCESS)
				return;

			result = dns_rdataset_first(&found);
			RUNTIME_CHECK(result == ISC_R_SUCCESS);
			dns_rdataset_current(&found, &rdata);
			result = dns_rdata_tostruct(&rdata, &soa, NULL);
			RUNTIME_CHECK(result == ISC_R_SUCCESS);
			if (dns_name_equal(&soa.origin, &prisoner) &&
			    dns_name_equal(&soa.contact, &hostmaster)) {
				char buf[DNS_NAME_FORMATSIZE];
				dns_name_format(fname, buf, sizeof(buf));
				ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
					      NS_LOGMODULE_QUERY,
					      ISC_LOG_WARNING,
					      "RFC 1918 response from "
					      "Internet for %s", buf);
			}
			dns_rdataset_disassociate(&found);
			return;
		}
	}
}

static void
query_findclosestnsec3(dns_name_t *qname, dns_db_t *db,
		       dns_dbversion_t *version, ns_client_t *client,
		       dns_rdataset_t *rdataset, dns_rdataset_t *sigrdataset,
		       dns_name_t *fname, isc_boolean_t exact,
		       dns_name_t *found)
{
	unsigned char salt[256];
	size_t salt_length;
	isc_uint16_t iterations;
	isc_result_t result;
	unsigned int dboptions;
	dns_fixedname_t fixed;
	dns_hash_t hash;
	dns_name_t name;
	unsigned int skip = 0, labels;
	dns_rdata_nsec3_t nsec3;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	isc_boolean_t optout;
	dns_clientinfomethods_t cm;
	dns_clientinfo_t ci;

	salt_length = sizeof(salt);
	result = dns_db_getnsec3parameters(db, version, &hash, NULL,
					   &iterations, salt, &salt_length);
	if (result != ISC_R_SUCCESS)
		return;

	dns_name_init(&name, NULL);
	dns_name_clone(qname, &name);
	labels = dns_name_countlabels(&name);
	dns_clientinfomethods_init(&cm, ns_client_sourceip);
	dns_clientinfo_init(&ci, client);

	/*
	 * Map unknown algorithm to known value.
	 */
	if (hash == DNS_NSEC3_UNKNOWNALG)
		hash = 1;

 again:
	dns_fixedname_init(&fixed);
	result = dns_nsec3_hashname(&fixed, NULL, NULL, &name,
				    dns_db_origin(db), hash,
				    iterations, salt, salt_length);
	if (result != ISC_R_SUCCESS)
		return;

	dboptions = client->query.dboptions | DNS_DBFIND_FORCENSEC3;
	result = dns_db_findext(db, dns_fixedname_name(&fixed), version,
				dns_rdatatype_nsec3, dboptions, client->now,
				NULL, fname, &cm, &ci, rdataset, sigrdataset);

	if (result == DNS_R_NXDOMAIN) {
		if (!dns_rdataset_isassociated(rdataset)) {
			return;
		}
		result = dns_rdataset_first(rdataset);
		INSIST(result == ISC_R_SUCCESS);
		dns_rdataset_current(rdataset, &rdata);
		dns_rdata_tostruct(&rdata, &nsec3, NULL);
		dns_rdata_reset(&rdata);
		optout = ISC_TF((nsec3.flags & DNS_NSEC3FLAG_OPTOUT) != 0);
		if (found != NULL && optout &&
		    dns_name_issubdomain(&name, dns_db_origin(db)))
		{
			dns_rdataset_disassociate(rdataset);
			if (dns_rdataset_isassociated(sigrdataset))
				dns_rdataset_disassociate(sigrdataset);
			skip++;
			dns_name_getlabelsequence(qname, skip, labels - skip,
						  &name);
			ns_client_log(client, DNS_LOGCATEGORY_DNSSEC,
				      NS_LOGMODULE_QUERY, ISC_LOG_DEBUG(3),
				      "looking for closest provable encloser");
			goto again;
		}
		if (exact)
			ns_client_log(client, DNS_LOGCATEGORY_DNSSEC,
				      NS_LOGMODULE_QUERY, ISC_LOG_WARNING,
				      "expected a exact match NSEC3, got "
				      "a covering record");

	} else if (result != ISC_R_SUCCESS) {
		return;
	} else if (!exact)
		ns_client_log(client, DNS_LOGCATEGORY_DNSSEC,
			      NS_LOGMODULE_QUERY, ISC_LOG_WARNING,
			      "expected covering NSEC3, got an exact match");
	if (found == qname) {
		if (skip != 0U)
			dns_name_getlabelsequence(qname, skip, labels - skip,
						  found);
	} else if (found != NULL)
		dns_name_copy(&name, found, NULL);
	return;
}

#ifdef ALLOW_FILTER_AAAA
static isc_boolean_t
is_v4_client(ns_client_t *client) {
	if (isc_sockaddr_pf(&client->peeraddr) == AF_INET)
		return (ISC_TRUE);
	if (isc_sockaddr_pf(&client->peeraddr) == AF_INET6 &&
	    IN6_IS_ADDR_V4MAPPED(&client->peeraddr.type.sin6.sin6_addr))
		return (ISC_TRUE);
	return (ISC_FALSE);
}

static isc_boolean_t
is_v6_client(ns_client_t *client) {
	if (isc_sockaddr_pf(&client->peeraddr) == AF_INET6 &&
	    !IN6_IS_ADDR_V4MAPPED(&client->peeraddr.type.sin6.sin6_addr))
		return (ISC_TRUE);
	return (ISC_FALSE);
}
#endif

static isc_uint32_t
dns64_ttl(dns_db_t *db, dns_dbversion_t *version) {
	dns_dbnode_t *node = NULL;
	dns_rdata_soa_t soa;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdataset_t rdataset;
	isc_result_t result;
	isc_uint32_t ttl = ISC_UINT32_MAX;

	dns_rdataset_init(&rdataset);

	result = dns_db_getoriginnode(db, &node);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	result = dns_db_findrdataset(db, node, version, dns_rdatatype_soa,
				     0, 0, &rdataset, NULL);
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	result = dns_rdataset_first(&rdataset);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	dns_rdataset_current(&rdataset, &rdata);
	result = dns_rdata_tostruct(&rdata, &soa, NULL);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);
	ttl = ISC_MIN(rdataset.ttl, soa.minimum);

cleanup:
	if (dns_rdataset_isassociated(&rdataset))
		dns_rdataset_disassociate(&rdataset);
	if (node != NULL)
		dns_db_detachnode(db, &node);
	return (ttl);
}

static isc_boolean_t
dns64_aaaaok(ns_client_t *client, dns_rdataset_t *rdataset,
	     dns_rdataset_t *sigrdataset)
{
	isc_netaddr_t netaddr;
	dns_dns64_t *dns64 = ISC_LIST_HEAD(client->view->dns64);
	unsigned int flags = 0;
	unsigned int i, count;
	isc_boolean_t *aaaaok;

	INSIST(client->query.dns64_aaaaok == NULL);
	INSIST(client->query.dns64_aaaaoklen == 0);
	INSIST(client->query.dns64_aaaa == NULL);
	INSIST(client->query.dns64_sigaaaa == NULL);

	if (dns64 == NULL)
		return (ISC_TRUE);

	if (RECURSIONOK(client))
		flags |= DNS_DNS64_RECURSIVE;

	if (sigrdataset != NULL && dns_rdataset_isassociated(sigrdataset))
		flags |= DNS_DNS64_DNSSEC;

	count = dns_rdataset_count(rdataset);
	aaaaok = isc_mem_get(client->mctx, sizeof(isc_boolean_t) * count);

	isc_netaddr_fromsockaddr(&netaddr, &client->peeraddr);
	if (dns_dns64_aaaaok(dns64, &netaddr, client->signer,
			     &ns_g_server->aclenv, flags, rdataset,
			     aaaaok, count)) {
		for (i = 0; i < count; i++) {
			if (aaaaok != NULL && !aaaaok[i]) {
				client->query.dns64_aaaaok = aaaaok;
				client->query.dns64_aaaaoklen = count;
				break;
			}
		}
		if (i == count && aaaaok != NULL)
			isc_mem_put(client->mctx, aaaaok,
				    sizeof(isc_boolean_t) * count);
		return (ISC_TRUE);
	}
	if (aaaaok != NULL)
		isc_mem_put(client->mctx, aaaaok,
			    sizeof(isc_boolean_t) * count);
	return (ISC_FALSE);
}

/*
 * Look for the name and type in the redirection zone.  If found update
 * the arguments as appropriate.  Return ISC_TRUE if a update was
 * performed.
 *
 * Only perform the update if the client is in the allow query acl and
 * returning the update would not cause a DNSSEC validation failure.
 */
static isc_result_t
redirect(ns_client_t *client, dns_name_t *name, dns_rdataset_t *rdataset,
	 dns_dbnode_t **nodep, dns_db_t **dbp, dns_dbversion_t **versionp,
	 dns_rdatatype_t qtype)
{
	dns_db_t *db = NULL;
	dns_dbnode_t *node = NULL;
	dns_fixedname_t fixed;
	dns_name_t *found;
	dns_rdataset_t trdataset;
	isc_result_t result;
	dns_rdatatype_t type;
	dns_clientinfomethods_t cm;
	dns_clientinfo_t ci;
	ns_dbversion_t *dbversion;

	CTRACE(ISC_LOG_DEBUG(3), "redirect");

	if (client->view->redirect == NULL)
		return (ISC_R_NOTFOUND);

	dns_fixedname_init(&fixed);
	found = dns_fixedname_name(&fixed);
	dns_rdataset_init(&trdataset);

	dns_clientinfomethods_init(&cm, ns_client_sourceip);
	dns_clientinfo_init(&ci, client);

	if (WANTDNSSEC(client) && dns_db_iszone(*dbp) && dns_db_issecure(*dbp))
		return (ISC_R_NOTFOUND);

	if (WANTDNSSEC(client) && dns_rdataset_isassociated(rdataset)) {
		if (rdataset->trust == dns_trust_secure)
			return (ISC_R_NOTFOUND);
		if (rdataset->trust == dns_trust_ultimate &&
		    (rdataset->type == dns_rdatatype_nsec ||
		     rdataset->type == dns_rdatatype_nsec3))
			return (ISC_R_NOTFOUND);
		if ((rdataset->attributes & DNS_RDATASETATTR_NEGATIVE) != 0) {
			for (result = dns_rdataset_first(rdataset);
			     result == ISC_R_SUCCESS;
			     result = dns_rdataset_next(rdataset)) {
				dns_ncache_current(rdataset, found, &trdataset);
				type = trdataset.type;
				dns_rdataset_disassociate(&trdataset);
				if (type == dns_rdatatype_nsec ||
				    type == dns_rdatatype_nsec3 ||
				    type == dns_rdatatype_rrsig)
					return (ISC_R_NOTFOUND);
			}
		}
	}

	result = ns_client_checkaclsilent(client, NULL,
				 dns_zone_getqueryacl(client->view->redirect),
					  ISC_TRUE);
	if (result != ISC_R_SUCCESS)
		return (ISC_R_NOTFOUND);

	result = dns_zone_getdb(client->view->redirect, &db);
	if (result != ISC_R_SUCCESS)
		return (ISC_R_NOTFOUND);

	dbversion = query_findversion(client, db);
	if (dbversion == NULL) {
		dns_db_detach(&db);
		return (ISC_R_NOTFOUND);
	}

	/*
	 * Lookup the requested data in the redirect zone.
	 */
	result = dns_db_findext(db, client->query.qname, dbversion->version,
				qtype, 0, client->now, &node, found, &cm, &ci,
				&trdataset, NULL);
	if (result == DNS_R_NXRRSET || result == DNS_R_NCACHENXRRSET) {
		if (dns_rdataset_isassociated(rdataset))
			dns_rdataset_disassociate(rdataset);
		if (dns_rdataset_isassociated(&trdataset))
			dns_rdataset_disassociate(&trdataset);
		goto nxrrset;
	} else if (result != ISC_R_SUCCESS) {
		if (dns_rdataset_isassociated(&trdataset))
			dns_rdataset_disassociate(&trdataset);
		if (node != NULL)
			dns_db_detachnode(db, &node);
		dns_db_detach(&db);
		return (ISC_R_NOTFOUND);
	}

	CTRACE(ISC_LOG_DEBUG(3), "redirect: found data: done");
	dns_name_copy(found, name, NULL);
	if (dns_rdataset_isassociated(rdataset))
		dns_rdataset_disassociate(rdataset);
	if (dns_rdataset_isassociated(&trdataset)) {
		dns_rdataset_clone(&trdataset, rdataset);
		dns_rdataset_disassociate(&trdataset);
	}
 nxrrset:
	if (*nodep != NULL)
		dns_db_detachnode(*dbp, nodep);
	dns_db_detach(dbp);
	dns_db_attachnode(db, node, nodep);
	dns_db_attach(db, dbp);
	dns_db_detachnode(db, &node);
	dns_db_detach(&db);
	*versionp = dbversion->version;

	client->query.attributes |= (NS_QUERYATTR_NOAUTHORITY |
				     NS_QUERYATTR_NOADDITIONAL);

	return (result);
}

/*
 * Do the bulk of query processing for the current query of 'client'.
 * If 'event' is non-NULL, we are returning from recursion and 'qtype'
 * is ignored.  Otherwise, 'qtype' is the query type.
 */
static isc_result_t
query_find(ns_client_t *client, dns_fetchevent_t *event, dns_rdatatype_t qtype)
{
	dns_db_t *db, *zdb;
	dns_dbnode_t *node;
	dns_rdatatype_t type = qtype;
	dns_name_t *fname, *zfname, *tname, *prefix;
	dns_rdataset_t *rdataset, *trdataset;
	dns_rdataset_t *sigrdataset, *zrdataset, *zsigrdataset;
	dns_rdataset_t **sigrdatasetp;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdatasetiter_t *rdsiter;
	isc_boolean_t want_restart, is_zone, need_wildcardproof;
	isc_boolean_t is_staticstub_zone;
	isc_boolean_t authoritative = ISC_FALSE;
	unsigned int n, nlabels;
	dns_namereln_t namereln;
	int order;
	isc_buffer_t *dbuf;
	isc_buffer_t b;
	isc_result_t result, eresult, tresult;
	dns_fixedname_t fixed;
	dns_fixedname_t wildcardname;
	dns_dbversion_t *version, *zversion;
	dns_zone_t *zone;
	dns_rdata_cname_t cname;
	dns_rdata_dname_t dname;
	unsigned int options;
	isc_boolean_t empty_wild;
	dns_rdataset_t *noqname;
	dns_rpz_st_t *rpz_st;
	isc_boolean_t resuming;
	int line = -1;
	isc_boolean_t dns64_exclude, dns64;
	isc_boolean_t nxrewrite = ISC_FALSE;
	isc_boolean_t redirected = ISC_FALSE;
	dns_clientinfomethods_t cm;
	dns_clientinfo_t ci;
	char errmsg[256];
	isc_boolean_t associated;
	dns_section_t section;
	dns_ttl_t ttl;
#ifdef WANT_QUERYTRACE
	char mbuf[BUFSIZ];
	char qbuf[DNS_NAME_FORMATSIZE];
#endif

	CTRACE(ISC_LOG_DEBUG(3), "query_find");

	/*
	 * One-time initialization.
	 *
	 * It's especially important to initialize anything that the cleanup
	 * code might cleanup.
	 */

	eresult = ISC_R_SUCCESS;
	fname = NULL;
	zfname = NULL;
	rdataset = NULL;
	zrdataset = NULL;
	sigrdataset = NULL;
	zsigrdataset = NULL;
	zversion = NULL;
	node = NULL;
	db = NULL;
	zdb = NULL;
	version = NULL;
	zone = NULL;
	need_wildcardproof = ISC_FALSE;
	empty_wild = ISC_FALSE;
	dns64_exclude = dns64 = ISC_FALSE;
	options = 0;
	resuming = ISC_FALSE;
	is_zone = ISC_FALSE;
	is_staticstub_zone = ISC_FALSE;

	dns_clientinfomethods_init(&cm, ns_client_sourceip);
	dns_clientinfo_init(&ci, client);

#ifdef WANT_QUERYTRACE
	if (client->query.origqname != NULL)
		dns_name_format(client->query.origqname, qbuf,
				sizeof(qbuf));
	else
		snprintf(qbuf, sizeof(qbuf), "<unset>");

	snprintf(mbuf, sizeof(mbuf) - 1,
		 "client attr:0x%x, query attr:0x%X, restarts:%d, "
		 "origqname:%s, timer:%d, authdb:%d, referral:%d",
		 client->attributes,
		 client->query.attributes,
		 client->query.restarts, qbuf,
		 (int) client->query.timerset,
		 (int) client->query.authdbset,
		 (int) client->query.isreferral);
	CTRACE(ISC_LOG_DEBUG(3), mbuf);
#endif

	if (event != NULL) {
		/*
		 * We're returning from recursion.  Restore the query context
		 * and resume.
		 */
		want_restart = ISC_FALSE;

		rpz_st = client->query.rpz_st;
		if (rpz_st != NULL &&
		    (rpz_st->state & DNS_RPZ_RECURSING) != 0)
		{
			CTRACE(ISC_LOG_DEBUG(3), "resume from RPZ recursion");
#ifdef WANT_QUERYTRACE
			{
				char pbuf[DNS_NAME_FORMATSIZE] = "<unset>";
				char fbuf[DNS_NAME_FORMATSIZE] = "<unset>";
				if (rpz_st->r_name != NULL)
					dns_name_format(rpz_st->r_name,
							qbuf, sizeof(qbuf));
				else
					snprintf(qbuf, sizeof(qbuf),
						 "<unset>");
				if (rpz_st->p_name != NULL)
					dns_name_format(rpz_st->p_name,
							pbuf, sizeof(pbuf));
				if (rpz_st->fname != NULL)
					dns_name_format(rpz_st->fname,
							fbuf, sizeof(fbuf));

				snprintf(mbuf, sizeof(mbuf) - 1,
					 "rpz rname:%s, pname:%s, fname:%s",
					 qbuf, pbuf, fbuf);
				CTRACE(ISC_LOG_DEBUG(3), mbuf);
			}
#endif

			is_zone = rpz_st->q.is_zone;
			authoritative = rpz_st->q.authoritative;
			zone = rpz_st->q.zone;
			rpz_st->q.zone = NULL;
			node = rpz_st->q.node;
			rpz_st->q.node = NULL;
			db = rpz_st->q.db;
			rpz_st->q.db = NULL;
			rdataset = rpz_st->q.rdataset;
			rpz_st->q.rdataset = NULL;
			sigrdataset = rpz_st->q.sigrdataset;
			rpz_st->q.sigrdataset = NULL;
			qtype = rpz_st->q.qtype;

			rpz_st->r.db = event->db;
			if (event->node != NULL)
				dns_db_detachnode(event->db, &event->node);
			rpz_st->r.r_type = event->qtype;
			rpz_st->r.r_rdataset = event->rdataset;
			query_putrdataset(client, &event->sigrdataset);
		} else {
			CTRACE(ISC_LOG_DEBUG(3),
			       "resume from normal recursion");
			authoritative = ISC_FALSE;

			qtype = event->qtype;
			db = event->db;
			node = event->node;
			rdataset = event->rdataset;
			sigrdataset = event->sigrdataset;
		}
		INSIST(rdataset != NULL);

		if (qtype == dns_rdatatype_rrsig || qtype == dns_rdatatype_sig)
			type = dns_rdatatype_any;
		else
			type = qtype;

		if (DNS64(client)) {
			client->query.attributes &= ~NS_QUERYATTR_DNS64;
			dns64 = ISC_TRUE;
		}
		if (DNS64EXCLUDE(client)) {
			client->query.attributes &= ~NS_QUERYATTR_DNS64EXCLUDE;
			dns64_exclude = ISC_TRUE;
		}

		if (rpz_st != NULL &&
		    (rpz_st->state & DNS_RPZ_RECURSING) != 0)
		{
			/*
			 * Has response policy changed out from under us?
			 */
			if (rpz_st->rpz_ver != client->view->rpzs->rpz_ver) {
				ns_client_log(client, NS_LOGCATEGORY_CLIENT,
					      NS_LOGMODULE_QUERY, ISC_LOG_INFO,
					      "query_find: RPZ settings "
					      "out of date "
					      "(rpz_ver %d, expected %d)",
					      client->view->rpzs->rpz_ver,
					      rpz_st->rpz_ver);
				QUERY_ERROR(DNS_R_SERVFAIL);
				goto cleanup;
			}
		}

		/*
		 * We'll need some resources...
		 */
		dbuf = query_getnamebuf(client);
		if (dbuf == NULL) {
			CTRACE(ISC_LOG_ERROR,
			       "query_find: query_getnamebuf failed (1)");
			QUERY_ERROR(DNS_R_SERVFAIL);
			goto cleanup;
		}
		fname = query_newname(client, dbuf, &b);
		if (fname == NULL) {
			CTRACE(ISC_LOG_ERROR,
			       "query_find: query_newname failed (1)");
			QUERY_ERROR(DNS_R_SERVFAIL);
			goto cleanup;
		}
		if (rpz_st != NULL &&
		    (rpz_st->state & DNS_RPZ_RECURSING) != 0) {
			tname = rpz_st->fname;
		} else {
			tname = dns_fixedname_name(&event->foundname);
		}
		result = dns_name_copy(tname, fname, NULL);
		if (result != ISC_R_SUCCESS) {
			CTRACE(ISC_LOG_ERROR,
			       "query_find: dns_name_copy failed");
			QUERY_ERROR(DNS_R_SERVFAIL);
			goto cleanup;
		}
		if (rpz_st != NULL &&
		    (rpz_st->state & DNS_RPZ_RECURSING) != 0) {
			rpz_st->r.r_result = event->result;
			result = rpz_st->q.result;
			isc_event_free(ISC_EVENT_PTR(&event));
		} else {
			result = event->result;
		}
		resuming = ISC_TRUE;
		goto resume;
	}

	/*
	 * Not returning from recursion.
	 */

	/*
	 * If it's a SIG query, we'll iterate the node.
	 */
	if (qtype == dns_rdatatype_rrsig || qtype == dns_rdatatype_sig)
		type = dns_rdatatype_any;
	else
		type = qtype;

 restart:
	CTRACE(ISC_LOG_DEBUG(3), "query_find: restart");
	want_restart = ISC_FALSE;
	authoritative = ISC_FALSE;
	version = NULL;
	need_wildcardproof = ISC_FALSE;

	if (client->view->checknames &&
	    !dns_rdata_checkowner(client->query.qname,
				  client->message->rdclass,
				  qtype, ISC_FALSE)) {
		char namebuf[DNS_NAME_FORMATSIZE];
		char typename[DNS_RDATATYPE_FORMATSIZE];
		char classname[DNS_RDATACLASS_FORMATSIZE];

		dns_name_format(client->query.qname, namebuf, sizeof(namebuf));
		dns_rdatatype_format(qtype, typename, sizeof(typename));
		dns_rdataclass_format(client->message->rdclass, classname,
				      sizeof(classname));
		ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
			      NS_LOGMODULE_QUERY, ISC_LOG_ERROR,
			      "check-names failure %s/%s/%s", namebuf,
			      typename, classname);
		QUERY_ERROR(DNS_R_REFUSED);
		goto cleanup;
	}

	/*
	 * First we must find the right database.
	 */
	options &= DNS_GETDB_NOLOG; /* Preserve DNS_GETDB_NOLOG. */
	if (dns_rdatatype_atparent(qtype) &&
	    !dns_name_equal(client->query.qname, dns_rootname))
		options |= DNS_GETDB_NOEXACT;
	result = query_getdb(client, client->query.qname, qtype, options,
			     &zone, &db, &version, &is_zone);
	if (ISC_UNLIKELY((result != ISC_R_SUCCESS || !is_zone) &&
			 qtype == dns_rdatatype_ds &&
			 !RECURSIONOK(client) &&
			 (options & DNS_GETDB_NOEXACT) != 0))
	{
		/*
		 * If the query type is DS, look to see if we are
		 * authoritative for the child zone.
		 */
		dns_db_t *tdb = NULL;
		dns_zone_t *tzone = NULL;
		dns_dbversion_t *tversion = NULL;

		tresult = query_getzonedb(client, client->query.qname, qtype,
					 DNS_GETDB_PARTIAL, &tzone, &tdb,
					 &tversion);
		if (tresult == ISC_R_SUCCESS) {
			options &= ~DNS_GETDB_NOEXACT;
			query_putrdataset(client, &rdataset);
			if (db != NULL)
				dns_db_detach(&db);
			if (zone != NULL)
				dns_zone_detach(&zone);
			version = tversion;
			db = tdb;
			zone = tzone;
			is_zone = ISC_TRUE;
			result = ISC_R_SUCCESS;
		} else {
			if (tdb != NULL)
				dns_db_detach(&tdb);
			if (tzone != NULL)
				dns_zone_detach(&tzone);
		}
	}
	if (result != ISC_R_SUCCESS) {
		if (result == DNS_R_REFUSED) {
			if (WANTRECURSION(client)) {
				inc_stats(client,
					  dns_nsstatscounter_recurserej);
			} else
				inc_stats(client, dns_nsstatscounter_authrej);
			if (!PARTIALANSWER(client))
				QUERY_ERROR(DNS_R_REFUSED);
		} else {
			CTRACE(ISC_LOG_ERROR,
			       "query_find: query_getdb failed");
			QUERY_ERROR(DNS_R_SERVFAIL);
		}
		goto cleanup;
	}

	is_staticstub_zone = ISC_FALSE;
	if (is_zone) {
		authoritative = ISC_TRUE;
		if (zone != NULL &&
		    dns_zone_gettype(zone) == dns_zone_staticstub)
			is_staticstub_zone = ISC_TRUE;
	}

	if (event == NULL && client->query.restarts == 0) {
		if (is_zone) {
			if (zone != NULL) {
				/*
				 * if is_zone = true, zone = NULL then this is
				 * a DLZ zone.  Don't attempt to attach zone.
				 */
				dns_zone_attach(zone, &client->query.authzone);
			}
			dns_db_attach(db, &client->query.authdb);
		}
		client->query.authdbset = ISC_TRUE;

		/* Track TCP vs UDP stats per zone */
		if ((client->attributes & NS_CLIENTATTR_TCP) != 0)
			inc_stats(client, dns_nsstatscounter_tcp);
		else
			inc_stats(client, dns_nsstatscounter_udp);
	}

 db_find:
	CTRACE(ISC_LOG_DEBUG(3), "query_find: db_find");
	/*
	 * We'll need some resources...
	 */
	dbuf = query_getnamebuf(client);
	if (ISC_UNLIKELY(dbuf == NULL)) {
		CTRACE(ISC_LOG_ERROR,
		       "query_find: query_getnamebuf failed (2)");
		QUERY_ERROR(DNS_R_SERVFAIL);
		goto cleanup;
	}
	fname = query_newname(client, dbuf, &b);
	rdataset = query_newrdataset(client);
	if (ISC_UNLIKELY(fname == NULL || rdataset == NULL)) {
		CTRACE(ISC_LOG_ERROR,
		       "query_find: query_newname failed (2)");
		QUERY_ERROR(DNS_R_SERVFAIL);
		goto cleanup;
	}
	if (WANTDNSSEC(client) && (!is_zone || dns_db_issecure(db))) {
		sigrdataset = query_newrdataset(client);
		if (sigrdataset == NULL) {
			CTRACE(ISC_LOG_ERROR,
			       "query_find: query_newrdataset failed (2)");
			QUERY_ERROR(DNS_R_SERVFAIL);
			goto cleanup;
		}
	}

	/*
	 * Now look for an answer in the database.
	 */
	result = dns_db_findext(db, client->query.qname, version, type,
				client->query.dboptions, client->now,
				&node, fname, &cm, &ci, rdataset, sigrdataset);

	if (!is_zone)
		dns_cache_updatestats(client->view->cache, result);

 resume:
	CTRACE(ISC_LOG_DEBUG(3), "query_find: resume");

	/*
	 * Rate limit these responses to this client.
	 * Do not delay counting and handling obvious referrals,
	 *	since those won't come here again.
	 * Delay handling delegations for which we are certain to recurse and
	 *	return here (DNS_R_DELEGATION, not a child of one of our
	 *	own zones, and recursion enabled)
	 * Don't mess with responses rewritten by RPZ
	 * Count each response at most once.
	 */
	if (client->view->rrl != NULL && !HAVESIT(client) &&
	    ((fname != NULL && dns_name_isabsolute(fname)) ||
	     (result == ISC_R_NOTFOUND && !RECURSIONOK(client))) &&
	    !(result == DNS_R_DELEGATION && !is_zone && RECURSIONOK(client)) &&
	    (client->query.rpz_st == NULL ||
	     (client->query.rpz_st->state & DNS_RPZ_REWRITTEN) == 0)&&
	    (client->query.attributes & NS_QUERYATTR_RRL_CHECKED) == 0)
	{
		dns_rdataset_t nc_rdataset;
		isc_boolean_t wouldlog;
		char log_buf[DNS_RRL_LOG_BUF_LEN];
		isc_result_t nc_result, resp_result;
		dns_rrl_result_t rrl_result;

		client->query.attributes |= NS_QUERYATTR_RRL_CHECKED;

		wouldlog = isc_log_wouldlog(ns_g_lctx, DNS_RRL_LOG_DROP);
		tname = fname;
		if (result == DNS_R_NXDOMAIN) {
			/*
			 * Use the database origin name to rate limit NXDOMAIN
			 */
			if (db != NULL)
				tname = dns_db_origin(db);
			resp_result = result;
		} else if (result == DNS_R_NCACHENXDOMAIN &&
			   rdataset != NULL &&
			   dns_rdataset_isassociated(rdataset) &&
			   (rdataset->attributes &
			    DNS_RDATASETATTR_NEGATIVE) != 0) {
			/*
			 * Try to use owner name in the negative cache SOA.
			 */
			dns_fixedname_init(&fixed);
			dns_rdataset_init(&nc_rdataset);
			for (nc_result = dns_rdataset_first(rdataset);
			     nc_result == ISC_R_SUCCESS;
			     nc_result = dns_rdataset_next(rdataset)) {
				dns_ncache_current(rdataset,
						   dns_fixedname_name(&fixed),
						   &nc_rdataset);
				if (nc_rdataset.type == dns_rdatatype_soa) {
					dns_rdataset_disassociate(&nc_rdataset);
					tname = dns_fixedname_name(&fixed);
					break;
				}
				dns_rdataset_disassociate(&nc_rdataset);
			}
			resp_result = DNS_R_NXDOMAIN;
		} else if (result == DNS_R_NXRRSET ||
			   result == DNS_R_EMPTYNAME) {
			resp_result = DNS_R_NXRRSET;
		} else if (result == DNS_R_DELEGATION) {
			resp_result = result;
		} else if (result == ISC_R_NOTFOUND) {
			/*
			 * Handle referral to ".", including when recursion
			 * is off or not requested and the hints have not
			 * been loaded or we have "additional-from-cache no".
			 */
			tname = dns_rootname;
			resp_result = DNS_R_DELEGATION;
		} else {
			resp_result = ISC_R_SUCCESS;
		}
		rrl_result = dns_rrl(client->view, &client->peeraddr,
				     ISC_TF((client->attributes
					     & NS_CLIENTATTR_TCP) != 0),
				     client->message->rdclass, qtype, tname,
				     resp_result, client->now,
				     wouldlog, log_buf, sizeof(log_buf));
		if (rrl_result != DNS_RRL_RESULT_OK) {
			/*
			 * Log dropped or slipped responses in the query
			 * category so that requests are not silently lost.
			 * Starts of rate-limited bursts are logged in
			 * DNS_LOGCATEGORY_RRL.
			 *
			 * Dropped responses are counted with dropped queries
			 * in QryDropped while slipped responses are counted
			 * with other truncated responses in RespTruncated.
			 */
			if (wouldlog) {
				ns_client_log(client, DNS_LOGCATEGORY_RRL,
					      NS_LOGMODULE_QUERY,
					      DNS_RRL_LOG_DROP,
					      "%s", log_buf);
			}
			if (!client->view->rrl->log_only) {
				if (rrl_result == DNS_RRL_RESULT_DROP) {
					/*
					 * These will also be counted in
					 * dns_nsstatscounter_dropped
					 */
					inc_stats(client,
						dns_nsstatscounter_ratedropped);
					QUERY_ERROR(DNS_R_DROP);
				} else {
					/*
					 * These will also be counted in
					 * dns_nsstatscounter_truncatedresp
					 */
					inc_stats(client,
						dns_nsstatscounter_rateslipped);
					client->message->flags |=
							DNS_MESSAGEFLAG_TC;
					if (resp_result == DNS_R_NXDOMAIN)
						client->message->rcode =
							dns_rcode_nxdomain;
				}
				goto cleanup;
			}
		}
	}

	if (!RECURSING(client) &&
	    !dns_name_equal(client->query.qname, dns_rootname))
	{
		isc_result_t rresult;

		rresult = rpz_rewrite(client, qtype, result, resuming,
				      rdataset, sigrdataset);
		rpz_st = client->query.rpz_st;
		switch (rresult) {
		case ISC_R_SUCCESS:
			break;
		case DNS_R_DISALLOWED:
			goto norpz;
		case DNS_R_DELEGATION:
			/*
			 * recursing for NS names or addresses,
			 * so save the main query state
			 */
			rpz_st->q.qtype = qtype;
			rpz_st->q.is_zone = is_zone;
			rpz_st->q.authoritative = authoritative;
			rpz_st->q.zone = zone;
			zone = NULL;
			rpz_st->q.db = db;
			db = NULL;
			rpz_st->q.node = node;
			node = NULL;
			rpz_st->q.rdataset = rdataset;
			rdataset = NULL;
			rpz_st->q.sigrdataset = sigrdataset;
			sigrdataset = NULL;
			dns_name_copy(fname, rpz_st->fname, NULL);
			rpz_st->q.result = result;
			client->query.attributes |= NS_QUERYATTR_RECURSING;
			goto cleanup;
		default:
			RECURSE_ERROR(rresult);
			goto cleanup;
		}

		if (rpz_st->m.policy != DNS_RPZ_POLICY_MISS)
			rpz_st->state |= DNS_RPZ_REWRITTEN;
		if (rpz_st->m.policy != DNS_RPZ_POLICY_MISS &&
		    rpz_st->m.policy != DNS_RPZ_POLICY_PASSTHRU &&
		    (rpz_st->m.policy != DNS_RPZ_POLICY_TCP_ONLY ||
		     (client->attributes & NS_CLIENTATTR_TCP) == 0) &&
		    rpz_st->m.policy != DNS_RPZ_POLICY_ERROR)
		{
			/*
			 * We got a hit and are going to answer with our
			 * fiction. Ensure that we answer with the name
			 * we looked up even if we were stopped short
			 * in recursion or for a deferral.
			 */
			rresult = dns_name_copy(client->query.qname,
						fname, NULL);
			RUNTIME_CHECK(rresult == ISC_R_SUCCESS);
			rpz_clean(&zone, &db, &node, NULL);
			if (rpz_st->m.rdataset != NULL) {
				query_putrdataset(client, &rdataset);
				rdataset = rpz_st->m.rdataset;
				rpz_st->m.rdataset = NULL;
			} else if (rdataset != NULL &&
				   dns_rdataset_isassociated(rdataset)) {
				dns_rdataset_disassociate(rdataset);
			}
			node = rpz_st->m.node;
			rpz_st->m.node = NULL;
			db = rpz_st->m.db;
			rpz_st->m.db = NULL;
			version = rpz_st->m.version;
			rpz_st->m.version = NULL;
			zone = rpz_st->m.zone;
			rpz_st->m.zone = NULL;

			switch (rpz_st->m.policy) {
			case DNS_RPZ_POLICY_TCP_ONLY:
				client->message->flags |= DNS_MESSAGEFLAG_TC;
				if (result == DNS_R_NXDOMAIN ||
				    result == DNS_R_NCACHENXDOMAIN)
					client->message->rcode =
						    dns_rcode_nxdomain;
				rpz_log_rewrite(client, ISC_FALSE,
						rpz_st->m.policy,
						rpz_st->m.type, zone,
						rpz_st->p_name);
				goto cleanup;
			case DNS_RPZ_POLICY_DROP:
				QUERY_ERROR(DNS_R_DROP);
				rpz_log_rewrite(client, ISC_FALSE,
						rpz_st->m.policy,
						rpz_st->m.type, zone,
						rpz_st->p_name);
				goto cleanup;
			case DNS_RPZ_POLICY_NXDOMAIN:
				result = DNS_R_NXDOMAIN;
				nxrewrite = ISC_TRUE;
				break;
			case DNS_RPZ_POLICY_NODATA:
				result = DNS_R_NXRRSET;
				nxrewrite = ISC_TRUE;
				break;
			case DNS_RPZ_POLICY_RECORD:
				result = rpz_st->m.result;
				if (qtype == dns_rdatatype_any &&
				    result != DNS_R_CNAME) {
					/*
					 * We will add all of the rdatasets of
					 * the node by iterating later,
					 * and set the TTL then.
					 */
					if (dns_rdataset_isassociated(rdataset))
					    dns_rdataset_disassociate(rdataset);
				} else {
					/*
					 * We will add this rdataset.
					 */
					rdataset->ttl = ISC_MIN(rdataset->ttl,
								rpz_st->m.ttl);
				}
				break;
			case DNS_RPZ_POLICY_WILDCNAME:
				result = dns_rdataset_first(rdataset);
				RUNTIME_CHECK(result == ISC_R_SUCCESS);
				dns_rdataset_current(rdataset, &rdata);
				result = dns_rdata_tostruct(&rdata, &cname,
							    NULL);
				RUNTIME_CHECK(result == ISC_R_SUCCESS);
				dns_rdata_reset(&rdata);
				result = rpz_add_cname(client, rpz_st,
						       &cname.cname,
						       fname, dbuf);
				if (result != ISC_R_SUCCESS)
					goto cleanup;
				fname = NULL;
				want_restart = ISC_TRUE;
				goto cleanup;
			case DNS_RPZ_POLICY_CNAME:
				/*
				 * Add overridding CNAME from a named.conf
				 * response-policy statement
				 */
				result = rpz_add_cname(client, rpz_st,
						       &rpz_st->m.rpz->cname,
						       fname, dbuf);
				if (result != ISC_R_SUCCESS)
					goto cleanup;
				fname = NULL;
				want_restart = ISC_TRUE;
				goto cleanup;
			default:
				INSIST(0);
			}

			/*
			 * Turn off DNSSEC because the results of a
			 * response policy zone cannot verify.
			 */
			client->attributes &= ~(NS_CLIENTATTR_WANTDNSSEC |
						NS_CLIENTATTR_WANTAD);
			client->message->flags &= ~DNS_MESSAGEFLAG_AD;
			query_putrdataset(client, &sigrdataset);
			rpz_st->q.is_zone = is_zone;
			is_zone = ISC_TRUE;
			rpz_log_rewrite(client, ISC_FALSE, rpz_st->m.policy,
					rpz_st->m.type, zone, rpz_st->p_name);
		}
	}

 norpz:
	switch (result) {
	case ISC_R_SUCCESS:
		/*
		 * This case is handled in the main line below.
		 */
		break;
	case DNS_R_GLUE:
	case DNS_R_ZONECUT:
		/*
		 * These cases are handled in the main line below.
		 */
		INSIST(is_zone);
		authoritative = ISC_FALSE;
		break;
	case ISC_R_NOTFOUND:
		/*
		 * The cache doesn't even have the root NS.  Get them from
		 * the hints DB.
		 */
		INSIST(!is_zone);
		if (db != NULL)
			dns_db_detach(&db);

		if (client->view->hints == NULL) {
			/* We have no hints. */
			result = ISC_R_FAILURE;
		} else {
			dns_db_attach(client->view->hints, &db);
			result = dns_db_findext(db, dns_rootname,
						NULL, dns_rdatatype_ns,
						0, client->now, &node,
						fname, &cm, &ci,
						rdataset, sigrdataset);
		}
		if (result != ISC_R_SUCCESS) {
			/*
			 * Nonsensical root hints may require cleanup.
			 */
			if (dns_rdataset_isassociated(rdataset))
				dns_rdataset_disassociate(rdataset);
			if (sigrdataset != NULL &&
			    dns_rdataset_isassociated(sigrdataset))
				dns_rdataset_disassociate(sigrdataset);
			if (node != NULL)
				dns_db_detachnode(db, &node);

			/*
			 * We don't have any root server hints, but
			 * we may have working forwarders, so try to
			 * recurse anyway.
			 */
			if (RECURSIONOK(client)) {
				result = query_recurse(client, qtype,
						       client->query.qname,
						       NULL, NULL, resuming);
				if (result == ISC_R_SUCCESS) {
					client->query.attributes |=
						NS_QUERYATTR_RECURSING;
					if (dns64)
						client->query.attributes |=
							NS_QUERYATTR_DNS64;
					if (dns64_exclude)
						client->query.attributes |=
						      NS_QUERYATTR_DNS64EXCLUDE;
				} else
					RECURSE_ERROR(result);
				goto cleanup;
			} else {
				/* Unable to give root server referral. */
				CTRACE(ISC_LOG_ERROR,
				       "unable to give root server referral");
				QUERY_ERROR(DNS_R_SERVFAIL);
				goto cleanup;
			}
		}
		/*
		 * XXXRTH  We should trigger root server priming here.
		 */
		/* FALLTHROUGH */
	case DNS_R_DELEGATION:
		authoritative = ISC_FALSE;
		if (is_zone) {
			/*
			 * Look to see if we are authoritative for the
			 * child zone if the query type is DS.
			 */
			if (!RECURSIONOK(client) &&
			    (options & DNS_GETDB_NOEXACT) != 0 &&
			    qtype == dns_rdatatype_ds) {
				dns_db_t *tdb = NULL;
				dns_zone_t *tzone = NULL;
				dns_dbversion_t *tversion = NULL;
				result = query_getzonedb(client,
							 client->query.qname,
							 qtype,
							 DNS_GETDB_PARTIAL,
							 &tzone, &tdb,
							 &tversion);
				if (result == ISC_R_SUCCESS) {
					options &= ~DNS_GETDB_NOEXACT;
					query_putrdataset(client, &rdataset);
					if (sigrdataset != NULL)
						query_putrdataset(client,
								  &sigrdataset);
					if (fname != NULL)
						query_releasename(client,
								  &fname);
					if (node != NULL)
						dns_db_detachnode(db, &node);
					if (db != NULL)
						dns_db_detach(&db);
					if (zone != NULL)
						dns_zone_detach(&zone);
					version = tversion;
					db = tdb;
					zone = tzone;
					authoritative = ISC_TRUE;
					goto db_find;
				}
				if (tdb != NULL)
					dns_db_detach(&tdb);
				if (tzone != NULL)
					dns_zone_detach(&tzone);
			}
			/*
			 * We're authoritative for an ancestor of QNAME.
			 */
			if (!USECACHE(client) || !RECURSIONOK(client)) {
				dns_fixedname_init(&fixed);
				dns_name_copy(fname,
					      dns_fixedname_name(&fixed), NULL);

				/*
				 * If we don't have a cache, this is the best
				 * answer.
				 *
				 * If the client is making a nonrecursive
				 * query we always give out the authoritative
				 * delegation.  This way even if we get
				 * junk in our cache, we won't fail in our
				 * role as the delegating authority if another
				 * nameserver asks us about a delegated
				 * subzone.
				 *
				 * We enable the retrieval of glue for this
				 * database by setting client->query.gluedb.
				 */
				client->query.gluedb = db;
				client->query.isreferral = ISC_TRUE;
				/*
				 * We must ensure NOADDITIONAL is off,
				 * because the generation of
				 * additional data is required in
				 * delegations.
				 */
				client->query.attributes &=
					~NS_QUERYATTR_NOADDITIONAL;
				if (sigrdataset != NULL)
					sigrdatasetp = &sigrdataset;
				else
					sigrdatasetp = NULL;
				query_addrrset(client, &fname,
					       &rdataset, sigrdatasetp,
					       dbuf, DNS_SECTION_AUTHORITY);
				client->query.gluedb = NULL;
				if (WANTDNSSEC(client))
					query_addds(client, db, node, version,
						   dns_fixedname_name(&fixed));
			} else {
				/*
				 * We might have a better answer or delegation
				 * in the cache.  We'll remember the current
				 * values of fname, rdataset, and sigrdataset.
				 * We'll then go looking for QNAME in the
				 * cache.  If we find something better, we'll
				 * use it instead.
				 */
				query_keepname(client, fname, dbuf);
				zdb = db;
				zfname = fname;
				fname = NULL;
				zrdataset = rdataset;
				rdataset = NULL;
				zsigrdataset = sigrdataset;
				sigrdataset = NULL;
				dns_db_detachnode(db, &node);
				zversion = version;
				version = NULL;
				db = NULL;
				dns_db_attach(client->view->cachedb, &db);
				is_zone = ISC_FALSE;
				goto db_find;
			}
		} else {
			if (zfname != NULL &&
			    (!dns_name_issubdomain(fname, zfname) ||
			     (is_staticstub_zone &&
			      dns_name_equal(fname, zfname)))) {
				/*
				 * In the following cases use "authoritative"
				 * data instead of the cache delegation:
				 * 1. We've already got a delegation from
				 *    authoritative data, and it is better
				 *    than what we found in the cache.
				 * 2. The query name matches the origin name
				 *    of a static-stub zone.  This needs to be
				 *    considered for the case where the NS of
				 *    the static-stub zone and the cached NS
				 *    are different.  We still need to contact
				 *    the nameservers configured in the
				 *    static-stub zone.
				 */
				query_releasename(client, &fname);
				fname = zfname;
				zfname = NULL;
				/*
				 * We've already done query_keepname() on
				 * zfname, so we must set dbuf to NULL to
				 * prevent query_addrrset() from trying to
				 * call query_keepname() again.
				 */
				dbuf = NULL;
				query_putrdataset(client, &rdataset);
				if (sigrdataset != NULL)
					query_putrdataset(client,
							  &sigrdataset);
				rdataset = zrdataset;
				zrdataset = NULL;
				sigrdataset = zsigrdataset;
				zsigrdataset = NULL;
				version = zversion;
				zversion = NULL;
				/*
				 * We don't clean up zdb here because we
				 * may still need it.  It will get cleaned
				 * up by the main cleanup code.
				 */
			}

			if (RECURSIONOK(client)) {
				/*
				 * Recurse!
				 */
				if (dns_rdatatype_atparent(type))
					result = query_recurse(client, qtype,
							 client->query.qname,
							 NULL, NULL, resuming);
				else if (dns64)
					result = query_recurse(client,
							 dns_rdatatype_a,
							 client->query.qname,
							 NULL, NULL, resuming);
				else
					result = query_recurse(client, qtype,
							 client->query.qname,
							 fname, rdataset,
							 resuming);

				if (result == ISC_R_SUCCESS) {
					client->query.attributes |=
						NS_QUERYATTR_RECURSING;
					if (dns64)
						client->query.attributes |=
							NS_QUERYATTR_DNS64;
					if (dns64_exclude)
						client->query.attributes |=
						      NS_QUERYATTR_DNS64EXCLUDE;
				} else if (result == DNS_R_DUPLICATE ||
					 result == DNS_R_DROP)
					QUERY_ERROR(result);
				else
					RECURSE_ERROR(result);
			} else {
				dns_fixedname_init(&fixed);
				dns_name_copy(fname,
					      dns_fixedname_name(&fixed), NULL);
				/*
				 * This is the best answer.
				 */
				client->query.attributes |=
					NS_QUERYATTR_CACHEGLUEOK;
				client->query.gluedb = zdb;
				client->query.isreferral = ISC_TRUE;
				/*
				 * We must ensure NOADDITIONAL is off,
				 * because the generation of
				 * additional data is required in
				 * delegations.
				 */
				client->query.attributes &=
					~NS_QUERYATTR_NOADDITIONAL;
				if (sigrdataset != NULL)
					sigrdatasetp = &sigrdataset;
				else
					sigrdatasetp = NULL;
				query_addrrset(client, &fname,
					       &rdataset, sigrdatasetp,
					       dbuf, DNS_SECTION_AUTHORITY);
				client->query.gluedb = NULL;
				client->query.attributes &=
					~NS_QUERYATTR_CACHEGLUEOK;
				if (WANTDNSSEC(client))
					query_addds(client, db, node, version,
						   dns_fixedname_name(&fixed));
			}
		}
		goto cleanup;

	case DNS_R_EMPTYNAME:
	case DNS_R_NXRRSET:
	iszone_nxrrset:
		INSIST(is_zone);

#ifdef dns64_bis_return_excluded_addresses
		if (dns64)
#else
		if (dns64 && !dns64_exclude)
#endif
		{
			/*
			 * Restore the answers from the previous AAAA lookup.
			 */
			if (rdataset != NULL)
				query_putrdataset(client, &rdataset);
			if (sigrdataset != NULL)
				query_putrdataset(client, &sigrdataset);
			rdataset = client->query.dns64_aaaa;
			sigrdataset = client->query.dns64_sigaaaa;
			client->query.dns64_aaaa = NULL;
			client->query.dns64_sigaaaa = NULL;
			if (fname == NULL) {
				dbuf = query_getnamebuf(client);
				if (dbuf == NULL) {
					CTRACE(ISC_LOG_ERROR,
					       "query_find: "
					       "query_getnamebuf failed (3)");
					QUERY_ERROR(DNS_R_SERVFAIL);
					goto cleanup;
				}
				fname = query_newname(client, dbuf, &b);
				if (fname == NULL) {
					CTRACE(ISC_LOG_ERROR,
					       "query_find: "
					       "query_newname failed (3)");
					QUERY_ERROR(DNS_R_SERVFAIL);
					goto cleanup;
				}
			}
			dns_name_copy(client->query.qname, fname, NULL);
			dns64 = ISC_FALSE;
#ifdef dns64_bis_return_excluded_addresses
			/*
			 * Resume the diverted processing of the AAAA response?
			 */
			if (dns64_excluded)
				break;
#endif
		} else if (result == DNS_R_NXRRSET &&
			   !ISC_LIST_EMPTY(client->view->dns64) &&
			   client->message->rdclass == dns_rdataclass_in &&
			   qtype == dns_rdatatype_aaaa)
		{
			/*
			 * Look to see if there are A records for this
			 * name.
			 */
			INSIST(client->query.dns64_aaaa == NULL);
			INSIST(client->query.dns64_sigaaaa == NULL);
			client->query.dns64_aaaa = rdataset;
			client->query.dns64_sigaaaa = sigrdataset;
			client->query.dns64_ttl = dns64_ttl(db, version);
			query_releasename(client, &fname);
			dns_db_detachnode(db, &node);
			rdataset = NULL;
			sigrdataset = NULL;
			type = qtype = dns_rdatatype_a;
			rpz_st = client->query.rpz_st;
			if (rpz_st != NULL) {
				/*
				 * Arrange for RPZ rewriting of any A records.
				 */
				if ((rpz_st->state & DNS_RPZ_REWRITTEN) != 0)
					is_zone = rpz_st->q.is_zone;
				rpz_st_clear(client);
			}
			dns64 = ISC_TRUE;
			goto db_find;
		}

		/*
		 * Look for a NSEC3 record if we don't have a NSEC record.
		 */
 nxrrset_rrsig:
		if (redirected)
			goto cleanup;
		if (!dns_rdataset_isassociated(rdataset) &&
		     WANTDNSSEC(client)) {
			if ((fname->attributes & DNS_NAMEATTR_WILDCARD) == 0) {
				dns_name_t *found;
				dns_name_t *qname;

				dns_fixedname_init(&fixed);
				found = dns_fixedname_name(&fixed);
				qname = client->query.qname;

				query_findclosestnsec3(qname, db, version,
						       client, rdataset,
						       sigrdataset, fname,
						       ISC_TRUE, found);
				/*
				 * Did we find the closest provable encloser
				 * instead? If so add the nearest to the
				 * closest provable encloser.
				 */
				if (dns_rdataset_isassociated(rdataset) &&
				    !dns_name_equal(qname, found) &&
				    !(ns_g_nonearest &&
				      qtype != dns_rdatatype_ds))
				{
					unsigned int count;
					unsigned int skip;

					/*
					 * Add the closest provable encloser.
					 */
					query_addrrset(client, &fname,
						       &rdataset, &sigrdataset,
						       dbuf,
						       DNS_SECTION_AUTHORITY);

					count = dns_name_countlabels(found)
							 + 1;
					skip = dns_name_countlabels(qname) -
							 count;
					dns_name_getlabelsequence(qname, skip,
								  count,
								  found);

					fixfname(client, &fname, &dbuf, &b);
					fixrdataset(client, &rdataset);
					fixrdataset(client, &sigrdataset);
					if (fname == NULL ||
					    rdataset == NULL ||
					    sigrdataset == NULL) {
						CTRACE(ISC_LOG_ERROR,
						       "query_find: "
						       "failure getting "
						       "closest encloser");
						QUERY_ERROR(DNS_R_SERVFAIL);
						goto cleanup;
					}
					/*
					 * 'nearest' doesn't exist so
					 * 'exist' is set to ISC_FALSE.
					 */
					query_findclosestnsec3(found, db,
							       version,
							       client,
							       rdataset,
							       sigrdataset,
							       fname,
							       ISC_FALSE,
							       NULL);
				}
			} else {
				query_releasename(client, &fname);
				query_addwildcardproof(client, db, version,
						       client->query.qname,
						       ISC_FALSE, ISC_TRUE);
			}
		}
		if (dns_rdataset_isassociated(rdataset)) {
			/*
			 * If we've got a NSEC record, we need to save the
			 * name now because we're going call query_addsoa()
			 * below, and it needs to use the name buffer.
			 */
			query_keepname(client, fname, dbuf);
		} else if (fname != NULL) {
			/*
			 * We're not going to use fname, and need to release
			 * our hold on the name buffer so query_addsoa()
			 * may use it.
			 */
			query_releasename(client, &fname);
		}

		/*
		 * Add SOA to the additional section if generated by a RPZ
		 * rewrite.
		 */
		associated = dns_rdataset_isassociated(rdataset);
		section = nxrewrite ? DNS_SECTION_ADDITIONAL :
				      DNS_SECTION_AUTHORITY;

		result = query_addsoa(client, db, version, ISC_UINT32_MAX,
				      associated, section);
		if (result != ISC_R_SUCCESS) {
			QUERY_ERROR(result);
			goto cleanup;
		}

		/*
		 * Add NSEC record if we found one.
		 */
		if (WANTDNSSEC(client)) {
			if (dns_rdataset_isassociated(rdataset))
				query_addnxrrsetnsec(client, db, version,
						     &fname, &rdataset,
						     &sigrdataset);
		}
		goto cleanup;

	case DNS_R_EMPTYWILD:
		empty_wild = ISC_TRUE;
		/* FALLTHROUGH */

	case DNS_R_NXDOMAIN:
		INSIST(is_zone);
		if (!empty_wild) {
			tresult = redirect(client, fname, rdataset, &node,
					   &db, &version, type);
			if (tresult == ISC_R_SUCCESS)
				break;
			if (tresult == DNS_R_NXRRSET) {
				redirected = ISC_TRUE;
				goto iszone_nxrrset;
			}
			if (tresult == DNS_R_NCACHENXRRSET) {
				redirected = ISC_TRUE;
				is_zone = ISC_FALSE;
				goto ncache_nxrrset;
			}
		}
		if (dns_rdataset_isassociated(rdataset)) {
			/*
			 * If we've got a NSEC record, we need to save the
			 * name now because we're going call query_addsoa()
			 * below, and it needs to use the name buffer.
			 */
			query_keepname(client, fname, dbuf);
		} else if (fname != NULL) {
			/*
			 * We're not going to use fname, and need to release
			 * our hold on the name buffer so query_addsoa()
			 * may use it.
			 */
			query_releasename(client, &fname);
		}

		/*
		 * Add SOA to the additional section if generated by a
		 * RPZ rewrite.
		 *
		 * If the query was for a SOA record force the
		 * ttl to zero so that it is possible for clients to find
		 * the containing zone of an arbitrary name with a stub
		 * resolver and not have it cached.
		 */
		associated = dns_rdataset_isassociated(rdataset);
		section = nxrewrite ? DNS_SECTION_ADDITIONAL :
				      DNS_SECTION_AUTHORITY;
		ttl = ISC_UINT32_MAX;
		if (!nxrewrite && qtype == dns_rdatatype_soa &&
		    zone != NULL && dns_zone_getzeronosoattl(zone))
			ttl = 0;
		result = query_addsoa(client, db, version, ttl, associated,
				      section);
		if (result != ISC_R_SUCCESS) {
			QUERY_ERROR(result);
			goto cleanup;
		}

		if (WANTDNSSEC(client)) {
			/*
			 * Add NSEC record if we found one.
			 */
			if (dns_rdataset_isassociated(rdataset))
				query_addrrset(client, &fname, &rdataset,
					       &sigrdataset,
					       NULL, DNS_SECTION_AUTHORITY);
			query_addwildcardproof(client, db, version,
					       client->query.qname, ISC_FALSE,
					       ISC_FALSE);
		}

		/*
		 * Set message rcode.
		 */
		if (empty_wild)
			client->message->rcode = dns_rcode_noerror;
		else
			client->message->rcode = dns_rcode_nxdomain;
		goto cleanup;

	case DNS_R_NCACHENXDOMAIN:
		tresult = redirect(client, fname, rdataset, &node,
				   &db, &version, type);
		if (tresult == ISC_R_SUCCESS)
			break;
		if (tresult == DNS_R_NXRRSET) {
			redirected = ISC_TRUE;
			is_zone = ISC_TRUE;
			goto iszone_nxrrset;
		}
		if (tresult == DNS_R_NCACHENXRRSET) {
			redirected = ISC_TRUE;
			result = tresult;
			goto ncache_nxrrset;
		}
		/* FALLTHROUGH */

	case DNS_R_NCACHENXRRSET:
	ncache_nxrrset:
		INSIST(!is_zone);
		authoritative = ISC_FALSE;
		/*
		 * Set message rcode, if required.
		 */
		if (result == DNS_R_NCACHENXDOMAIN)
			client->message->rcode = dns_rcode_nxdomain;
		/*
		 * Look for RFC 1918 leakage from Internet.
		 */
		if (result == DNS_R_NCACHENXDOMAIN &&
		    qtype == dns_rdatatype_ptr &&
		    client->message->rdclass == dns_rdataclass_in &&
		    dns_name_countlabels(fname) == 7)
			warn_rfc1918(client, fname, rdataset);

#ifdef dns64_bis_return_excluded_addresses
		if (dns64)
#else
		if (dns64 && !dns64_exclude)
#endif
		{
			/*
			 * Restore the answers from the previous AAAA lookup.
			 */
			if (rdataset != NULL)
				query_putrdataset(client, &rdataset);
			if (sigrdataset != NULL)
				query_putrdataset(client, &sigrdataset);
			rdataset = client->query.dns64_aaaa;
			sigrdataset = client->query.dns64_sigaaaa;
			client->query.dns64_aaaa = NULL;
			client->query.dns64_sigaaaa = NULL;
			if (fname == NULL) {
				dbuf = query_getnamebuf(client);
				if (dbuf == NULL) {
					CTRACE(ISC_LOG_ERROR,
					       "query_find: "
					       "query_getnamebuf failed (4)");
					QUERY_ERROR(DNS_R_SERVFAIL);
					goto cleanup;
				}
				fname = query_newname(client, dbuf, &b);
				if (fname == NULL) {
					CTRACE(ISC_LOG_ERROR,
					       "query_find: "
					       "query_newname failed (4)");
					QUERY_ERROR(DNS_R_SERVFAIL);
					goto cleanup;
				}
			}
			dns_name_copy(client->query.qname, fname, NULL);
			dns64 = ISC_FALSE;
#ifdef dns64_bis_return_excluded_addresses
			if (dns64_excluded)
				break;
#endif
		} else if (result == DNS_R_NCACHENXRRSET &&
			   !ISC_LIST_EMPTY(client->view->dns64) &&
			   client->message->rdclass == dns_rdataclass_in &&
			   qtype == dns_rdatatype_aaaa)
		{
			/*
			 * Look to see if there are A records for this
			 * name.
			 */
			INSIST(client->query.dns64_aaaa == NULL);
			INSIST(client->query.dns64_sigaaaa == NULL);
			client->query.dns64_aaaa = rdataset;
			client->query.dns64_sigaaaa = sigrdataset;
			/*
			 * If the ttl is zero we need to workout if we have just
			 * decremented to zero or if there was no negative cache
			 * ttl in the answer.
			 */
			if (rdataset->ttl != 0)
				client->query.dns64_ttl = rdataset->ttl;
			else if (dns_rdataset_first(rdataset) == ISC_R_SUCCESS)
				client->query.dns64_ttl = 0;
			query_releasename(client, &fname);
			dns_db_detachnode(db, &node);
			rdataset = NULL;
			sigrdataset = NULL;
			fname = NULL;
			type = qtype = dns_rdatatype_a;
			rpz_st = client->query.rpz_st;
			if (rpz_st != NULL) {
				/*
				 * Arrange for RPZ rewriting of any A records.
				 */
				if ((rpz_st->state & DNS_RPZ_REWRITTEN) != 0)
					is_zone = rpz_st->q.is_zone;
				rpz_st_clear(client);
			}
			dns64 = ISC_TRUE;
			goto db_find;
		}

		/*
		 * We don't call query_addrrset() because we don't need any
		 * of its extra features (and things would probably break!).
		 */
		query_keepname(client, fname, dbuf);
		dns_message_addname(client->message, fname,
				    DNS_SECTION_AUTHORITY);
		ISC_LIST_APPEND(fname->list, rdataset, link);
		fname = NULL;
		rdataset = NULL;
		goto cleanup;

	case DNS_R_CNAME:
		/*
		 * Keep a copy of the rdataset.  We have to do this because
		 * query_addrrset may clear 'rdataset' (to prevent the
		 * cleanup code from cleaning it up).
		 */
		trdataset = rdataset;
		/*
		 * Add the CNAME to the answer section.
		 */
		if (sigrdataset != NULL)
			sigrdatasetp = &sigrdataset;
		else
			sigrdatasetp = NULL;
		if (WANTDNSSEC(client) &&
		    (fname->attributes & DNS_NAMEATTR_WILDCARD) != 0)
		{
			dns_fixedname_init(&wildcardname);
			dns_name_copy(fname, dns_fixedname_name(&wildcardname),
				      NULL);
			need_wildcardproof = ISC_TRUE;
		}
		if (NOQNAME(rdataset) && WANTDNSSEC(client))
			noqname = rdataset;
		else
			noqname = NULL;
		if (!is_zone && RECURSIONOK(client))
			query_prefetch(client, fname, rdataset);
		query_addrrset(client, &fname, &rdataset, sigrdatasetp, dbuf,
			       DNS_SECTION_ANSWER);
		if (noqname != NULL)
			query_addnoqnameproof(client, noqname);
		/*
		 * We set the PARTIALANSWER attribute so that if anything goes
		 * wrong later on, we'll return what we've got so far.
		 */
		client->query.attributes |= NS_QUERYATTR_PARTIALANSWER;
		/*
		 * Reset qname to be the target name of the CNAME and restart
		 * the query.
		 */
		tname = NULL;
		result = dns_message_gettempname(client->message, &tname);
		if (result != ISC_R_SUCCESS)
			goto cleanup;
		result = dns_rdataset_first(trdataset);
		if (result != ISC_R_SUCCESS) {
			dns_message_puttempname(client->message, &tname);
			goto cleanup;
		}
		dns_rdataset_current(trdataset, &rdata);
		result = dns_rdata_tostruct(&rdata, &cname, NULL);
		dns_rdata_reset(&rdata);
		if (result != ISC_R_SUCCESS) {
			dns_message_puttempname(client->message, &tname);
			goto cleanup;
		}
		dns_name_init(tname, NULL);
		result = dns_name_dup(&cname.cname, client->mctx, tname);
		if (result != ISC_R_SUCCESS) {
			dns_message_puttempname(client->message, &tname);
			dns_rdata_freestruct(&cname);
			goto cleanup;
		}
		dns_rdata_freestruct(&cname);
		ns_client_qnamereplace(client, tname);
		want_restart = ISC_TRUE;
		if (!WANTRECURSION(client))
			options |= DNS_GETDB_NOLOG;
		goto addauth;
	case DNS_R_DNAME:
		/*
		 * Compare the current qname to the found name.  We need
		 * to know how many labels and bits are in common because
		 * we're going to have to split qname later on.
		 */
		namereln = dns_name_fullcompare(client->query.qname, fname,
						&order, &nlabels);
		INSIST(namereln == dns_namereln_subdomain);
		/*
		 * Keep a copy of the rdataset.  We have to do this because
		 * query_addrrset may clear 'rdataset' (to prevent the
		 * cleanup code from cleaning it up).
		 */
		trdataset = rdataset;
		/*
		 * Add the DNAME to the answer section.
		 */
		if (sigrdataset != NULL)
			sigrdatasetp = &sigrdataset;
		else
			sigrdatasetp = NULL;
		if (WANTDNSSEC(client) &&
		    (fname->attributes & DNS_NAMEATTR_WILDCARD) != 0)
		{
			dns_fixedname_init(&wildcardname);
			dns_name_copy(fname, dns_fixedname_name(&wildcardname),
				      NULL);
			need_wildcardproof = ISC_TRUE;
		}
		if (!is_zone && RECURSIONOK(client))
			query_prefetch(client, fname, rdataset);
		query_addrrset(client, &fname, &rdataset, sigrdatasetp, dbuf,
			       DNS_SECTION_ANSWER);
		/*
		 * We set the PARTIALANSWER attribute so that if anything goes
		 * wrong later on, we'll return what we've got so far.
		 */
		client->query.attributes |= NS_QUERYATTR_PARTIALANSWER;
		/*
		 * Get the target name of the DNAME.
		 */
		tname = NULL;
		result = dns_message_gettempname(client->message, &tname);
		if (result != ISC_R_SUCCESS)
			goto cleanup;
		result = dns_rdataset_first(trdataset);
		if (result != ISC_R_SUCCESS) {
			dns_message_puttempname(client->message, &tname);
			goto cleanup;
		}
		dns_rdataset_current(trdataset, &rdata);
		result = dns_rdata_tostruct(&rdata, &dname, NULL);
		dns_rdata_reset(&rdata);
		if (result != ISC_R_SUCCESS) {
			dns_message_puttempname(client->message, &tname);
			goto cleanup;
		}
		dns_name_clone(&dname.dname, tname);
		dns_rdata_freestruct(&dname);
		/*
		 * Construct the new qname consisting of
		 * <found name prefix>.<dname target>
		 */
		dns_fixedname_init(&fixed);
		prefix = dns_fixedname_name(&fixed);
		dns_name_split(client->query.qname, nlabels, prefix, NULL);
		INSIST(fname == NULL);
		dbuf = query_getnamebuf(client);
		if (dbuf == NULL) {
			dns_message_puttempname(client->message, &tname);
			goto cleanup;
		}
		fname = query_newname(client, dbuf, &b);
		if (fname == NULL) {
			dns_message_puttempname(client->message, &tname);
			goto cleanup;
		}
		result = dns_name_concatenate(prefix, tname, fname, NULL);
		dns_message_puttempname(client->message, &tname);

		/*
		 * RFC2672, section 4.1, subsection 3c says
		 * we should return YXDOMAIN if the constructed
		 * name would be too long.
		 */
		if (result == DNS_R_NAMETOOLONG)
			client->message->rcode = dns_rcode_yxdomain;
		if (result != ISC_R_SUCCESS)
			goto cleanup;

		query_keepname(client, fname, dbuf);
		/*
		 * Synthesize a CNAME consisting of
		 *   <old qname> <dname ttl> CNAME <new qname>
		 *	    with <dname trust value>
		 *
		 * Synthesize a CNAME so old old clients that don't understand
		 * DNAME can chain.
		 *
		 * We do not try to synthesize a signature because we hope
		 * that security aware servers will understand DNAME.  Also,
		 * even if we had an online key, making a signature
		 * on-the-fly is costly, and not really legitimate anyway
		 * since the synthesized CNAME is NOT in the zone.
		 */
		result = query_add_cname(client, client->query.qname, fname,
					 trdataset->trust, trdataset->ttl);
		if (result != ISC_R_SUCCESS)
			goto cleanup;
		/*
		 * Switch to the new qname and restart.
		 */
		ns_client_qnamereplace(client, fname);
		fname = NULL;
		want_restart = ISC_TRUE;
		if (!WANTRECURSION(client))
			options |= DNS_GETDB_NOLOG;
		goto addauth;
	default:
		/*
		 * Something has gone wrong.
		 */
		snprintf(errmsg, sizeof(errmsg) - 1,
			 "query_find: unexpected error after resuming: %s",
			 isc_result_totext(result));
		CTRACE(ISC_LOG_ERROR, errmsg);
		QUERY_ERROR(DNS_R_SERVFAIL);
		goto cleanup;
	}

	if (WANTDNSSEC(client) &&
	    (fname->attributes & DNS_NAMEATTR_WILDCARD) != 0)
	{
		dns_fixedname_init(&wildcardname);
		dns_name_copy(fname, dns_fixedname_name(&wildcardname), NULL);
		need_wildcardproof = ISC_TRUE;
	}

#ifdef ALLOW_FILTER_AAAA
	/*
	 * The filter-aaaa-on-v4 option should suppress AAAAs for IPv4
	 * clients if there is an A; filter-aaaa-on-v6 option does the same
	 * for IPv6 clients.
	 */
	client->filter_aaaa = dns_aaaa_ok;
	if (client->view->v4_aaaa != dns_aaaa_ok ||
	    client->view->v6_aaaa != dns_aaaa_ok)
	{
		result = ns_client_checkaclsilent(client, NULL,
						  client->view->aaaa_acl,
						  ISC_TRUE);
		if (result == ISC_R_SUCCESS &&
		    client->view->v4_aaaa != dns_aaaa_ok &&
		    is_v4_client(client))
			client->filter_aaaa = client->view->v4_aaaa;
		else if (result == ISC_R_SUCCESS &&
			 client->view->v6_aaaa != dns_aaaa_ok &&
			 is_v6_client(client))
			client->filter_aaaa = client->view->v6_aaaa;
	}

#endif

	if (type == dns_rdatatype_any) {
#ifdef ALLOW_FILTER_AAAA
		isc_boolean_t have_aaaa, have_a, have_sig;

		/*
		 * If we are not authoritative, assume there is a A
		 * even in if it is not in our cache.  This assumption could
		 * be wrong but it is a good bet.
		 */
		have_aaaa = ISC_FALSE;
		have_a = !authoritative;
		have_sig = ISC_FALSE;
#endif
		/*
		 * XXXRTH  Need to handle zonecuts with special case
		 * code.
		 */
		n = 0;
		rdsiter = NULL;
		result = dns_db_allrdatasets(db, node, version, 0, &rdsiter);
		if (result != ISC_R_SUCCESS) {
			CTRACE(ISC_LOG_ERROR,
			       "query_find: type any; allrdatasets failed");
			QUERY_ERROR(DNS_R_SERVFAIL);
			goto cleanup;
		}

		/*
		 * Calling query_addrrset() with a non-NULL dbuf is going
		 * to either keep or release the name.  We don't want it to
		 * release fname, since we may have to call query_addrrset()
		 * more than once.  That means we have to call query_keepname()
		 * now, and pass a NULL dbuf to query_addrrset().
		 *
		 * If we do a query_addrrset() below, we must set fname to
		 * NULL before leaving this block, otherwise we might try to
		 * cleanup fname even though we're using it!
		 */
		query_keepname(client, fname, dbuf);
		tname = fname;
		result = dns_rdatasetiter_first(rdsiter);
		while (result == ISC_R_SUCCESS) {
			dns_rdatasetiter_current(rdsiter, rdataset);
#ifdef ALLOW_FILTER_AAAA
			/*
			 * Notice the presence of A and AAAAs so
			 * that AAAAs can be hidden from IPv4 clients.
			 */
			if (client->filter_aaaa != dns_aaaa_ok) {
				if (rdataset->type == dns_rdatatype_aaaa)
					have_aaaa = ISC_TRUE;
				else if (rdataset->type == dns_rdatatype_a)
					have_a = ISC_TRUE;
			}
#endif
			if (is_zone && qtype == dns_rdatatype_any &&
			    !dns_db_issecure(db) &&
			    dns_rdatatype_isdnssec(rdataset->type)) {
				/*
				 * The zone is transitioning from insecure
				 * to secure. Hide the dnssec records from
				 * ANY queries.
				 */
				dns_rdataset_disassociate(rdataset);
			} else if ((qtype == dns_rdatatype_any ||
			     rdataset->type == qtype) && rdataset->type != 0) {
#ifdef ALLOW_FILTER_AAAA
				if (dns_rdatatype_isdnssec(rdataset->type))
					have_sig = ISC_TRUE;
#endif
				if (NOQNAME(rdataset) && WANTDNSSEC(client))
					noqname = rdataset;
				else
					noqname = NULL;
				rpz_st = client->query.rpz_st;
				if (rpz_st != NULL)
					rdataset->ttl = ISC_MIN(rdataset->ttl,
							    rpz_st->m.ttl);
				if (!is_zone && RECURSIONOK(client)) {
					dns_name_t *name;
					name = (fname != NULL) ? fname : tname;
					query_prefetch(client, name, rdataset);
				}
				query_addrrset(client,
					       fname != NULL ? &fname : &tname,
					       &rdataset, NULL,
					       NULL, DNS_SECTION_ANSWER);
				if (noqname != NULL)
					query_addnoqnameproof(client, noqname);
				n++;
				INSIST(tname != NULL);
				/*
				 * rdataset is non-NULL only in certain
				 * pathological cases involving DNAMEs.
				 */
				if (rdataset != NULL)
					query_putrdataset(client, &rdataset);
				rdataset = query_newrdataset(client);
				if (rdataset == NULL)
					break;
			} else {
				/*
				 * We're not interested in this rdataset.
				 */
				dns_rdataset_disassociate(rdataset);
			}
			result = dns_rdatasetiter_next(rdsiter);
		}

#ifdef ALLOW_FILTER_AAAA
		/*
		 * Filter AAAAs if there is an A and there is no signature
		 * or we are supposed to break DNSSEC.
		 */
		if (client->filter_aaaa == dns_aaaa_break_dnssec)
			client->attributes |= NS_CLIENTATTR_FILTER_AAAA;
		else if (client->filter_aaaa != dns_aaaa_ok &&
			 have_aaaa && have_a &&
			 (!have_sig || !WANTDNSSEC(client)))
			  client->attributes |= NS_CLIENTATTR_FILTER_AAAA;
#endif
		if (fname != NULL)
			dns_message_puttempname(client->message, &fname);

		if (n == 0) {
			/*
			 * No matching rdatasets found in cache. If we were
			 * searching for RRSIG/SIG, that's probably okay;
			 * otherwise this is an error condition.
			 */
			if ((qtype == dns_rdatatype_rrsig ||
			     qtype == dns_rdatatype_sig) &&
			    result == ISC_R_NOMORE) {
				if (!is_zone) {
					authoritative = ISC_FALSE;
					dns_rdatasetiter_destroy(&rdsiter);
					client->attributes &= ~NS_CLIENTATTR_RA;
					goto addauth;
				}

				if (qtype == dns_rdatatype_rrsig &&
				    dns_db_issecure(db)) {
					char namebuf[DNS_NAME_FORMATSIZE];
					dns_name_format(client->query.qname,
							namebuf,
							sizeof(namebuf));
					ns_client_log(client,
						      DNS_LOGCATEGORY_DNSSEC,
						      NS_LOGMODULE_QUERY,
						      ISC_LOG_WARNING,
						      "missing signature "
						      "for %s", namebuf);
				}

				dns_rdatasetiter_destroy(&rdsiter);
				fname = query_newname(client, dbuf, &b);
				goto nxrrset_rrsig;
			} else {
				CTRACE(ISC_LOG_ERROR,
				       "query_find: no matching rdatasets "
				       "in cache");
				result = DNS_R_SERVFAIL;
			}
		}

		dns_rdatasetiter_destroy(&rdsiter);
		if (result != ISC_R_NOMORE) {
			CTRACE(ISC_LOG_ERROR,
			       "query_find: dns_rdatasetiter_destroy failed");
			QUERY_ERROR(DNS_R_SERVFAIL);
			goto cleanup;
		}
	} else {
		/*
		 * This is the "normal" case -- an ordinary question to which
		 * we know the answer.
		 */

#ifdef ALLOW_FILTER_AAAA
		/*
		 * Optionally hide AAAAs from IPv4 clients if there is an A.
		 * We add the AAAAs now, but might refuse to render them later
		 * after DNSSEC is figured out.
		 * This could be more efficient, but the whole idea is
		 * so fundamentally wrong, unavoidably inaccurate, and
		 * unneeded that it is best to keep it as short as possible.
		 */
		if (client->filter_aaaa == dns_aaaa_break_dnssec ||
		    (client->filter_aaaa == dns_aaaa_filter &&
		     (!WANTDNSSEC(client) || sigrdataset == NULL ||
		     !dns_rdataset_isassociated(sigrdataset))))
		{
			if (qtype == dns_rdatatype_aaaa) {
				trdataset = query_newrdataset(client);
				result = dns_db_findrdataset(db, node, version,
							     dns_rdatatype_a, 0,
							     client->now,
							     trdataset, NULL);
				if (dns_rdataset_isassociated(trdataset))
					dns_rdataset_disassociate(trdataset);
				query_putrdataset(client, &trdataset);

				/*
				 * We have an AAAA but the A is not in our cache.
				 * Assume any result other than DNS_R_DELEGATION
				 * or ISC_R_NOTFOUND means there is no A and
				 * so AAAAs are ok.
				 * Assume there is no A if we can't recurse
				 * for this client, although that could be
				 * the wrong answer. What else can we do?
				 * Besides, that we have the AAAA and are using
				 * this mechanism suggests that we care more
				 * about As than AAAAs and would have cached
				 * the A if it existed.
				 */
				if (result == ISC_R_SUCCESS) {
					client->attributes |=
						    NS_CLIENTATTR_FILTER_AAAA;

				} else if (authoritative ||
					   !RECURSIONOK(client) ||
					   (result != DNS_R_DELEGATION &&
					    result != ISC_R_NOTFOUND)) {
					client->attributes &=
						    ~NS_CLIENTATTR_FILTER_AAAA;
				} else {
					/*
					 * This is an ugly kludge to recurse
					 * for the A and discard the result.
					 *
					 * Continue to add the AAAA now.
					 * We'll make a note to not render it
					 * if the recursion for the A succeeds.
					 */
					result = query_recurse(client,
							dns_rdatatype_a,
							client->query.qname,
							NULL, NULL, resuming);
					if (result == ISC_R_SUCCESS) {
					    client->attributes |=
						    NS_CLIENTATTR_FILTER_AAAA_RC;
					    client->query.attributes |=
							NS_QUERYATTR_RECURSING;
					}
				}

			} else if (qtype == dns_rdatatype_a &&
				   (client->attributes &
					    NS_CLIENTATTR_FILTER_AAAA_RC) != 0) {
				client->attributes &=
					    ~NS_CLIENTATTR_FILTER_AAAA_RC;
				client->attributes |=
					    NS_CLIENTATTR_FILTER_AAAA;
				dns_rdataset_disassociate(rdataset);
				if (sigrdataset != NULL &&
				    dns_rdataset_isassociated(sigrdataset))
					dns_rdataset_disassociate(sigrdataset);
				goto cleanup;
			}
		}
#endif
		/*
		 * Check to see if the AAAA RRset has non-excluded addresses
		 * in it.  If not look for a A RRset.
		 */
		INSIST(client->query.dns64_aaaaok == NULL);

		if (qtype == dns_rdatatype_aaaa && !dns64_exclude &&
		    !ISC_LIST_EMPTY(client->view->dns64) &&
		    client->message->rdclass == dns_rdataclass_in &&
		    !dns64_aaaaok(client, rdataset, sigrdataset)) {
			/*
			 * Look to see if there are A records for this
			 * name.
			 */
			client->query.dns64_aaaa = rdataset;
			client->query.dns64_sigaaaa = sigrdataset;
			client->query.dns64_ttl = rdataset->ttl;
			query_releasename(client, &fname);
			dns_db_detachnode(db, &node);
			rdataset = NULL;
			sigrdataset = NULL;
			type = qtype = dns_rdatatype_a;
			rpz_st = client->query.rpz_st;
			if (rpz_st != NULL) {
				/*
				 * Arrange for RPZ rewriting of any A records.
				 */
				if ((rpz_st->state & DNS_RPZ_REWRITTEN) != 0)
					is_zone = rpz_st->q.is_zone;
				rpz_st_clear(client);
			}
			dns64_exclude = dns64 = ISC_TRUE;
			goto db_find;
		}

		if (sigrdataset != NULL)
			sigrdatasetp = &sigrdataset;
		else
			sigrdatasetp = NULL;
		if (NOQNAME(rdataset) && WANTDNSSEC(client))
			noqname = rdataset;
		else
			noqname = NULL;
		/*
		 * BIND 8 priming queries need the additional section.
		 */
		if (is_zone && qtype == dns_rdatatype_ns &&
		    dns_name_equal(client->query.qname, dns_rootname))
			client->query.attributes &= ~NS_QUERYATTR_NOADDITIONAL;

		/*
		 * Return the time to expire for slave zones.
		 */
		if (zone != NULL) {
			dns_zone_t *raw = NULL, *mayberaw;

			if (is_zone)
				dns_zone_getraw(zone, &raw);
			mayberaw = (raw != NULL) ? raw : zone;

			if (is_zone && qtype == dns_rdatatype_soa &&
			    ((client->attributes &
			      NS_CLIENTATTR_WANTEXPIRE) != 0) &&
			    client->query.restarts == 0 &&
			    dns_zone_gettype(mayberaw) == dns_zone_slave)
			{
				isc_time_t expiretime;
				isc_uint32_t secs;
				dns_zone_getexpiretime(zone, &expiretime);
				secs = isc_time_seconds(&expiretime);
				if (secs >= client->now &&
				    result == ISC_R_SUCCESS) {
					client->attributes |=
						NS_CLIENTATTR_HAVEEXPIRE;
					client->expire = secs - client->now;
				}
			}
			if (raw != NULL)
				dns_zone_detach(&raw);
		}

		if (dns64) {
			qtype = type = dns_rdatatype_aaaa;
			result = query_dns64(client, &fname, rdataset,
					     sigrdataset, dbuf,
					     DNS_SECTION_ANSWER);
			dns_rdataset_disassociate(rdataset);
			dns_message_puttemprdataset(client->message, &rdataset);
			if (result == ISC_R_NOMORE) {
#ifndef dns64_bis_return_excluded_addresses
				if (dns64_exclude) {
					if (!is_zone)
						goto cleanup;
					/*
					 * Add a fake SOA record.
					 */
					(void)query_addsoa(client, db, version,
							   600, ISC_FALSE,
							DNS_SECTION_AUTHORITY);
					goto cleanup;
				}
#endif
				if (is_zone)
					goto iszone_nxrrset;
				else
					goto ncache_nxrrset;
			} else if (result != ISC_R_SUCCESS) {
				eresult = result;
				goto cleanup;
			}
		} else if (client->query.dns64_aaaaok != NULL) {
			query_filter64(client, &fname, rdataset, dbuf,
				       DNS_SECTION_ANSWER);
			query_putrdataset(client, &rdataset);
		} else {
			if (!is_zone && RECURSIONOK(client))
				query_prefetch(client, fname, rdataset);
			query_addrrset(client, &fname, &rdataset,
				       sigrdatasetp, dbuf, DNS_SECTION_ANSWER);
		}

		if (noqname != NULL)
			query_addnoqnameproof(client, noqname);
		/*
		 * We shouldn't ever fail to add 'rdataset'
		 * because it's already in the answer.
		 */
		INSIST(rdataset == NULL);
	}

 addauth:
	CTRACE(ISC_LOG_DEBUG(3), "query_find: addauth");
	/*
	 * Add NS records to the authority section (if we haven't already
	 * added them to the answer section).
	 */
	if (!want_restart && !NOAUTHORITY(client)) {
		if (is_zone) {
			if (!((qtype == dns_rdatatype_ns ||
			       qtype == dns_rdatatype_any) &&
			      dns_name_equal(client->query.qname,
					     dns_db_origin(db))))
				(void)query_addns(client, db, version);
		} else if (qtype != dns_rdatatype_ns) {
			if (fname != NULL)
				query_releasename(client, &fname);
			query_addbestns(client);
		}
	}

	/*
	 * Add NSEC records to the authority section if they're needed for
	 * DNSSEC wildcard proofs.
	 */
	if (need_wildcardproof && dns_db_issecure(db))
		query_addwildcardproof(client, db, version,
				       dns_fixedname_name(&wildcardname),
				       ISC_TRUE, ISC_FALSE);
 cleanup:
	CTRACE(ISC_LOG_DEBUG(3), "query_find: cleanup");
	/*
	 * General cleanup.
	 */
	rpz_st = client->query.rpz_st;
	if (rpz_st != NULL && (rpz_st->state & DNS_RPZ_RECURSING) == 0) {
		rpz_match_clear(rpz_st);
		rpz_st->state &= ~DNS_RPZ_DONE_QNAME;
	}
	if (rdataset != NULL)
		query_putrdataset(client, &rdataset);
	if (sigrdataset != NULL)
		query_putrdataset(client, &sigrdataset);
	if (fname != NULL)
		query_releasename(client, &fname);
	if (node != NULL)
		dns_db_detachnode(db, &node);
	if (db != NULL)
		dns_db_detach(&db);
	if (zone != NULL)
		dns_zone_detach(&zone);
	if (zdb != NULL) {
		query_putrdataset(client, &zrdataset);
		if (zsigrdataset != NULL)
			query_putrdataset(client, &zsigrdataset);
		if (zfname != NULL)
			query_releasename(client, &zfname);
		dns_db_detach(&zdb);
	}
	if (event != NULL)
		isc_event_free(ISC_EVENT_PTR(&event));

	/*
	 * AA bit.
	 */
	if (client->query.restarts == 0 && !authoritative) {
		/*
		 * We're not authoritative, so we must ensure the AA bit
		 * isn't set.
		 */
		client->message->flags &= ~DNS_MESSAGEFLAG_AA;
	}

	/*
	 * Restart the query?
	 */
	if (want_restart && client->query.restarts < MAX_RESTARTS) {
		client->query.restarts++;
		goto restart;
	}

	if (eresult != ISC_R_SUCCESS &&
	    (!PARTIALANSWER(client) || WANTRECURSION(client)
	     || eresult == DNS_R_DROP)) {
		if (eresult == DNS_R_DUPLICATE || eresult == DNS_R_DROP) {
			/*
			 * This was a duplicate query that we are
			 * recursing on or the result of rate limiting.
			 * Don't send a response now for a duplicate query,
			 * because the original will still cause a response.
			 */
			query_next(client, eresult);
		} else {
			/*
			 * If we don't have any answer to give the client,
			 * or if the client requested recursion and thus wanted
			 * the complete answer, send an error response.
			 */
			INSIST(line >= 0);
			query_error(client, eresult, line);
		}
		ns_client_detach(&client);
	} else if (!RECURSING(client)) {
		/*
		 * We are done.  Set up sortlist data for the message
		 * rendering code, make a final tweak to the AA bit if the
		 * auth-nxdomain config option says so, then render and
		 * send the response.
		 */
		setup_query_sortlist(client);

		/*
		 * If this is a referral and the answer to the question
		 * is in the glue sort it to the start of the additional
		 * section.
		 */
		if (ISC_LIST_EMPTY(client->message->sections[DNS_SECTION_ANSWER]) &&
		    client->message->rcode == dns_rcode_noerror &&
		    (qtype == dns_rdatatype_a || qtype == dns_rdatatype_aaaa))
			answer_in_glue(client, qtype);

		if (client->message->rcode == dns_rcode_nxdomain &&
		    client->view->auth_nxdomain == ISC_TRUE)
			client->message->flags |= DNS_MESSAGEFLAG_AA;

		/*
		 * If the response is somehow unexpected for the client and this
		 * is a result of recursion, return an error to the caller
		 * to indicate it may need to be logged.
		 */
		if (resuming &&
		    (ISC_LIST_EMPTY(client->message->sections[DNS_SECTION_ANSWER]) ||
		     client->message->rcode != dns_rcode_noerror))
			eresult = ISC_R_FAILURE;

		query_send(client);
		ns_client_detach(&client);
	}
	CTRACE(ISC_LOG_DEBUG(3), "query_find: done");

	return (eresult);
}

static inline void
log_query(ns_client_t *client, unsigned int flags, unsigned int extflags) {
	char namebuf[DNS_NAME_FORMATSIZE];
	char typename[DNS_RDATATYPE_FORMATSIZE];
	char classname[DNS_RDATACLASS_FORMATSIZE];
	char onbuf[ISC_NETADDR_FORMATSIZE];
	dns_rdataset_t *rdataset;
	int level = ISC_LOG_INFO;

	if (! isc_log_wouldlog(ns_g_lctx, level))
		return;

	rdataset = ISC_LIST_HEAD(client->query.qname->list);
	INSIST(rdataset != NULL);
	dns_name_format(client->query.qname, namebuf, sizeof(namebuf));
	dns_rdataclass_format(rdataset->rdclass, classname, sizeof(classname));
	dns_rdatatype_format(rdataset->type, typename, sizeof(typename));
	isc_netaddr_format(&client->destaddr, onbuf, sizeof(onbuf));

	ns_client_log(client, NS_LOGCATEGORY_QUERIES, NS_LOGMODULE_QUERY,
		      level, "query: %s %s %s %s%s%s%s%s%s (%s)", namebuf,
		      classname, typename, WANTRECURSION(client) ? "+" : "-",
		      (client->signer != NULL) ? "S" : "",
		      (client->ednsversion >= 0) ? "E" : "",
		      ((client->attributes & NS_CLIENTATTR_TCP) != 0) ?
				 "T" : "",
		      ((extflags & DNS_MESSAGEEXTFLAG_DO) != 0) ? "D" : "",
		      ((flags & DNS_MESSAGEFLAG_CD) != 0) ? "C" : "",
		      onbuf);
}

static inline void
log_queryerror(ns_client_t *client, isc_result_t result, int line, int level) {
	char namebuf[DNS_NAME_FORMATSIZE];
	char typename[DNS_RDATATYPE_FORMATSIZE];
	char classname[DNS_RDATACLASS_FORMATSIZE];
	const char *namep, *typep, *classp, *sep1, *sep2;
	dns_rdataset_t *rdataset;

	if (!isc_log_wouldlog(ns_g_lctx, level))
		return;

	namep = typep = classp = sep1 = sep2 = "";

	/*
	 * Query errors can happen for various reasons.  In some cases we cannot
	 * even assume the query contains a valid question section, so we should
	 * expect exceptional cases.
	 */
	if (client->query.origqname != NULL) {
		dns_name_format(client->query.origqname, namebuf,
				sizeof(namebuf));
		namep = namebuf;
		sep1 = " for ";

		rdataset = ISC_LIST_HEAD(client->query.origqname->list);
		if (rdataset != NULL) {
			dns_rdataclass_format(rdataset->rdclass, classname,
					      sizeof(classname));
			classp = classname;
			dns_rdatatype_format(rdataset->type, typename,
					     sizeof(typename));
			typep = typename;
			sep2 = "/";
		}
	}

	ns_client_log(client, NS_LOGCATEGORY_QUERY_ERRORS, NS_LOGMODULE_QUERY,
		      level, "query failed (%s)%s%s%s%s%s%s at %s:%d",
		      isc_result_totext(result), sep1, namep, sep2,
		      classp, sep2, typep, __FILE__, line);
}

void
ns_query_start(ns_client_t *client) {
	isc_result_t result;
	dns_message_t *message = client->message;
	dns_rdataset_t *rdataset;
	ns_client_t *qclient;
	dns_rdatatype_t qtype;
	unsigned int saved_extflags = client->extflags;
	unsigned int saved_flags = client->message->flags;

	CTRACE(ISC_LOG_DEBUG(3), "ns_query_start");

	/*
	 * Test only.
	 */
	if (ns_g_clienttest && !TCP(client)) {
		result = ns_client_replace(client);
		if (result == ISC_R_SHUTTINGDOWN) {
			ns_client_next(client, result);
			return;
		} else if (result != ISC_R_SUCCESS) {
			query_error(client, result, __LINE__);
			return;
		}
	}

	/*
	 * Ensure that appropriate cleanups occur.
	 */
	client->next = query_next_callback;

	/*
	 * Behave as if we don't support DNSSEC if not enabled.
	 */
	if (!client->view->enablednssec) {
		message->flags &= ~DNS_MESSAGEFLAG_CD;
		client->extflags &= ~DNS_MESSAGEEXTFLAG_DO;
	}

	if ((message->flags & DNS_MESSAGEFLAG_RD) != 0)
		client->query.attributes |= NS_QUERYATTR_WANTRECURSION;

	if ((client->extflags & DNS_MESSAGEEXTFLAG_DO) != 0)
		client->attributes |= NS_CLIENTATTR_WANTDNSSEC;

	if (client->view->minimalresponses)
		client->query.attributes |= (NS_QUERYATTR_NOAUTHORITY |
					     NS_QUERYATTR_NOADDITIONAL);

	if ((client->view->cachedb == NULL)
	    || (!client->view->additionalfromcache)) {
		/*
		 * We don't have a cache.  Turn off cache support and
		 * recursion.
		 */
		client->query.attributes &=
			~(NS_QUERYATTR_RECURSIONOK|NS_QUERYATTR_CACHEOK);
	} else if ((client->attributes & NS_CLIENTATTR_RA) == 0 ||
		   (message->flags & DNS_MESSAGEFLAG_RD) == 0) {
		/*
		 * If the client isn't allowed to recurse (due to
		 * "recursion no", the allow-recursion ACL, or the
		 * lack of a resolver in this view), or if it
		 * doesn't want recursion, turn recursion off.
		 */
		client->query.attributes &= ~NS_QUERYATTR_RECURSIONOK;
	}

	/*
	 * Check for multiple question queries, since edns1 is dead.
	 */
	if (message->counts[DNS_SECTION_QUESTION] > 1) {
		query_error(client, DNS_R_FORMERR, __LINE__);
		return;
	}

	/*
	 * Get the question name.
	 */
	result = dns_message_firstname(message, DNS_SECTION_QUESTION);
	if (result != ISC_R_SUCCESS) {
		query_error(client, result, __LINE__);
		return;
	}
	dns_message_currentname(message, DNS_SECTION_QUESTION,
				&client->query.qname);
	client->query.origqname = client->query.qname;
	result = dns_message_nextname(message, DNS_SECTION_QUESTION);
	if (result != ISC_R_NOMORE) {
		if (result == ISC_R_SUCCESS) {
			/*
			 * There's more than one QNAME in the question
			 * section.
			 */
			query_error(client, DNS_R_FORMERR, __LINE__);
		} else
			query_error(client, result, __LINE__);
		return;
	}

	if (ns_g_server->log_queries)
		log_query(client, saved_flags, saved_extflags);

	/*
	 * Check for meta-queries like IXFR and AXFR.
	 */
	rdataset = ISC_LIST_HEAD(client->query.qname->list);
	INSIST(rdataset != NULL);
	client->query.qtype = qtype = rdataset->type;
	dns_rdatatypestats_increment(ns_g_server->rcvquerystats, qtype);

	if (dns_rdatatype_ismeta(qtype)) {
		switch (qtype) {
		case dns_rdatatype_any:
			break; /* Let query_find handle it. */
		case dns_rdatatype_ixfr:
		case dns_rdatatype_axfr:
			ns_xfr_start(client, rdataset->type);
			return;
		case dns_rdatatype_maila:
		case dns_rdatatype_mailb:
			query_error(client, DNS_R_NOTIMP, __LINE__);
			return;
		case dns_rdatatype_tkey:
			result = dns_tkey_processquery(client->message,
						ns_g_server->tkeyctx,
						client->view->dynamickeys);
			if (result == ISC_R_SUCCESS)
				query_send(client);
			else
				query_error(client, result, __LINE__);
			return;
		default: /* TSIG, etc. */
			query_error(client, DNS_R_FORMERR, __LINE__);
			return;
		}
	}

	/*
	 * Turn on minimal response for DNSKEY and DS queries.
	 */
	if (qtype == dns_rdatatype_dnskey || qtype == dns_rdatatype_ds)
		client->query.attributes |= (NS_QUERYATTR_NOAUTHORITY |
					     NS_QUERYATTR_NOADDITIONAL);

	/*
	 * Turn on minimal responses for EDNS/UDP bufsize 512 queries.
	 */
	if (client->ednsversion >= 0 && client->udpsize <= 512U &&
	    (client->attributes & NS_CLIENTATTR_TCP) == 0)
		client->query.attributes |= (NS_QUERYATTR_NOAUTHORITY |
					     NS_QUERYATTR_NOADDITIONAL);

	/*
	 * If the client has requested that DNSSEC checking be disabled,
	 * allow lookups to return pending data and instruct the resolver
	 * to return data before validation has completed.
	 *
	 * We don't need to set DNS_DBFIND_PENDINGOK when validation is
	 * disabled as there will be no pending data.
	 */
	if (message->flags & DNS_MESSAGEFLAG_CD ||
	    qtype == dns_rdatatype_rrsig)
	{
		client->query.dboptions |= DNS_DBFIND_PENDINGOK;
		client->query.fetchoptions |= DNS_FETCHOPT_NOVALIDATE;
	} else if (!client->view->enablevalidation)
		client->query.fetchoptions |= DNS_FETCHOPT_NOVALIDATE;

	/*
	 * Allow glue NS records to be added to the authority section
	 * if the answer is secure.
	 */
	if (message->flags & DNS_MESSAGEFLAG_CD)
		client->query.attributes &= ~NS_QUERYATTR_SECURE;

	/*
	 * Set NS_CLIENTATTR_WANTDNSSEC if the client has set AD in the query.
	 * This allows AD to be returned on queries without DO set.
	 */
	if ((message->flags & DNS_MESSAGEFLAG_AD) != 0)
		client->attributes |= NS_CLIENTATTR_WANTAD;

	/*
	 * This is an ordinary query.
	 */
	result = dns_message_reply(message, ISC_TRUE);
	if (result != ISC_R_SUCCESS) {
		query_next(client, result);
		return;
	}

	/*
	 * Assume authoritative response until it is known to be
	 * otherwise.
	 *
	 * If "-T noaa" has been set on the command line don't set
	 * AA on authoritative answers.
	 */
	if (!ns_g_noaa)
		message->flags |= DNS_MESSAGEFLAG_AA;

	/*
	 * Set AD.  We must clear it if we add non-validated data to a
	 * response.
	 */
	if (WANTDNSSEC(client) || WANTAD(client))
		message->flags |= DNS_MESSAGEFLAG_AD;

	qclient = NULL;
	ns_client_attach(client, &qclient);
	(void)query_find(qclient, NULL, qtype);
}
