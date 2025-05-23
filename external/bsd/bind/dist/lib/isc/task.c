/*	$NetBSD$	*/

/*
 * Copyright (C) 2004-2015  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1998-2003  Internet Software Consortium.
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

/*! \file
 * \author Principal Author: Bob Halley
 */

/*
 * XXXRTH  Need to document the states a task can be in, and the rules
 * for changing states.
 */

#include <config.h>

#include <isc/app.h>
#include <isc/condition.h>
#include <isc/event.h>
#include <isc/json.h>
#include <isc/magic.h>
#include <isc/mem.h>
#include <isc/msgs.h>
#include <isc/once.h>
#include <isc/platform.h>
#include <isc/print.h>
#include <isc/string.h>
#include <isc/task.h>
#include <isc/thread.h>
#include <isc/util.h>
#include <isc/xml.h>

#ifdef OPENSSL_LEAKS
#include <openssl/err.h>
#endif

/*%
 * For BIND9 internal applications:
 * when built with threads we use multiple worker threads shared by the whole
 * application.
 * when built without threads we share a single global task manager and use
 * an integrated event loop for socket, timer, and other generic task events.
 * For generic library:
 * we don't use either of them: an application can have multiple task managers
 * whether or not it's threaded, and if the application is threaded each thread
 * is expected to have a separate manager; no "worker threads" are shared by
 * the application threads.
 */
#ifdef ISC_PLATFORM_USETHREADS
#define USE_WORKER_THREADS
#else
#define USE_SHARED_MANAGER
#endif	/* ISC_PLATFORM_USETHREADS */

#include "task_p.h"

#ifdef ISC_TASK_TRACE
#define XTRACE(m)		fprintf(stderr, "task %p thread %lu: %s\n", \
				       task, isc_thread_self(), (m))
#define XTTRACE(t, m)		fprintf(stderr, "task %p thread %lu: %s\n", \
				       (t), isc_thread_self(), (m))
#define XTHREADTRACE(m)		fprintf(stderr, "thread %lu: %s\n", \
				       isc_thread_self(), (m))
#else
#define XTRACE(m)
#define XTTRACE(t, m)
#define XTHREADTRACE(m)
#endif

/***
 *** Types.
 ***/

typedef enum {
	task_state_idle, task_state_ready, task_state_running,
	task_state_done
} task_state_t;

#if defined(HAVE_LIBXML2) || defined(HAVE_JSON)
static const char *statenames[] = {
	"idle", "ready", "running", "done",
};
#endif

#define TASK_MAGIC			ISC_MAGIC('T', 'A', 'S', 'K')
#define VALID_TASK(t)			ISC_MAGIC_VALID(t, TASK_MAGIC)

typedef struct isc__task isc__task_t;
typedef struct isc__taskmgr isc__taskmgr_t;

struct isc__task {
	/* Not locked. */
	isc_task_t			common;
	isc__taskmgr_t *		manager;
	isc_mutex_t			lock;
	/* Locked by task lock. */
	task_state_t			state;
	unsigned int			references;
	isc_eventlist_t			events;
	isc_eventlist_t			on_shutdown;
	unsigned int			nevents;
	unsigned int			quantum;
	unsigned int			flags;
	isc_stdtime_t			now;
	char				name[16];
	void *				tag;
	/* Locked by task manager lock. */
	LINK(isc__task_t)		link;
	LINK(isc__task_t)		ready_link;
	LINK(isc__task_t)		ready_priority_link;
};

#define TASK_F_SHUTTINGDOWN		0x01
#define TASK_F_PRIVILEGED		0x02

#define TASK_SHUTTINGDOWN(t)		(((t)->flags & TASK_F_SHUTTINGDOWN) \
					 != 0)

#define TASK_MANAGER_MAGIC		ISC_MAGIC('T', 'S', 'K', 'M')
#define VALID_MANAGER(m)		ISC_MAGIC_VALID(m, TASK_MANAGER_MAGIC)

typedef ISC_LIST(isc__task_t)	isc__tasklist_t;

struct isc__taskmgr {
	/* Not locked. */
	isc_taskmgr_t			common;
	isc_mem_t *			mctx;
	isc_mutex_t			lock;
#ifdef ISC_PLATFORM_USETHREADS
	unsigned int			workers;
	isc_thread_t *			threads;
#endif /* ISC_PLATFORM_USETHREADS */
	/* Locked by task manager lock. */
	unsigned int			default_quantum;
	LIST(isc__task_t)		tasks;
	isc__tasklist_t			ready_tasks;
	isc__tasklist_t			ready_priority_tasks;
	isc_taskmgrmode_t		mode;
#ifdef ISC_PLATFORM_USETHREADS
	isc_condition_t			work_available;
	isc_condition_t			exclusive_granted;
	isc_condition_t			paused;
#endif /* ISC_PLATFORM_USETHREADS */
	unsigned int			tasks_running;
	unsigned int			tasks_ready;
	isc_boolean_t			pause_requested;
	isc_boolean_t			exclusive_requested;
	isc_boolean_t			exiting;

	/*
	 * Multiple threads can read/write 'excl' at the same time, so we need
	 * to protect the access.  We can't use 'lock' since isc_task_detach()
	 * will try to acquire it.
	 */
	isc_mutex_t			excl_lock;
	isc__task_t			*excl;
#ifdef USE_SHARED_MANAGER
	unsigned int			refs;
#endif /* ISC_PLATFORM_USETHREADS */
};

#define DEFAULT_TASKMGR_QUANTUM		10
#define DEFAULT_DEFAULT_QUANTUM		5
#define FINISHED(m)			((m)->exiting && EMPTY((m)->tasks))

#ifdef USE_SHARED_MANAGER
static isc__taskmgr_t *taskmgr = NULL;
#endif /* USE_SHARED_MANAGER */

/*%
 * The following are intended for internal use (indicated by "isc__"
 * prefix) but are not declared as static, allowing direct access from
 * unit tests etc.
 */

isc_result_t
isc__task_create(isc_taskmgr_t *manager0, unsigned int quantum,
		 isc_task_t **taskp);
void
isc__task_attach(isc_task_t *source0, isc_task_t **targetp);
void
isc__task_detach(isc_task_t **taskp);
void
isc__task_send(isc_task_t *task0, isc_event_t **eventp);
void
isc__task_sendanddetach(isc_task_t **taskp, isc_event_t **eventp);
unsigned int
isc__task_purgerange(isc_task_t *task0, void *sender, isc_eventtype_t first,
		     isc_eventtype_t last, void *tag);
unsigned int
isc__task_purge(isc_task_t *task, void *sender, isc_eventtype_t type,
		void *tag);
isc_boolean_t
isc_task_purgeevent(isc_task_t *task0, isc_event_t *event);
unsigned int
isc__task_unsendrange(isc_task_t *task, void *sender, isc_eventtype_t first,
		      isc_eventtype_t last, void *tag,
		      isc_eventlist_t *events);
unsigned int
isc__task_unsend(isc_task_t *task, void *sender, isc_eventtype_t type,
		 void *tag, isc_eventlist_t *events);
isc_result_t
isc__task_onshutdown(isc_task_t *task0, isc_taskaction_t action,
		     void *arg);
void
isc__task_shutdown(isc_task_t *task0);
void
isc__task_destroy(isc_task_t **taskp);
void
isc__task_setname(isc_task_t *task0, const char *name, void *tag);
const char *
isc__task_getname(isc_task_t *task0);
void *
isc__task_gettag(isc_task_t *task0);
void
isc__task_getcurrenttime(isc_task_t *task0, isc_stdtime_t *t);
isc_result_t
isc__taskmgr_create(isc_mem_t *mctx, unsigned int workers,
		    unsigned int default_quantum, isc_taskmgr_t **managerp);
void
isc__taskmgr_destroy(isc_taskmgr_t **managerp);
void
isc_taskmgr_setexcltask(isc_taskmgr_t *mgr0, isc_task_t *task0);
isc_result_t
isc_taskmgr_excltask(isc_taskmgr_t *mgr0, isc_task_t **taskp);
isc_result_t
isc__task_beginexclusive(isc_task_t *task);
void
isc__task_endexclusive(isc_task_t *task0);
void
isc__task_setprivilege(isc_task_t *task0, isc_boolean_t priv);
isc_boolean_t
isc__task_privilege(isc_task_t *task0);
void
isc__taskmgr_setmode(isc_taskmgr_t *manager0, isc_taskmgrmode_t mode);
isc_taskmgrmode_t
isc__taskmgr_mode(isc_taskmgr_t *manager0);

static inline isc_boolean_t
empty_readyq(isc__taskmgr_t *manager);

static inline isc__task_t *
pop_readyq(isc__taskmgr_t *manager);

static inline void
push_readyq(isc__taskmgr_t *manager, isc__task_t *task);

static struct isc__taskmethods {
	isc_taskmethods_t methods;

	/*%
	 * The following are defined just for avoiding unused static functions.
	 */
	void *purgeevent, *unsendrange, *getname, *gettag, *getcurrenttime;
} taskmethods = {
	{
		isc__task_attach,
		isc__task_detach,
		isc__task_destroy,
		isc__task_send,
		isc__task_sendanddetach,
		isc__task_unsend,
		isc__task_onshutdown,
		isc__task_shutdown,
		isc__task_setname,
		isc__task_purge,
		isc__task_purgerange,
		isc__task_beginexclusive,
		isc__task_endexclusive,
		isc__task_setprivilege,
		isc__task_privilege
	},
	(void *)isc_task_purgeevent,
	(void *)isc__task_unsendrange,
	(void *)isc__task_getname,
	(void *)isc__task_gettag,
	(void *)isc__task_getcurrenttime
};

static isc_taskmgrmethods_t taskmgrmethods = {
	isc__taskmgr_destroy,
	isc__taskmgr_setmode,
	isc__taskmgr_mode,
	isc__task_create,
	isc_taskmgr_setexcltask,
	isc_taskmgr_excltask
};

/***
 *** Tasks.
 ***/

static void
task_finished(isc__task_t *task) {
	isc__taskmgr_t *manager = task->manager;

	REQUIRE(EMPTY(task->events));
	REQUIRE(task->nevents == 0);
	REQUIRE(EMPTY(task->on_shutdown));
	REQUIRE(task->references == 0);
	REQUIRE(task->state == task_state_done);

	XTRACE("task_finished");

	LOCK(&manager->lock);
	UNLINK(manager->tasks, task, link);
#ifdef USE_WORKER_THREADS
	if (FINISHED(manager)) {
		/*
		 * All tasks have completed and the
		 * task manager is exiting.  Wake up
		 * any idle worker threads so they
		 * can exit.
		 */
		BROADCAST(&manager->work_available);
	}
#endif /* USE_WORKER_THREADS */
	UNLOCK(&manager->lock);

	DESTROYLOCK(&task->lock);
	task->common.impmagic = 0;
	task->common.magic = 0;
	isc_mem_put(manager->mctx, task, sizeof(*task));
}

isc_result_t
isc__task_create(isc_taskmgr_t *manager0, unsigned int quantum,
		 isc_task_t **taskp)
{
	isc__taskmgr_t *manager = (isc__taskmgr_t *)manager0;
	isc__task_t *task;
	isc_boolean_t exiting;
	isc_result_t result;

	REQUIRE(VALID_MANAGER(manager));
	REQUIRE(taskp != NULL && *taskp == NULL);

	task = isc_mem_get(manager->mctx, sizeof(*task));
	if (task == NULL)
		return (ISC_R_NOMEMORY);
	XTRACE("isc_task_create");
	task->manager = manager;
	result = isc_mutex_init(&task->lock);
	if (result != ISC_R_SUCCESS) {
		isc_mem_put(manager->mctx, task, sizeof(*task));
		return (result);
	}
	task->state = task_state_idle;
	task->references = 1;
	INIT_LIST(task->events);
	INIT_LIST(task->on_shutdown);
	task->nevents = 0;
	task->quantum = quantum;
	task->flags = 0;
	task->now = 0;
	memset(task->name, 0, sizeof(task->name));
	task->tag = NULL;
	INIT_LINK(task, link);
	INIT_LINK(task, ready_link);
	INIT_LINK(task, ready_priority_link);

	exiting = ISC_FALSE;
	LOCK(&manager->lock);
	if (!manager->exiting) {
		if (task->quantum == 0)
			task->quantum = manager->default_quantum;
		APPEND(manager->tasks, task, link);
	} else
		exiting = ISC_TRUE;
	UNLOCK(&manager->lock);

	if (exiting) {
		DESTROYLOCK(&task->lock);
		isc_mem_put(manager->mctx, task, sizeof(*task));
		return (ISC_R_SHUTTINGDOWN);
	}

	task->common.methods = (isc_taskmethods_t *)&taskmethods;
	task->common.magic = ISCAPI_TASK_MAGIC;
	task->common.impmagic = TASK_MAGIC;
	*taskp = (isc_task_t *)task;

	return (ISC_R_SUCCESS);
}

void
isc__task_attach(isc_task_t *source0, isc_task_t **targetp) {
	isc__task_t *source = (isc__task_t *)source0;

	/*
	 * Attach *targetp to source.
	 */

	REQUIRE(VALID_TASK(source));
	REQUIRE(targetp != NULL && *targetp == NULL);

	XTTRACE(source, "isc_task_attach");

	LOCK(&source->lock);
	source->references++;
	UNLOCK(&source->lock);

	*targetp = (isc_task_t *)source;
}

static inline isc_boolean_t
task_shutdown(isc__task_t *task) {
	isc_boolean_t was_idle = ISC_FALSE;
	isc_event_t *event, *prev;

	/*
	 * Caller must be holding the task's lock.
	 */

	XTRACE("task_shutdown");

	if (! TASK_SHUTTINGDOWN(task)) {
		XTRACE(isc_msgcat_get(isc_msgcat, ISC_MSGSET_GENERAL,
				      ISC_MSG_SHUTTINGDOWN, "shutting down"));
		task->flags |= TASK_F_SHUTTINGDOWN;
		if (task->state == task_state_idle) {
			INSIST(EMPTY(task->events));
			task->state = task_state_ready;
			was_idle = ISC_TRUE;
		}
		INSIST(task->state == task_state_ready ||
		       task->state == task_state_running);

		/*
		 * Note that we post shutdown events LIFO.
		 */
		for (event = TAIL(task->on_shutdown);
		     event != NULL;
		     event = prev) {
			prev = PREV(event, ev_link);
			DEQUEUE(task->on_shutdown, event, ev_link);
			ENQUEUE(task->events, event, ev_link);
			task->nevents++;
		}
	}

	return (was_idle);
}

/*
 * Moves a task onto the appropriate run queue.
 *
 * Caller must NOT hold manager lock.
 */
static inline void
task_ready(isc__task_t *task) {
	isc__taskmgr_t *manager = task->manager;
#ifdef USE_WORKER_THREADS
	isc_boolean_t has_privilege = isc__task_privilege((isc_task_t *) task);
#endif /* USE_WORKER_THREADS */

	REQUIRE(VALID_MANAGER(manager));
	REQUIRE(task->state == task_state_ready);

	XTRACE("task_ready");

	LOCK(&manager->lock);
	push_readyq(manager, task);
#ifdef USE_WORKER_THREADS
	if (manager->mode == isc_taskmgrmode_normal || has_privilege)
		SIGNAL(&manager->work_available);
#endif /* USE_WORKER_THREADS */
	UNLOCK(&manager->lock);
}

static inline isc_boolean_t
task_detach(isc__task_t *task) {

	/*
	 * Caller must be holding the task lock.
	 */

	REQUIRE(task->references > 0);

	XTRACE("detach");

	task->references--;
	if (task->references == 0 && task->state == task_state_idle) {
		INSIST(EMPTY(task->events));
		/*
		 * There are no references to this task, and no
		 * pending events.  We could try to optimize and
		 * either initiate shutdown or clean up the task,
		 * depending on its state, but it's easier to just
		 * make the task ready and allow run() or the event
		 * loop to deal with shutting down and termination.
		 */
		task->state = task_state_ready;
		return (ISC_TRUE);
	}

	return (ISC_FALSE);
}

void
isc__task_detach(isc_task_t **taskp) {
	isc__task_t *task;
	isc_boolean_t was_idle;

	/*
	 * Detach *taskp from its task.
	 */

	REQUIRE(taskp != NULL);
	task = (isc__task_t *)*taskp;
	REQUIRE(VALID_TASK(task));

	XTRACE("isc_task_detach");

	LOCK(&task->lock);
	was_idle = task_detach(task);
	UNLOCK(&task->lock);

	if (was_idle)
		task_ready(task);

	*taskp = NULL;
}

static inline isc_boolean_t
task_send(isc__task_t *task, isc_event_t **eventp) {
	isc_boolean_t was_idle = ISC_FALSE;
	isc_event_t *event;

	/*
	 * Caller must be holding the task lock.
	 */

	REQUIRE(eventp != NULL);
	event = *eventp;
	REQUIRE(event != NULL);
	REQUIRE(event->ev_type > 0);
	REQUIRE(task->state != task_state_done);

	XTRACE("task_send");

	if (task->state == task_state_idle) {
		was_idle = ISC_TRUE;
		INSIST(EMPTY(task->events));
		task->state = task_state_ready;
	}
	INSIST(task->state == task_state_ready ||
	       task->state == task_state_running);
	ENQUEUE(task->events, event, ev_link);
	task->nevents++;
	*eventp = NULL;

	return (was_idle);
}

void
isc__task_send(isc_task_t *task0, isc_event_t **eventp) {
	isc__task_t *task = (isc__task_t *)task0;
	isc_boolean_t was_idle;

	/*
	 * Send '*event' to 'task'.
	 */

	REQUIRE(VALID_TASK(task));

	XTRACE("isc_task_send");

	/*
	 * We're trying hard to hold locks for as short a time as possible.
	 * We're also trying to hold as few locks as possible.  This is why
	 * some processing is deferred until after the lock is released.
	 */
	LOCK(&task->lock);
	was_idle = task_send(task, eventp);
	UNLOCK(&task->lock);

	if (was_idle) {
		/*
		 * We need to add this task to the ready queue.
		 *
		 * We've waited until now to do it because making a task
		 * ready requires locking the manager.  If we tried to do
		 * this while holding the task lock, we could deadlock.
		 *
		 * We've changed the state to ready, so no one else will
		 * be trying to add this task to the ready queue.  The
		 * only way to leave the ready state is by executing the
		 * task.  It thus doesn't matter if events are added,
		 * removed, or a shutdown is started in the interval
		 * between the time we released the task lock, and the time
		 * we add the task to the ready queue.
		 */
		task_ready(task);
	}
}

void
isc__task_sendanddetach(isc_task_t **taskp, isc_event_t **eventp) {
	isc_boolean_t idle1, idle2;
	isc__task_t *task;

	/*
	 * Send '*event' to '*taskp' and then detach '*taskp' from its
	 * task.
	 */

	REQUIRE(taskp != NULL);
	task = (isc__task_t *)*taskp;
	REQUIRE(VALID_TASK(task));

	XTRACE("isc_task_sendanddetach");

	LOCK(&task->lock);
	idle1 = task_send(task, eventp);
	idle2 = task_detach(task);
	UNLOCK(&task->lock);

	/*
	 * If idle1, then idle2 shouldn't be true as well since we're holding
	 * the task lock, and thus the task cannot switch from ready back to
	 * idle.
	 */
	INSIST(!(idle1 && idle2));

	if (idle1 || idle2)
		task_ready(task);

	*taskp = NULL;
}

#define PURGE_OK(event)	(((event)->ev_attributes & ISC_EVENTATTR_NOPURGE) == 0)

static unsigned int
dequeue_events(isc__task_t *task, void *sender, isc_eventtype_t first,
	       isc_eventtype_t last, void *tag,
	       isc_eventlist_t *events, isc_boolean_t purging)
{
	isc_event_t *event, *next_event;
	unsigned int count = 0;

	REQUIRE(VALID_TASK(task));
	REQUIRE(last >= first);

	XTRACE("dequeue_events");

	/*
	 * Events matching 'sender', whose type is >= first and <= last, and
	 * whose tag is 'tag' will be dequeued.  If 'purging', matching events
	 * which are marked as unpurgable will not be dequeued.
	 *
	 * sender == NULL means "any sender", and tag == NULL means "any tag".
	 */

	LOCK(&task->lock);

	for (event = HEAD(task->events); event != NULL; event = next_event) {
		next_event = NEXT(event, ev_link);
		if (event->ev_type >= first && event->ev_type <= last &&
		    (sender == NULL || event->ev_sender == sender) &&
		    (tag == NULL || event->ev_tag == tag) &&
		    (!purging || PURGE_OK(event))) {
			DEQUEUE(task->events, event, ev_link);
			task->nevents--;
			ENQUEUE(*events, event, ev_link);
			count++;
		}
	}

	UNLOCK(&task->lock);

	return (count);
}

unsigned int
isc__task_purgerange(isc_task_t *task0, void *sender, isc_eventtype_t first,
		     isc_eventtype_t last, void *tag)
{
	isc__task_t *task = (isc__task_t *)task0;
	unsigned int count;
	isc_eventlist_t events;
	isc_event_t *event, *next_event;

	/*
	 * Purge events from a task's event queue.
	 */

	XTRACE("isc_task_purgerange");

	ISC_LIST_INIT(events);

	count = dequeue_events(task, sender, first, last, tag, &events,
			       ISC_TRUE);

	for (event = HEAD(events); event != NULL; event = next_event) {
		next_event = NEXT(event, ev_link);
		isc_event_free(&event);
	}

	/*
	 * Note that purging never changes the state of the task.
	 */

	return (count);
}

unsigned int
isc__task_purge(isc_task_t *task, void *sender, isc_eventtype_t type,
		void *tag)
{
	/*
	 * Purge events from a task's event queue.
	 */

	XTRACE("isc_task_purge");

	return (isc__task_purgerange(task, sender, type, type, tag));
}

isc_boolean_t
isc_task_purgeevent(isc_task_t *task0, isc_event_t *event) {
	isc__task_t *task = (isc__task_t *)task0;
	isc_event_t *curr_event, *next_event;

	/*
	 * Purge 'event' from a task's event queue.
	 *
	 * XXXRTH:  WARNING:  This method may be removed before beta.
	 */

	REQUIRE(VALID_TASK(task));

	/*
	 * If 'event' is on the task's event queue, it will be purged,
	 * unless it is marked as unpurgeable.  'event' does not have to be
	 * on the task's event queue; in fact, it can even be an invalid
	 * pointer.  Purging only occurs if the event is actually on the task's
	 * event queue.
	 *
	 * Purging never changes the state of the task.
	 */

	LOCK(&task->lock);
	for (curr_event = HEAD(task->events);
	     curr_event != NULL;
	     curr_event = next_event) {
		next_event = NEXT(curr_event, ev_link);
		if (curr_event == event && PURGE_OK(event)) {
			DEQUEUE(task->events, curr_event, ev_link);
			task->nevents--;
			break;
		}
	}
	UNLOCK(&task->lock);

	if (curr_event == NULL)
		return (ISC_FALSE);

	isc_event_free(&curr_event);

	return (ISC_TRUE);
}

unsigned int
isc__task_unsendrange(isc_task_t *task, void *sender, isc_eventtype_t first,
		      isc_eventtype_t last, void *tag,
		      isc_eventlist_t *events)
{
	/*
	 * Remove events from a task's event queue.
	 */

	XTRACE("isc_task_unsendrange");

	return (dequeue_events((isc__task_t *)task, sender, first,
			       last, tag, events, ISC_FALSE));
}

unsigned int
isc__task_unsend(isc_task_t *task, void *sender, isc_eventtype_t type,
		 void *tag, isc_eventlist_t *events)
{
	/*
	 * Remove events from a task's event queue.
	 */

	XTRACE("isc_task_unsend");

	return (dequeue_events((isc__task_t *)task, sender, type,
			       type, tag, events, ISC_FALSE));
}

isc_result_t
isc__task_onshutdown(isc_task_t *task0, isc_taskaction_t action,
		     void *arg)
{
	isc__task_t *task = (isc__task_t *)task0;
	isc_boolean_t disallowed = ISC_FALSE;
	isc_result_t result = ISC_R_SUCCESS;
	isc_event_t *event;

	/*
	 * Send a shutdown event with action 'action' and argument 'arg' when
	 * 'task' is shutdown.
	 */

	REQUIRE(VALID_TASK(task));
	REQUIRE(action != NULL);

	event = isc_event_allocate(task->manager->mctx,
				   NULL,
				   ISC_TASKEVENT_SHUTDOWN,
				   action,
				   arg,
				   sizeof(*event));
	if (event == NULL)
		return (ISC_R_NOMEMORY);

	LOCK(&task->lock);
	if (TASK_SHUTTINGDOWN(task)) {
		disallowed = ISC_TRUE;
		result = ISC_R_SHUTTINGDOWN;
	} else
		ENQUEUE(task->on_shutdown, event, ev_link);
	UNLOCK(&task->lock);

	if (disallowed)
		isc_mem_put(task->manager->mctx, event, sizeof(*event));

	return (result);
}

void
isc__task_shutdown(isc_task_t *task0) {
	isc__task_t *task = (isc__task_t *)task0;
	isc_boolean_t was_idle;

	/*
	 * Shutdown 'task'.
	 */

	REQUIRE(VALID_TASK(task));

	LOCK(&task->lock);
	was_idle = task_shutdown(task);
	UNLOCK(&task->lock);

	if (was_idle)
		task_ready(task);
}

void
isc__task_destroy(isc_task_t **taskp) {

	/*
	 * Destroy '*taskp'.
	 */

	REQUIRE(taskp != NULL);

	isc_task_shutdown(*taskp);
	isc_task_detach(taskp);
}

void
isc__task_setname(isc_task_t *task0, const char *name, void *tag) {
	isc__task_t *task = (isc__task_t *)task0;

	/*
	 * Name 'task'.
	 */

	REQUIRE(VALID_TASK(task));

	LOCK(&task->lock);
	memset(task->name, 0, sizeof(task->name));
	strncpy(task->name, name, sizeof(task->name) - 1);
	task->tag = tag;
	UNLOCK(&task->lock);
}

const char *
isc__task_getname(isc_task_t *task0) {
	isc__task_t *task = (isc__task_t *)task0;

	REQUIRE(VALID_TASK(task));

	return (task->name);
}

void *
isc__task_gettag(isc_task_t *task0) {
	isc__task_t *task = (isc__task_t *)task0;

	REQUIRE(VALID_TASK(task));

	return (task->tag);
}

void
isc__task_getcurrenttime(isc_task_t *task0, isc_stdtime_t *t) {
	isc__task_t *task = (isc__task_t *)task0;

	REQUIRE(VALID_TASK(task));
	REQUIRE(t != NULL);

	LOCK(&task->lock);
	*t = task->now;
	UNLOCK(&task->lock);
}

/***
 *** Task Manager.
 ***/

/*
 * Return ISC_TRUE if the current ready list for the manager, which is
 * either ready_tasks or the ready_priority_tasks, depending on whether
 * the manager is currently in normal or privileged execution mode.
 *
 * Caller must hold the task manager lock.
 */
static inline isc_boolean_t
empty_readyq(isc__taskmgr_t *manager) {
	isc__tasklist_t queue;

	if (manager->mode == isc_taskmgrmode_normal)
		queue = manager->ready_tasks;
	else
		queue = manager->ready_priority_tasks;

	return (ISC_TF(EMPTY(queue)));
}

/*
 * Dequeue and return a pointer to the first task on the current ready
 * list for the manager.
 * If the task is privileged, dequeue it from the other ready list
 * as well.
 *
 * Caller must hold the task manager lock.
 */
static inline isc__task_t *
pop_readyq(isc__taskmgr_t *manager) {
	isc__task_t *task;

	if (manager->mode == isc_taskmgrmode_normal)
		task = HEAD(manager->ready_tasks);
	else
		task = HEAD(manager->ready_priority_tasks);

	if (task != NULL) {
		DEQUEUE(manager->ready_tasks, task, ready_link);
		if (ISC_LINK_LINKED(task, ready_priority_link))
			DEQUEUE(manager->ready_priority_tasks, task,
				ready_priority_link);
	}

	return (task);
}

/*
 * Push 'task' onto the ready_tasks queue.  If 'task' has the privilege
 * flag set, then also push it onto the ready_priority_tasks queue.
 *
 * Caller must hold the task manager lock.
 */
static inline void
push_readyq(isc__taskmgr_t *manager, isc__task_t *task) {
	ENQUEUE(manager->ready_tasks, task, ready_link);
	if ((task->flags & TASK_F_PRIVILEGED) != 0)
		ENQUEUE(manager->ready_priority_tasks, task,
			ready_priority_link);
	manager->tasks_ready++;
}

static void
dispatch(isc__taskmgr_t *manager) {
	isc__task_t *task;
#ifndef USE_WORKER_THREADS
	unsigned int total_dispatch_count = 0;
	isc__tasklist_t new_ready_tasks;
	isc__tasklist_t new_priority_tasks;
	unsigned int tasks_ready = 0;
#endif /* USE_WORKER_THREADS */

	REQUIRE(VALID_MANAGER(manager));

	/*
	 * Again we're trying to hold the lock for as short a time as possible
	 * and to do as little locking and unlocking as possible.
	 *
	 * In both while loops, the appropriate lock must be held before the
	 * while body starts.  Code which acquired the lock at the top of
	 * the loop would be more readable, but would result in a lot of
	 * extra locking.  Compare:
	 *
	 * Straightforward:
	 *
	 *	LOCK();
	 *	...
	 *	UNLOCK();
	 *	while (expression) {
	 *		LOCK();
	 *		...
	 *		UNLOCK();
	 *
	 *	       	Unlocked part here...
	 *
	 *		LOCK();
	 *		...
	 *		UNLOCK();
	 *	}
	 *
	 * Note how if the loop continues we unlock and then immediately lock.
	 * For N iterations of the loop, this code does 2N+1 locks and 2N+1
	 * unlocks.  Also note that the lock is not held when the while
	 * condition is tested, which may or may not be important, depending
	 * on the expression.
	 *
	 * As written:
	 *
	 *	LOCK();
	 *	while (expression) {
	 *		...
	 *		UNLOCK();
	 *
	 *	       	Unlocked part here...
	 *
	 *		LOCK();
	 *		...
	 *	}
	 *	UNLOCK();
	 *
	 * For N iterations of the loop, this code does N+1 locks and N+1
	 * unlocks.  The while expression is always protected by the lock.
	 */

#ifndef USE_WORKER_THREADS
	ISC_LIST_INIT(new_ready_tasks);
	ISC_LIST_INIT(new_priority_tasks);
#endif
	LOCK(&manager->lock);

	while (!FINISHED(manager)) {
#ifdef USE_WORKER_THREADS
		/*
		 * For reasons similar to those given in the comment in
		 * isc_task_send() above, it is safe for us to dequeue
		 * the task while only holding the manager lock, and then
		 * change the task to running state while only holding the
		 * task lock.
		 *
		 * If a pause has been requested, don't do any work
		 * until it's been released.
		 */
		while ((empty_readyq(manager) || manager->pause_requested ||
			manager->exclusive_requested) && !FINISHED(manager))
		{
			XTHREADTRACE(isc_msgcat_get(isc_msgcat,
						    ISC_MSGSET_GENERAL,
						    ISC_MSG_WAIT, "wait"));
			WAIT(&manager->work_available, &manager->lock);
			XTHREADTRACE(isc_msgcat_get(isc_msgcat,
						    ISC_MSGSET_TASK,
						    ISC_MSG_AWAKE, "awake"));
		}
#else /* USE_WORKER_THREADS */
		if (total_dispatch_count >= DEFAULT_TASKMGR_QUANTUM ||
		    empty_readyq(manager))
			break;
#endif /* USE_WORKER_THREADS */
		XTHREADTRACE(isc_msgcat_get(isc_msgcat, ISC_MSGSET_TASK,
					    ISC_MSG_WORKING, "working"));

		task = pop_readyq(manager);
		if (task != NULL) {
			unsigned int dispatch_count = 0;
			isc_boolean_t done = ISC_FALSE;
			isc_boolean_t requeue = ISC_FALSE;
			isc_boolean_t finished = ISC_FALSE;
			isc_event_t *event;

			INSIST(VALID_TASK(task));

			/*
			 * Note we only unlock the manager lock if we actually
			 * have a task to do.  We must reacquire the manager
			 * lock before exiting the 'if (task != NULL)' block.
			 */
			manager->tasks_ready--;
			manager->tasks_running++;
			UNLOCK(&manager->lock);

			LOCK(&task->lock);
			INSIST(task->state == task_state_ready);
			task->state = task_state_running;
			XTRACE(isc_msgcat_get(isc_msgcat, ISC_MSGSET_GENERAL,
					      ISC_MSG_RUNNING, "running"));
			isc_stdtime_get(&task->now);
			do {
				if (!EMPTY(task->events)) {
					event = HEAD(task->events);
					DEQUEUE(task->events, event, ev_link);
					task->nevents--;

					/*
					 * Execute the event action.
					 */
					XTRACE(isc_msgcat_get(isc_msgcat,
							    ISC_MSGSET_TASK,
							    ISC_MSG_EXECUTE,
							    "execute action"));
					if (event->ev_action != NULL) {
						UNLOCK(&task->lock);
						(event->ev_action)(
							(isc_task_t *)task,
							event);
						LOCK(&task->lock);
					}
					dispatch_count++;
#ifndef USE_WORKER_THREADS
					total_dispatch_count++;
#endif /* USE_WORKER_THREADS */
				}

				if (task->references == 0 &&
				    EMPTY(task->events) &&
				    !TASK_SHUTTINGDOWN(task)) {
					isc_boolean_t was_idle;

					/*
					 * There are no references and no
					 * pending events for this task,
					 * which means it will not become
					 * runnable again via an external
					 * action (such as sending an event
					 * or detaching).
					 *
					 * We initiate shutdown to prevent
					 * it from becoming a zombie.
					 *
					 * We do this here instead of in
					 * the "if EMPTY(task->events)" block
					 * below because:
					 *
					 *	If we post no shutdown events,
					 *	we want the task to finish.
					 *
					 *	If we did post shutdown events,
					 *	will still want the task's
					 *	quantum to be applied.
					 */
					was_idle = task_shutdown(task);
					INSIST(!was_idle);
				}

				if (EMPTY(task->events)) {
					/*
					 * Nothing else to do for this task
					 * right now.
					 */
					XTRACE(isc_msgcat_get(isc_msgcat,
							      ISC_MSGSET_TASK,
							      ISC_MSG_EMPTY,
							      "empty"));
					if (task->references == 0 &&
					    TASK_SHUTTINGDOWN(task)) {
						/*
						 * The task is done.
						 */
						XTRACE(isc_msgcat_get(
							       isc_msgcat,
							       ISC_MSGSET_TASK,
							       ISC_MSG_DONE,
							       "done"));
						finished = ISC_TRUE;
						task->state = task_state_done;
					} else
						task->state = task_state_idle;
					done = ISC_TRUE;
				} else if (dispatch_count >= task->quantum) {
					/*
					 * Our quantum has expired, but
					 * there is more work to be done.
					 * We'll requeue it to the ready
					 * queue later.
					 *
					 * We don't check quantum until
					 * dispatching at least one event,
					 * so the minimum quantum is one.
					 */
					XTRACE(isc_msgcat_get(isc_msgcat,
							      ISC_MSGSET_TASK,
							      ISC_MSG_QUANTUM,
							      "quantum"));
					task->state = task_state_ready;
					requeue = ISC_TRUE;
					done = ISC_TRUE;
				}
			} while (!done);
			UNLOCK(&task->lock);

			if (finished)
				task_finished(task);

			LOCK(&manager->lock);
			manager->tasks_running--;
#ifdef USE_WORKER_THREADS
			if (manager->exclusive_requested &&
			    manager->tasks_running == 1) {
				SIGNAL(&manager->exclusive_granted);
			} else if (manager->pause_requested &&
				   manager->tasks_running == 0) {
				SIGNAL(&manager->paused);
			}
#endif /* USE_WORKER_THREADS */
			if (requeue) {
				/*
				 * We know we're awake, so we don't have
				 * to wakeup any sleeping threads if the
				 * ready queue is empty before we requeue.
				 *
				 * A possible optimization if the queue is
				 * empty is to 'goto' the 'if (task != NULL)'
				 * block, avoiding the ENQUEUE of the task
				 * and the subsequent immediate DEQUEUE
				 * (since it is the only executable task).
				 * We don't do this because then we'd be
				 * skipping the exit_requested check.  The
				 * cost of ENQUEUE is low anyway, especially
				 * when you consider that we'd have to do
				 * an extra EMPTY check to see if we could
				 * do the optimization.  If the ready queue
				 * were usually nonempty, the 'optimization'
				 * might even hurt rather than help.
				 */
#ifdef USE_WORKER_THREADS
				push_readyq(manager, task);
#else
				ENQUEUE(new_ready_tasks, task, ready_link);
				if ((task->flags & TASK_F_PRIVILEGED) != 0)
					ENQUEUE(new_priority_tasks, task,
						ready_priority_link);
				tasks_ready++;
#endif
			}
		}

#ifdef USE_WORKER_THREADS
		/*
		 * If we are in privileged execution mode and there are no
		 * tasks remaining on the current ready queue, then
		 * we're stuck.  Automatically drop privileges at that
		 * point and continue with the regular ready queue.
		 */
		if (manager->tasks_running == 0 && empty_readyq(manager)) {
			manager->mode = isc_taskmgrmode_normal;
			if (!empty_readyq(manager))
				BROADCAST(&manager->work_available);
		}
#endif
	}

#ifndef USE_WORKER_THREADS
	ISC_LIST_APPENDLIST(manager->ready_tasks, new_ready_tasks, ready_link);
	ISC_LIST_APPENDLIST(manager->ready_priority_tasks, new_priority_tasks,
			    ready_priority_link);
	manager->tasks_ready += tasks_ready;
	if (empty_readyq(manager))
		manager->mode = isc_taskmgrmode_normal;
#endif

	UNLOCK(&manager->lock);
}

#ifdef USE_WORKER_THREADS
static isc_threadresult_t
#ifdef _WIN32
WINAPI
#endif
run(void *uap) {
	isc__taskmgr_t *manager = uap;

	XTHREADTRACE(isc_msgcat_get(isc_msgcat, ISC_MSGSET_GENERAL,
				    ISC_MSG_STARTING, "starting"));

	dispatch(manager);

	XTHREADTRACE(isc_msgcat_get(isc_msgcat, ISC_MSGSET_GENERAL,
				    ISC_MSG_EXITING, "exiting"));

#ifdef OPENSSL_LEAKS
	ERR_remove_state(0);
#endif

	return ((isc_threadresult_t)0);
}
#endif /* USE_WORKER_THREADS */

static void
manager_free(isc__taskmgr_t *manager) {
	isc_mem_t *mctx;

#ifdef USE_WORKER_THREADS
	(void)isc_condition_destroy(&manager->exclusive_granted);
	(void)isc_condition_destroy(&manager->work_available);
	(void)isc_condition_destroy(&manager->paused);
	isc_mem_free(manager->mctx, manager->threads);
#endif /* USE_WORKER_THREADS */
	DESTROYLOCK(&manager->lock);
	DESTROYLOCK(&manager->excl_lock);
	manager->common.impmagic = 0;
	manager->common.magic = 0;
	mctx = manager->mctx;
	isc_mem_put(mctx, manager, sizeof(*manager));
	isc_mem_detach(&mctx);

#ifdef USE_SHARED_MANAGER
	taskmgr = NULL;
#endif	/* USE_SHARED_MANAGER */
}

isc_result_t
isc__taskmgr_create(isc_mem_t *mctx, unsigned int workers,
		    unsigned int default_quantum, isc_taskmgr_t **managerp)
{
	isc_result_t result;
	unsigned int i, started = 0;
	isc__taskmgr_t *manager;

	/*
	 * Create a new task manager.
	 */

	REQUIRE(workers > 0);
	REQUIRE(managerp != NULL && *managerp == NULL);

#ifndef USE_WORKER_THREADS
	UNUSED(i);
	UNUSED(started);
#endif

#ifdef USE_SHARED_MANAGER
	if (taskmgr != NULL) {
		if (taskmgr->refs == 0)
			return (ISC_R_SHUTTINGDOWN);
		taskmgr->refs++;
		*managerp = (isc_taskmgr_t *)taskmgr;
		return (ISC_R_SUCCESS);
	}
#endif /* USE_SHARED_MANAGER */

	manager = isc_mem_get(mctx, sizeof(*manager));
	if (manager == NULL)
		return (ISC_R_NOMEMORY);
	manager->common.methods = &taskmgrmethods;
	manager->common.impmagic = TASK_MANAGER_MAGIC;
	manager->common.magic = ISCAPI_TASKMGR_MAGIC;
	manager->mode = isc_taskmgrmode_normal;
	manager->mctx = NULL;
	result = isc_mutex_init(&manager->lock);
	if (result != ISC_R_SUCCESS)
		goto cleanup_mgr;
	result = isc_mutex_init(&manager->excl_lock);
	if (result != ISC_R_SUCCESS) {
		DESTROYLOCK(&manager->lock);
		goto cleanup_mgr;
	}

#ifdef USE_WORKER_THREADS
	manager->workers = 0;
	manager->threads = isc_mem_allocate(mctx,
					    workers * sizeof(isc_thread_t));
	if (manager->threads == NULL) {
		result = ISC_R_NOMEMORY;
		goto cleanup_lock;
	}
	if (isc_condition_init(&manager->work_available) != ISC_R_SUCCESS) {
		UNEXPECTED_ERROR(__FILE__, __LINE__,
				 "isc_condition_init() %s",
				 isc_msgcat_get(isc_msgcat, ISC_MSGSET_GENERAL,
						ISC_MSG_FAILED, "failed"));
		result = ISC_R_UNEXPECTED;
		goto cleanup_threads;
	}
	if (isc_condition_init(&manager->exclusive_granted) != ISC_R_SUCCESS) {
		UNEXPECTED_ERROR(__FILE__, __LINE__,
				 "isc_condition_init() %s",
				 isc_msgcat_get(isc_msgcat, ISC_MSGSET_GENERAL,
						ISC_MSG_FAILED, "failed"));
		result = ISC_R_UNEXPECTED;
		goto cleanup_workavailable;
	}
	if (isc_condition_init(&manager->paused) != ISC_R_SUCCESS) {
		UNEXPECTED_ERROR(__FILE__, __LINE__,
				 "isc_condition_init() %s",
				 isc_msgcat_get(isc_msgcat, ISC_MSGSET_GENERAL,
						ISC_MSG_FAILED, "failed"));
		result = ISC_R_UNEXPECTED;
		goto cleanup_exclusivegranted;
	}
#endif /* USE_WORKER_THREADS */
	if (default_quantum == 0)
		default_quantum = DEFAULT_DEFAULT_QUANTUM;
	manager->default_quantum = default_quantum;
	INIT_LIST(manager->tasks);
	INIT_LIST(manager->ready_tasks);
	INIT_LIST(manager->ready_priority_tasks);
	manager->tasks_running = 0;
	manager->tasks_ready = 0;
	manager->exclusive_requested = ISC_FALSE;
	manager->pause_requested = ISC_FALSE;
	manager->exiting = ISC_FALSE;
	manager->excl = NULL;

	isc_mem_attach(mctx, &manager->mctx);

#ifdef USE_WORKER_THREADS
	LOCK(&manager->lock);
	/*
	 * Start workers.
	 */
	for (i = 0; i < workers; i++) {
		if (isc_thread_create(run, manager,
				      &manager->threads[manager->workers]) ==
		    ISC_R_SUCCESS) {
			manager->workers++;
			started++;
		}
	}
	UNLOCK(&manager->lock);

	if (started == 0) {
		manager_free(manager);
		return (ISC_R_NOTHREADS);
	}
	isc_thread_setconcurrency(workers);
#endif /* USE_WORKER_THREADS */
#ifdef USE_SHARED_MANAGER
	manager->refs = 1;
	taskmgr = manager;
#endif /* USE_SHARED_MANAGER */

	*managerp = (isc_taskmgr_t *)manager;

	return (ISC_R_SUCCESS);

#ifdef USE_WORKER_THREADS
 cleanup_exclusivegranted:
	(void)isc_condition_destroy(&manager->exclusive_granted);
 cleanup_workavailable:
	(void)isc_condition_destroy(&manager->work_available);
 cleanup_threads:
	isc_mem_free(mctx, manager->threads);
 cleanup_lock:
	DESTROYLOCK(&manager->lock);
#endif
 cleanup_mgr:
	isc_mem_put(mctx, manager, sizeof(*manager));
	return (result);
}

void
isc__taskmgr_destroy(isc_taskmgr_t **managerp) {
	isc__taskmgr_t *manager;
	isc__task_t *task;
	unsigned int i;

	/*
	 * Destroy '*managerp'.
	 */

	REQUIRE(managerp != NULL);
	manager = (isc__taskmgr_t *)*managerp;
	REQUIRE(VALID_MANAGER(manager));

#ifndef USE_WORKER_THREADS
	UNUSED(i);
#endif /* USE_WORKER_THREADS */

#ifdef USE_SHARED_MANAGER
	manager->refs--;
	if (manager->refs > 0) {
		*managerp = NULL;
		return;
	}
#endif

	XTHREADTRACE("isc_taskmgr_destroy");
	/*
	 * Only one non-worker thread may ever call this routine.
	 * If a worker thread wants to initiate shutdown of the
	 * task manager, it should ask some non-worker thread to call
	 * isc_taskmgr_destroy(), e.g. by signalling a condition variable
	 * that the startup thread is sleeping on.
	 */

	/*
	 * Detach the exclusive task before acquiring the manager lock
	 */
	LOCK(&manager->excl_lock);
	if (manager->excl != NULL)
		isc__task_detach((isc_task_t **) &manager->excl);
	UNLOCK(&manager->excl_lock);

	/*
	 * Unlike elsewhere, we're going to hold this lock a long time.
	 * We need to do so, because otherwise the list of tasks could
	 * change while we were traversing it.
	 *
	 * This is also the only function where we will hold both the
	 * task manager lock and a task lock at the same time.
	 */

	LOCK(&manager->lock);

	/*
	 * Make sure we only get called once.
	 */
	INSIST(!manager->exiting);
	manager->exiting = ISC_TRUE;

	/*
	 * If privileged mode was on, turn it off.
	 */
	manager->mode = isc_taskmgrmode_normal;

	/*
	 * Post shutdown event(s) to every task (if they haven't already been
	 * posted).
	 */
	for (task = HEAD(manager->tasks);
	     task != NULL;
	     task = NEXT(task, link)) {
		LOCK(&task->lock);
		if (task_shutdown(task))
			push_readyq(manager, task);
		UNLOCK(&task->lock);
	}
#ifdef USE_WORKER_THREADS
	/*
	 * Wake up any sleeping workers.  This ensures we get work done if
	 * there's work left to do, and if there are already no tasks left
	 * it will cause the workers to see manager->exiting.
	 */
	BROADCAST(&manager->work_available);
	UNLOCK(&manager->lock);

	/*
	 * Wait for all the worker threads to exit.
	 */
	for (i = 0; i < manager->workers; i++)
		(void)isc_thread_join(manager->threads[i], NULL);
#else /* USE_WORKER_THREADS */
	/*
	 * Dispatch the shutdown events.
	 */
	UNLOCK(&manager->lock);
	while (isc__taskmgr_ready((isc_taskmgr_t *)manager))
		(void)isc__taskmgr_dispatch((isc_taskmgr_t *)manager);
	if (!ISC_LIST_EMPTY(manager->tasks))
		isc_mem_printallactive(stderr);
	INSIST(ISC_LIST_EMPTY(manager->tasks));
#ifdef USE_SHARED_MANAGER
	taskmgr = NULL;
#endif
#endif /* USE_WORKER_THREADS */

	manager_free(manager);

	*managerp = NULL;
}

void
isc__taskmgr_setmode(isc_taskmgr_t *manager0, isc_taskmgrmode_t mode) {
	isc__taskmgr_t *manager = (isc__taskmgr_t *)manager0;

	LOCK(&manager->lock);
	manager->mode = mode;
	UNLOCK(&manager->lock);
}

isc_taskmgrmode_t
isc__taskmgr_mode(isc_taskmgr_t *manager0) {
	isc__taskmgr_t *manager = (isc__taskmgr_t *)manager0;
	isc_taskmgrmode_t mode;
	LOCK(&manager->lock);
	mode = manager->mode;
	UNLOCK(&manager->lock);
	return (mode);
}

#ifndef USE_WORKER_THREADS
isc_boolean_t
isc__taskmgr_ready(isc_taskmgr_t *manager0) {
	isc__taskmgr_t *manager = (isc__taskmgr_t *)manager0;
	isc_boolean_t is_ready;

#ifdef USE_SHARED_MANAGER
	if (manager == NULL)
		manager = taskmgr;
#endif
	if (manager == NULL)
		return (ISC_FALSE);

	LOCK(&manager->lock);
	is_ready = !empty_readyq(manager);
	UNLOCK(&manager->lock);

	return (is_ready);
}

isc_result_t
isc__taskmgr_dispatch(isc_taskmgr_t *manager0) {
	isc__taskmgr_t *manager = (isc__taskmgr_t *)manager0;

#ifdef USE_SHARED_MANAGER
	if (manager == NULL)
		manager = taskmgr;
#endif
	if (manager == NULL)
		return (ISC_R_NOTFOUND);

	dispatch(manager);

	return (ISC_R_SUCCESS);
}

#else
void
isc__taskmgr_pause(isc_taskmgr_t *manager0) {
	isc__taskmgr_t *manager = (isc__taskmgr_t *)manager0;
	manager->pause_requested = ISC_TRUE;
	LOCK(&manager->lock);
	while (manager->tasks_running > 0) {
		WAIT(&manager->paused, &manager->lock);
	}
	UNLOCK(&manager->lock);
}

void
isc__taskmgr_resume(isc_taskmgr_t *manager0) {
	isc__taskmgr_t *manager = (isc__taskmgr_t *)manager0;

	LOCK(&manager->lock);
	if (manager->pause_requested) {
		manager->pause_requested = ISC_FALSE;
		BROADCAST(&manager->work_available);
	}
	UNLOCK(&manager->lock);
}
#endif /* USE_WORKER_THREADS */

void
isc_taskmgr_setexcltask(isc_taskmgr_t *mgr0, isc_task_t *task0) {
	isc__taskmgr_t *mgr = (isc__taskmgr_t *) mgr0;
	isc__task_t *task = (isc__task_t *) task0;

	REQUIRE(VALID_MANAGER(mgr));
	REQUIRE(VALID_TASK(task));
	LOCK(&mgr->excl_lock);
	if (mgr->excl != NULL)
		isc__task_detach((isc_task_t **) &mgr->excl);
	isc__task_attach(task0, (isc_task_t **) &mgr->excl);
	UNLOCK(&mgr->excl_lock);
}

isc_result_t
isc_taskmgr_excltask(isc_taskmgr_t *mgr0, isc_task_t **taskp) {
	isc__taskmgr_t *mgr = (isc__taskmgr_t *) mgr0;
	isc_result_t result = ISC_R_SUCCESS;

	REQUIRE(VALID_MANAGER(mgr));
	REQUIRE(taskp != NULL && *taskp == NULL);

	LOCK(&mgr->excl_lock);
	if (mgr->excl != NULL)
		isc__task_attach((isc_task_t *) mgr->excl, taskp);
	else
		result = ISC_R_NOTFOUND;
	UNLOCK(&mgr->excl_lock);

	return (result);
}

isc_result_t
isc__task_beginexclusive(isc_task_t *task0) {
#ifdef USE_WORKER_THREADS
	isc__task_t *task = (isc__task_t *)task0;
	isc__taskmgr_t *manager = task->manager;

	REQUIRE(task->state == task_state_running);
	/* XXX: Require task == manager->excl? */

	LOCK(&manager->lock);
	if (manager->exclusive_requested) {
		UNLOCK(&manager->lock);
		return (ISC_R_LOCKBUSY);
	}
	manager->exclusive_requested = ISC_TRUE;
	while (manager->tasks_running > 1) {
		WAIT(&manager->exclusive_granted, &manager->lock);
	}
	UNLOCK(&manager->lock);
#else
	UNUSED(task0);
#endif
	return (ISC_R_SUCCESS);
}

void
isc__task_endexclusive(isc_task_t *task0) {
#ifdef USE_WORKER_THREADS
	isc__task_t *task = (isc__task_t *)task0;
	isc__taskmgr_t *manager = task->manager;

	REQUIRE(task->state == task_state_running);
	LOCK(&manager->lock);
	REQUIRE(manager->exclusive_requested);
	manager->exclusive_requested = ISC_FALSE;
	BROADCAST(&manager->work_available);
	UNLOCK(&manager->lock);
#else
	UNUSED(task0);
#endif
}

void
isc__task_setprivilege(isc_task_t *task0, isc_boolean_t priv) {
	isc__task_t *task = (isc__task_t *)task0;
	isc__taskmgr_t *manager = task->manager;
	isc_boolean_t oldpriv;

	LOCK(&task->lock);
	oldpriv = ISC_TF((task->flags & TASK_F_PRIVILEGED) != 0);
	if (priv)
		task->flags |= TASK_F_PRIVILEGED;
	else
		task->flags &= ~TASK_F_PRIVILEGED;
	UNLOCK(&task->lock);

	if (priv == oldpriv)
		return;

	LOCK(&manager->lock);
	if (priv && ISC_LINK_LINKED(task, ready_link))
		ENQUEUE(manager->ready_priority_tasks, task,
			ready_priority_link);
	else if (!priv && ISC_LINK_LINKED(task, ready_priority_link))
		DEQUEUE(manager->ready_priority_tasks, task,
			ready_priority_link);
	UNLOCK(&manager->lock);
}

isc_boolean_t
isc__task_privilege(isc_task_t *task0) {
	isc__task_t *task = (isc__task_t *)task0;
	isc_boolean_t priv;

	LOCK(&task->lock);
	priv = ISC_TF((task->flags & TASK_F_PRIVILEGED) != 0);
	UNLOCK(&task->lock);
	return (priv);
}

isc_result_t
isc__task_register(void) {
	return (isc_task_register(isc__taskmgr_create));
}

isc_boolean_t
isc_task_exiting(isc_task_t *t) {
	isc__task_t *task = (isc__task_t *)t;

	REQUIRE(VALID_TASK(task));
	return (TASK_SHUTTINGDOWN(task));
}


#ifdef HAVE_LIBXML2
#define TRY0(a) do { xmlrc = (a); if (xmlrc < 0) goto error; } while(/*CONSTCOND*/0)
int
isc_taskmgr_renderxml(isc_taskmgr_t *mgr0, xmlTextWriterPtr writer) {
	isc__taskmgr_t *mgr = (isc__taskmgr_t *)mgr0;
	isc__task_t *task = NULL;
	int xmlrc;

	LOCK(&mgr->lock);

	/*
	 * Write out the thread-model, and some details about each depending
	 * on which type is enabled.
	 */
	TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "thread-model"));
#ifdef ISC_PLATFORM_USETHREADS
	TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "type"));
	TRY0(xmlTextWriterWriteString(writer, ISC_XMLCHAR "threaded"));
	TRY0(xmlTextWriterEndElement(writer)); /* type */

	TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "worker-threads"));
	TRY0(xmlTextWriterWriteFormatString(writer, "%d", mgr->workers));
	TRY0(xmlTextWriterEndElement(writer)); /* worker-threads */
#else /* ISC_PLATFORM_USETHREADS */
	TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "type"));
	TRY0(xmlTextWriterWriteString(writer, ISC_XMLCHAR "non-threaded"));
	TRY0(xmlTextWriterEndElement(writer)); /* type */

	TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "references"));
	TRY0(xmlTextWriterWriteFormatString(writer, "%d", mgr->refs));
	TRY0(xmlTextWriterEndElement(writer)); /* references */
#endif /* ISC_PLATFORM_USETHREADS */

	TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "default-quantum"));
	TRY0(xmlTextWriterWriteFormatString(writer, "%d",
					    mgr->default_quantum));
	TRY0(xmlTextWriterEndElement(writer)); /* default-quantum */

	TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "tasks-running"));
	TRY0(xmlTextWriterWriteFormatString(writer, "%d", mgr->tasks_running));
	TRY0(xmlTextWriterEndElement(writer)); /* tasks-running */

	TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "tasks-ready"));
	TRY0(xmlTextWriterWriteFormatString(writer, "%d", mgr->tasks_ready));
	TRY0(xmlTextWriterEndElement(writer)); /* tasks-ready */

	TRY0(xmlTextWriterEndElement(writer)); /* thread-model */

	TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "tasks"));
	task = ISC_LIST_HEAD(mgr->tasks);
	while (task != NULL) {
		LOCK(&task->lock);
		TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "task"));

		if (task->name[0] != 0) {
			TRY0(xmlTextWriterStartElement(writer,
						       ISC_XMLCHAR "name"));
			TRY0(xmlTextWriterWriteFormatString(writer, "%s",
						       task->name));
			TRY0(xmlTextWriterEndElement(writer)); /* name */
		}

		TRY0(xmlTextWriterStartElement(writer,
					       ISC_XMLCHAR "references"));
		TRY0(xmlTextWriterWriteFormatString(writer, "%d",
						    task->references));
		TRY0(xmlTextWriterEndElement(writer)); /* references */

		TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "id"));
		TRY0(xmlTextWriterWriteFormatString(writer, "%p", task));
		TRY0(xmlTextWriterEndElement(writer)); /* id */

		TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "state"));
		TRY0(xmlTextWriterWriteFormatString(writer, "%s",
					       statenames[task->state]));
		TRY0(xmlTextWriterEndElement(writer)); /* state */

		TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "quantum"));
		TRY0(xmlTextWriterWriteFormatString(writer, "%d",
						    task->quantum));
		TRY0(xmlTextWriterEndElement(writer)); /* quantum */

		TRY0(xmlTextWriterStartElement(writer, ISC_XMLCHAR "events"));
		TRY0(xmlTextWriterWriteFormatString(writer, "%d",
						    task->nevents));
		TRY0(xmlTextWriterEndElement(writer)); /* events */

		TRY0(xmlTextWriterEndElement(writer));

		UNLOCK(&task->lock);
		task = ISC_LIST_NEXT(task, link);
	}
	TRY0(xmlTextWriterEndElement(writer)); /* tasks */

 error:
	if (task != NULL)
		UNLOCK(&task->lock);
	UNLOCK(&mgr->lock);

	return (xmlrc);
}
#endif /* HAVE_LIBXML2 */

#ifdef HAVE_JSON
#define CHECKMEM(m) do { \
	if (m == NULL) { \
		result = ISC_R_NOMEMORY;\
		goto error;\
	} \
} while(/*CONSTCOND*/0)

isc_result_t
isc_taskmgr_renderjson(isc_taskmgr_t *mgr0, json_object *tasks) {
	isc_result_t result = ISC_R_SUCCESS;
	isc__taskmgr_t *mgr = (isc__taskmgr_t *)mgr0;
	isc__task_t *task = NULL;
	json_object *obj = NULL, *array = NULL, *taskobj = NULL;

	LOCK(&mgr->lock);

	/*
	 * Write out the thread-model, and some details about each depending
	 * on which type is enabled.
	 */
#ifdef ISC_PLATFORM_USETHREADS
	obj = json_object_new_string("threaded");
	CHECKMEM(obj);
	json_object_object_add(tasks, "thread-model", obj);

	obj = json_object_new_int(mgr->workers);
	CHECKMEM(obj);
	json_object_object_add(tasks, "worker-threads", obj);
#else /* ISC_PLATFORM_USETHREADS */
	obj = json_object_new_string("non-threaded");
	CHECKMEM(obj);
	json_object_object_add(tasks, "thread-model", obj);

	obj = json_object_new_int(mgr->refs);
	CHECKMEM(obj);
	json_object_object_add(tasks, "references", obj);
#endif /* ISC_PLATFORM_USETHREADS */

	obj = json_object_new_int(mgr->default_quantum);
	CHECKMEM(obj);
	json_object_object_add(tasks, "default-quantum", obj);

	obj = json_object_new_int(mgr->tasks_running);
	CHECKMEM(obj);
	json_object_object_add(tasks, "tasks-running", obj);

	obj = json_object_new_int(mgr->tasks_ready);
	CHECKMEM(obj);
	json_object_object_add(tasks, "tasks-ready", obj);

	array = json_object_new_array();
	CHECKMEM(array);

	for (task = ISC_LIST_HEAD(mgr->tasks);
	     task != NULL;
	     task = ISC_LIST_NEXT(task, link))
	{
		char buf[255];

		LOCK(&task->lock);

		taskobj = json_object_new_object();
		CHECKMEM(taskobj);
		json_object_array_add(array, taskobj);

		sprintf(buf, "%p", task);
		obj = json_object_new_string(buf);
		CHECKMEM(obj);
		json_object_object_add(taskobj, "id", obj);

		if (task->name[0] != 0) {
			obj = json_object_new_string(task->name);
			CHECKMEM(obj);
			json_object_object_add(taskobj, "name", obj);
		}

		obj = json_object_new_int(task->references);
		CHECKMEM(obj);
		json_object_object_add(taskobj, "references", obj);

		obj = json_object_new_string(statenames[task->state]);
		CHECKMEM(obj);
		json_object_object_add(taskobj, "state", obj);

		obj = json_object_new_int(task->quantum);
		CHECKMEM(obj);
		json_object_object_add(taskobj, "quantum", obj);

		obj = json_object_new_int(task->nevents);
		CHECKMEM(obj);
		json_object_object_add(taskobj, "events", obj);

		UNLOCK(&task->lock);
	}

	json_object_object_add(tasks, "tasks", array);
	array = NULL;
	result = ISC_R_SUCCESS;

 error:
	if (array != NULL)
		json_object_put(array);

	if (task != NULL)
		UNLOCK(&task->lock);
	UNLOCK(&mgr->lock);

	return (result);
}
#endif


static isc_mutex_t createlock;
static isc_once_t once = ISC_ONCE_INIT;
static isc_taskmgrcreatefunc_t taskmgr_createfunc = NULL;

static void
initialize(void) {
	RUNTIME_CHECK(isc_mutex_init(&createlock) == ISC_R_SUCCESS);
}

isc_result_t
isc_task_register(isc_taskmgrcreatefunc_t createfunc) {
	isc_result_t result = ISC_R_SUCCESS;

	RUNTIME_CHECK(isc_once_do(&once, initialize) == ISC_R_SUCCESS);

	LOCK(&createlock);
	if (taskmgr_createfunc == NULL)
		taskmgr_createfunc = createfunc;
	else
		result = ISC_R_EXISTS;
	UNLOCK(&createlock);

	return (result);
}

isc_result_t
isc_taskmgr_createinctx(isc_mem_t *mctx, isc_appctx_t *actx,
			unsigned int workers, unsigned int default_quantum,
			isc_taskmgr_t **managerp)
{
	isc_result_t result;

	LOCK(&createlock);

	REQUIRE(taskmgr_createfunc != NULL);
	result = (*taskmgr_createfunc)(mctx, workers, default_quantum,
				       managerp);

	UNLOCK(&createlock);

	if (result == ISC_R_SUCCESS)
		isc_appctx_settaskmgr(actx, *managerp);

	return (result);
}

isc_result_t
isc_taskmgr_create(isc_mem_t *mctx, unsigned int workers,
		   unsigned int default_quantum, isc_taskmgr_t **managerp)
{
	isc_result_t result;

	if (isc_bind9)
		return (isc__taskmgr_create(mctx, workers,
					    default_quantum, managerp));
	LOCK(&createlock);

	REQUIRE(taskmgr_createfunc != NULL);
	result = (*taskmgr_createfunc)(mctx, workers, default_quantum,
				       managerp);

	UNLOCK(&createlock);

	return (result);
}

void
isc_taskmgr_destroy(isc_taskmgr_t **managerp) {
	REQUIRE(managerp != NULL && ISCAPI_TASKMGR_VALID(*managerp));

	if (isc_bind9)
		isc__taskmgr_destroy(managerp);
	else
		(*managerp)->methods->destroy(managerp);

	ENSURE(*managerp == NULL);
}

void
isc_taskmgr_setmode(isc_taskmgr_t *manager, isc_taskmgrmode_t mode) {
	REQUIRE(ISCAPI_TASKMGR_VALID(manager));

	if (isc_bind9)
		isc__taskmgr_setmode(manager, mode);
	else
		manager->methods->setmode(manager, mode);
}

isc_taskmgrmode_t
isc_taskmgr_mode(isc_taskmgr_t *manager) {
	REQUIRE(ISCAPI_TASKMGR_VALID(manager));

	if (isc_bind9)
		return (isc__taskmgr_mode(manager));

	return (manager->methods->mode(manager));
}

isc_result_t
isc_task_create(isc_taskmgr_t *manager, unsigned int quantum,
		isc_task_t **taskp)
{
	REQUIRE(ISCAPI_TASKMGR_VALID(manager));
	REQUIRE(taskp != NULL && *taskp == NULL);

	if (isc_bind9)
		return (isc__task_create(manager, quantum, taskp));

	return (manager->methods->taskcreate(manager, quantum, taskp));
}

void
isc_task_attach(isc_task_t *source, isc_task_t **targetp) {
	REQUIRE(ISCAPI_TASK_VALID(source));
	REQUIRE(targetp != NULL && *targetp == NULL);

	if (isc_bind9)
		isc__task_attach(source, targetp);
	else
		source->methods->attach(source, targetp);

	ENSURE(*targetp == source);
}

void
isc_task_detach(isc_task_t **taskp) {
	REQUIRE(taskp != NULL && ISCAPI_TASK_VALID(*taskp));

	if (isc_bind9)
		isc__task_detach(taskp);
	else
		(*taskp)->methods->detach(taskp);

	ENSURE(*taskp == NULL);
}

void
isc_task_send(isc_task_t *task, isc_event_t **eventp) {
	REQUIRE(ISCAPI_TASK_VALID(task));
	REQUIRE(eventp != NULL && *eventp != NULL);

	if (isc_bind9)
		isc__task_send(task, eventp);
	else {
		task->methods->send(task, eventp);
		ENSURE(*eventp == NULL);
	}
}

void
isc_task_sendanddetach(isc_task_t **taskp, isc_event_t **eventp) {
	REQUIRE(taskp != NULL && ISCAPI_TASK_VALID(*taskp));
	REQUIRE(eventp != NULL && *eventp != NULL);

	if (isc_bind9)
		isc__task_sendanddetach(taskp, eventp);
	else {
		(*taskp)->methods->sendanddetach(taskp, eventp);
		ENSURE(*eventp == NULL);
	}

	ENSURE(*taskp == NULL);
}

unsigned int
isc_task_unsend(isc_task_t *task, void *sender, isc_eventtype_t type,
		void *tag, isc_eventlist_t *events)
{
	REQUIRE(ISCAPI_TASK_VALID(task));

	if (isc_bind9)
		return (isc__task_unsend(task, sender, type, tag, events));

	return (task->methods->unsend(task, sender, type, tag, events));
}

isc_result_t
isc_task_onshutdown(isc_task_t *task, isc_taskaction_t action, void *arg)
{
	REQUIRE(ISCAPI_TASK_VALID(task));

	if (isc_bind9)
		return (isc__task_onshutdown(task, action, arg));

	return (task->methods->onshutdown(task, action, arg));
}

void
isc_task_shutdown(isc_task_t *task) {
	REQUIRE(ISCAPI_TASK_VALID(task));

	if (isc_bind9)
		isc__task_shutdown(task);
	else
		task->methods->shutdown(task);
}

void
isc_task_destroy(isc_task_t **taskp) {
	if (!isc_bind9)
		return;

	isc__task_destroy(taskp);
}

void
isc_task_setname(isc_task_t *task, const char *name, void *tag) {
	REQUIRE(ISCAPI_TASK_VALID(task));

	if (isc_bind9)
		isc__task_setname(task, name, tag);
	else
		task->methods->setname(task, name, tag);
}

unsigned int
isc_task_purge(isc_task_t *task, void *sender, isc_eventtype_t type, void *tag)
{
	REQUIRE(ISCAPI_TASK_VALID(task));

	if (isc_bind9)
		return (isc__task_purge(task, sender, type, tag));

	return (task->methods->purgeevents(task, sender, type, tag));
}

isc_result_t
isc_task_beginexclusive(isc_task_t *task) {
	REQUIRE(ISCAPI_TASK_VALID(task));

	if (isc_bind9)
		return (isc__task_beginexclusive(task));

	return (task->methods->beginexclusive(task));
}

void
isc_task_endexclusive(isc_task_t *task) {
	REQUIRE(ISCAPI_TASK_VALID(task));

	if (isc_bind9)
		isc__task_endexclusive(task);
	else
		task->methods->endexclusive(task);
}

void
isc_task_setprivilege(isc_task_t *task, isc_boolean_t priv) {
	REQUIRE(ISCAPI_TASK_VALID(task));

	if (isc_bind9)
		isc__task_setprivilege(task, priv);
	else
		task->methods->setprivilege(task, priv);
}

isc_boolean_t
isc_task_privilege(isc_task_t *task) {
	REQUIRE(ISCAPI_TASK_VALID(task));

	if (isc_bind9)
		return (isc__task_privilege(task));

	return (task->methods->privilege(task));
}

void
isc_task_getcurrenttime(isc_task_t *task, isc_stdtime_t *t) {
	if (!isc_bind9)
		return;

	isc__task_getcurrenttime(task, t);
}

/*%
 * This is necessary for libisc's internal timer implementation.  Other
 * implementation might skip implementing this.
 */
unsigned int
isc_task_purgerange(isc_task_t *task, void *sender, isc_eventtype_t first,
		    isc_eventtype_t last, void *tag)
{
	REQUIRE(ISCAPI_TASK_VALID(task));

	if (isc_bind9)
		return (isc__task_purgerange(task, sender, first, last, tag));

	return (task->methods->purgerange(task, sender, first, last, tag));
}
