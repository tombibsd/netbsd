/*	$NetBSD$	*/

/*-
 * Copyright (c) 1997-2013 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Michael Graff <explorer@flame.org> and Thor Lancelot Simon.
 * This code uses ideas and algorithms from the Linux driver written by
 * Ted Ts'o.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include <sys/param.h>
#include <sys/atomic.h>
#include <sys/callout.h>
#include <sys/fcntl.h>
#include <sys/intr.h>
#include <sys/ioctl.h>
#include <sys/kauth.h>
#include <sys/kernel.h>
#include <sys/kmem.h>
#include <sys/mutex.h>
#include <sys/pool.h>
#include <sys/proc.h>
#include <sys/rnd.h>
#include <sys/rndpool.h>
#include <sys/rndsink.h>
#include <sys/rndsource.h>
#include <sys/rngtest.h>
#include <sys/systm.h>

#include <dev/rnd_private.h>

#ifdef COMPAT_50
#include <compat/sys/rnd.h>
#endif

#if defined(__HAVE_CPU_RNG) && !defined(_RUMPKERNEL)
#include <machine/cpu_rng.h>
#endif

#if defined(__HAVE_CPU_COUNTER)
#include <machine/cpu_counter.h>
#endif

#ifdef RND_DEBUG
#define	DPRINTF(l,x)      if (rnd_debug & (l)) rnd_printf x
int	rnd_debug = 0;
#else
#define	DPRINTF(l,x)
#endif

/*
 * list devices attached
 */
#if 0
#define	RND_VERBOSE
#endif

#ifdef RND_VERBOSE
#define	rnd_printf_verbose(fmt, ...)	rnd_printf(fmt, ##__VA_ARGS__)
#else
#define	rnd_printf_verbose(fmt, ...)	((void)0)
#endif

#ifdef RND_VERBOSE
static unsigned int deltacnt;
#endif

/*
 * This is a little bit of state information attached to each device that we
 * collect entropy from.  This is simply a collection buffer, and when it
 * is full it will be "detached" from the source and added to the entropy
 * pool after entropy is distilled as much as possible.
 */
#define	RND_SAMPLE_COUNT	64	/* collect N samples, then compress */
typedef struct _rnd_sample_t {
	SIMPLEQ_ENTRY(_rnd_sample_t) next;
	krndsource_t	*source;
	int		cursor;
	int		entropy;
	uint32_t	ts[RND_SAMPLE_COUNT];
	uint32_t	values[RND_SAMPLE_COUNT];
} rnd_sample_t;

SIMPLEQ_HEAD(rnd_sampleq, _rnd_sample_t);

/*
 * The sample queue.  Samples are put into the queue and processed in a
 * softint in order to limit the latency of adding a sample.
 */
static struct {
	kmutex_t		lock;
	struct rnd_sampleq	q;
} rnd_samples __cacheline_aligned;

/*
 * Memory pool for sample buffers
 */
static pool_cache_t rnd_mempc __read_mostly;

/*
 * Global entropy pool and sources.
 */
static struct {
	kmutex_t		lock;
	rndpool_t		pool;
	LIST_HEAD(, krndsource)	sources;
	kcondvar_t		cv;
} rnd_global __cacheline_aligned;

/*
 * This source is used to easily "remove" queue entries when the source
 * which actually generated the events is going away.
 */
static krndsource_t rnd_source_no_collect = {
	/* LIST_ENTRY list */
	.name = { 'N', 'o', 'C', 'o', 'l', 'l', 'e', 'c', 't',
		   0, 0, 0, 0, 0, 0, 0 },
	.total = 0,
	.type = RND_TYPE_UNKNOWN,
	.flags = (RND_FLAG_NO_COLLECT |
		  RND_FLAG_NO_ESTIMATE),
	.state = NULL,
	.test_cnt = 0,
	.test = NULL
};

krndsource_t rnd_printf_source, rnd_autoconf_source;

static void *rnd_process __read_mostly;
static void *rnd_wakeup __read_mostly;

static inline uint32_t	rnd_counter(void);
static        void	rnd_intr(void *);
static	      void	rnd_wake(void *);
static	      void	rnd_process_events(void);
static	      void	rnd_add_data_ts(krndsource_t *, const void *const,
					uint32_t, uint32_t, uint32_t, bool);
static inline void	rnd_schedule_process(void);

int			rnd_ready = 0;
int			rnd_initial_entropy = 0;

static volatile unsigned	rnd_printing = 0;

#ifdef DIAGNOSTIC
static int		rnd_tested = 0;
static rngtest_t	rnd_rt;
static uint8_t		rnd_testbits[sizeof(rnd_rt.rt_b)];
#endif

static rndsave_t	*boot_rsp;

static inline void
rnd_printf(const char *fmt, ...)
{
	va_list ap;

	if (atomic_cas_uint(&rnd_printing, 0, 1) != 0)
		return;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	rnd_printing = 0;
}

/*
 * Generate a 32-bit counter.
 */
static inline uint32_t
rnd_counter(void)
{
	struct bintime bt;
	uint32_t ret;

#if defined(__HAVE_CPU_COUNTER)
	if (cpu_hascounter())
		return cpu_counter32();
#endif
	if (!rnd_ready)
		/* Too early to call nanotime.  */
		return 0;

	binuptime(&bt);
	ret = bt.sec;
	ret ^= bt.sec >> 32;
	ret ^= bt.frac;
	ret ^= bt.frac >> 32;

	return ret;
}

/*
 * We may be called from low IPL -- protect our softint.
 */

static inline void
rnd_schedule_softint(void *softint)
{

	kpreempt_disable();
	softint_schedule(softint);
	kpreempt_enable();
}

static inline void
rnd_schedule_process(void)
{

	if (__predict_true(rnd_process)) {
		rnd_schedule_softint(rnd_process);
		return;
	}
	rnd_process_events();
}

static inline void
rnd_schedule_wakeup(void)
{

	if (__predict_true(rnd_wakeup)) {
		rnd_schedule_softint(rnd_wakeup);
		return;
	}
	rndsinks_distribute();
}

/*
 * Tell any sources with "feed me" callbacks that we are hungry.
 */
void
rnd_getmore(size_t byteswanted)
{
	krndsource_t *rs, *next;

	/*
	 * Due to buffering in rnd_process_events, even if the entropy
	 * sources provide the requested number of bytes, users may not
	 * be woken because the data may be stuck in unfilled buffers.
	 * So ask for enough data to fill all the buffers.
	 *
	 * XXX Just get rid of this buffering and solve the
	 * /dev/random-as-side-channel-for-keystroke-timings a
	 * different way.
	 */
	byteswanted = MAX(byteswanted,
	    MAX(RND_POOLBITS/NBBY, sizeof(uint32_t)*RND_SAMPLE_COUNT));

	mutex_spin_enter(&rnd_global.lock);
	LIST_FOREACH_SAFE(rs, &rnd_global.sources, list, next) {
		/* Skip if the source is disabled.  */
		if (!RND_ENABLED(rs))
			continue;

		/* Skip if there's no callback.  */
		if (!ISSET(rs->flags, RND_FLAG_HASCB))
			continue;
		KASSERT(rs->get != NULL);

		/* Skip if there are too many users right now.  */
		if (rs->refcnt == UINT_MAX)
			continue;

		/*
		 * Hold a reference while we release rnd_global.lock to
		 * call the callback.  The callback may in turn call
		 * rnd_add_data, which acquires rnd_global.lock.
		 */
		rs->refcnt++;
		mutex_spin_exit(&rnd_global.lock);
		rs->get(byteswanted, rs->getarg);
		mutex_spin_enter(&rnd_global.lock);
		if (--rs->refcnt == 0)
			cv_broadcast(&rnd_global.cv);

		/* Dribble some goo to the console.  */
		rnd_printf_verbose("rnd: entropy estimate %zu bits\n",
		    rndpool_get_entropy_count(&rnd_global.pool));
		rnd_printf_verbose("rnd: asking source %s for %zu bytes\n",
		    rs->name, byteswanted);
	}
	mutex_spin_exit(&rnd_global.lock);

	/*
	 * Check whether we got entropy samples to process.  In that
	 * case, we may need to distribute entropy to waiters.  Do
	 * that, if we can do it asynchronously.
	 *
	 * - Conditionally because we don't want a softint loop.
	 * - Asynchronously because if we did it synchronously, we may
	 *   end up with lock recursion on rndsinks_lock.
	 */
	if (!SIMPLEQ_EMPTY(&rnd_samples.q) && rnd_process != NULL)
		rnd_schedule_process();
}

/*
 * Use the timing/value of the event to estimate the entropy gathered.
 * If all the differentials (first, second, and third) are non-zero, return
 * non-zero.  If any of these are zero, return zero.
 */
static inline uint32_t
rnd_delta_estimate(rnd_delta_t *d, uint32_t v, int32_t delta)
{
	int32_t delta2, delta3;

	d->insamples++;

	/*
	 * Calculate the second and third order differentials
	 */
	delta2 = d->dx - delta;
	if (delta2 < 0)
		delta2 = -delta2;

	delta3 = d->d2x - delta2;
	if (delta3 < 0)
		delta3 = -delta3;

	d->x = v;
	d->dx = delta;
	d->d2x = delta2;

	/*
	 * If any delta is 0, we got no entropy.  If all are non-zero, we
	 * might have something.
	 */
	if (delta == 0 || delta2 == 0 || delta3 == 0)
		return 0;

	d->outbits++;
	return 1;
}

/*
 * Delta estimator for 32-bit timeestamps.  Must handle wrap.
 */
static inline uint32_t
rnd_dt_estimate(krndsource_t *rs, uint32_t t)
{
	int32_t delta;
	uint32_t ret;
	rnd_delta_t *d = &rs->time_delta;

	if (t < d->x) {
		delta = UINT32_MAX - d->x + t;
	} else {
		delta = d->x - t;
	}

	if (delta < 0) {
		delta = -delta;
	}

	ret = rnd_delta_estimate(d, t, delta);

	KASSERT(d->x == t);
	KASSERT(d->dx == delta);
#ifdef RND_VERBOSE
	if (deltacnt++ % 1151 == 0) {
		rnd_printf_verbose("rnd_dt_estimate: %s x = %lld, dx = %lld, "
		       "d2x = %lld\n", rs->name,
		       (int)d->x, (int)d->dx, (int)d->d2x);
	}
#endif
	return ret;
}

/*
 * Delta estimator for 32 or bit values.  "Wrap" isn't.
 */
static inline uint32_t
rnd_dv_estimate(krndsource_t *rs, uint32_t v)
{
	int32_t delta;
	uint32_t ret;
	rnd_delta_t *d = &rs->value_delta;

	delta = d->x - v;

	if (delta < 0) {
		delta = -delta;
	}
	ret = rnd_delta_estimate(d, v, (uint32_t)delta);

	KASSERT(d->x == v);
	KASSERT(d->dx == delta);
#ifdef RND_VERBOSE
	if (deltacnt++ % 1151 == 0) {
		rnd_printf_verbose("rnd_dv_estimate: %s x = %lld, dx = %lld, "
		       " d2x = %lld\n", rs->name,
		       (long long int)d->x,
		       (long long int)d->dx,
		       (long long int)d->d2x);
	}
#endif
	return ret;
}

#if defined(__HAVE_CPU_RNG) && !defined(_RUMPKERNEL)
static struct {
	kmutex_t	lock;	/* unfortunately, must protect krndsource */
	krndsource_t	source;
} rnd_cpu __cacheline_aligned;

static void
rnd_cpu_get(size_t bytes, void *priv)
{
	krndsource_t *cpusrcp = priv;
	cpu_rng_t buf[2 * RND_ENTROPY_THRESHOLD / sizeof(cpu_rng_t)];
	cpu_rng_t *bufp;
	size_t cnt = __arraycount(buf);
	size_t entropy = 0;

	KASSERT(cpusrcp == &rnd_cpu.source);

	for (bufp = buf; bufp < buf + cnt; bufp++) {
		entropy += cpu_rng(bufp);
	}
	if (__predict_true(entropy)) {
		mutex_spin_enter(&rnd_cpu.lock);
		rnd_add_data_sync(cpusrcp, buf, sizeof(buf), entropy);
		explicit_memset(buf, 0, sizeof(buf));
		mutex_spin_exit(&rnd_cpu.lock);
	}
}

#endif

#if defined(__HAVE_CPU_COUNTER)
static struct {
	kmutex_t	lock;
	int		iter;
	struct callout	callout;
	krndsource_t	source;
} rnd_skew __cacheline_aligned;

static void rnd_skew_intr(void *);

static void
rnd_skew_enable(krndsource_t *rs, bool enabled)
{

	if (enabled) {
		rnd_skew_intr(rs);
	} else {
		callout_stop(&rnd_skew.callout);
	}
}

static void
rnd_skew_get(size_t bytes, void *priv)
{
	krndsource_t *skewsrcp __diagused = priv;

	KASSERT(skewsrcp == &rnd_skew.source);

	/* Measure 100 times */
	rnd_skew.iter = 100;
	callout_schedule(&rnd_skew.callout, 1);
}

static void
rnd_skew_intr(void *arg)
{
	/*
	 * Even on systems with seemingly stable clocks, the
	 * delta-time entropy estimator seems to think we get 1 bit here
	 * about every 2 calls.
	 *
	 */
	mutex_spin_enter(&rnd_skew.lock);

	if (RND_ENABLED(&rnd_skew.source)) {
		int next_ticks = 1;
		if (rnd_skew.iter & 1) {
			rnd_add_uint32(&rnd_skew.source, rnd_counter());
			next_ticks = hz / 10;
		}
		if (--rnd_skew.iter > 0) {
			callout_schedule(&rnd_skew.callout, next_ticks);
		}
	}
	mutex_spin_exit(&rnd_skew.lock);
}
#endif

void
rnd_init_softint(void)
{

	rnd_process = softint_establish(SOFTINT_SERIAL|SOFTINT_MPSAFE,
	    rnd_intr, NULL);
	rnd_wakeup = softint_establish(SOFTINT_CLOCK|SOFTINT_MPSAFE,
	    rnd_wake, NULL);
	rnd_schedule_process();
}

/*
 * Entropy was just added to the pool.  If we crossed the threshold for
 * the first time, set rnd_initial_entropy = 1.
 */
static void
rnd_entropy_added(void)
{
	uint32_t pool_entropy;

	KASSERT(mutex_owned(&rnd_global.lock));

	if (__predict_true(rnd_initial_entropy))
		return;
	pool_entropy = rndpool_get_entropy_count(&rnd_global.pool);
	if (pool_entropy > RND_ENTROPY_THRESHOLD * NBBY) {
		rnd_printf_verbose("rnd: have initial entropy (%zu)\n",
		    pool_entropy);
		rnd_initial_entropy = 1;
	}
}

/*
 * initialize the global random pool for our use.
 * rnd_init() must be called very early on in the boot process, so
 * the pool is ready for other devices to attach as sources.
 */
void
rnd_init(void)
{
	uint32_t c;

	if (rnd_ready)
		return;

	/*
	 * take a counter early, hoping that there's some variance in
	 * the following operations
	 */
	c = rnd_counter();

	rndsinks_init();

	/* Initialize the sample queue.  */
	mutex_init(&rnd_samples.lock, MUTEX_DEFAULT, IPL_VM);
	SIMPLEQ_INIT(&rnd_samples.q);

	/* Initialize the global pool and sources list.  */
	mutex_init(&rnd_global.lock, MUTEX_DEFAULT, IPL_VM);
	rndpool_init(&rnd_global.pool);
	LIST_INIT(&rnd_global.sources);
	cv_init(&rnd_global.cv, "rndsrc");

	rnd_mempc = pool_cache_init(sizeof(rnd_sample_t), 0, 0, 0,
				    "rndsample", NULL, IPL_VM,
				    NULL, NULL, NULL);

	/*
	 * Set resource limit. The rnd_process_events() function
	 * is called every tick and process the sample queue.
	 * Without limitation, if a lot of rnd_add_*() are called,
	 * all kernel memory may be eaten up.
	 */
	pool_cache_sethardlimit(rnd_mempc, RND_POOLBITS, NULL, 0);

	/*
	 * Mix *something*, *anything* into the pool to help it get started.
	 * However, it's not safe for rnd_counter() to call microtime() yet,
	 * so on some platforms we might just end up with zeros anyway.
	 * XXX more things to add would be nice.
	 */
	if (c) {
		mutex_spin_enter(&rnd_global.lock);
		rndpool_add_data(&rnd_global.pool, &c, sizeof(c), 1);
		c = rnd_counter();
		rndpool_add_data(&rnd_global.pool, &c, sizeof(c), 1);
		mutex_spin_exit(&rnd_global.lock);
	}

	/*
	 * Attach CPU RNG if available.
	 */
#if defined(__HAVE_CPU_RNG) && !defined(_RUMPKERNEL)
	if (cpu_rng_init()) {
		/* IPL_VM because taken while rnd_global.lock is held.  */
		mutex_init(&rnd_cpu.lock, MUTEX_DEFAULT, IPL_VM);
		rndsource_setcb(&rnd_cpu.source, rnd_cpu_get, &rnd_cpu.source);
		rnd_attach_source(&rnd_cpu.source, "cpurng",
		    RND_TYPE_RNG, RND_FLAG_COLLECT_VALUE|
		    RND_FLAG_HASCB|RND_FLAG_HASENABLE);
		rnd_cpu_get(RND_ENTROPY_THRESHOLD, &rnd_cpu.source);
	}
#endif

	/*
	 * If we have a cycle counter, take its error with respect
	 * to the callout mechanism as a source of entropy, ala
	 * TrueRand.
 	 *
	 */
#if defined(__HAVE_CPU_COUNTER)
	/* IPL_VM because taken while rnd_global.lock is held.  */
	mutex_init(&rnd_skew.lock, MUTEX_DEFAULT, IPL_VM);
	callout_init(&rnd_skew.callout, CALLOUT_MPSAFE);
	callout_setfunc(&rnd_skew.callout, rnd_skew_intr, NULL);
	rndsource_setcb(&rnd_skew.source, rnd_skew_get, &rnd_skew.source);
	rndsource_setenable(&rnd_skew.source, rnd_skew_enable);
	rnd_attach_source(&rnd_skew.source, "callout", RND_TYPE_SKEW,
	    RND_FLAG_COLLECT_VALUE|RND_FLAG_ESTIMATE_VALUE|
	    RND_FLAG_HASCB|RND_FLAG_HASENABLE);
	rnd_skew.iter = 100;
	rnd_skew_intr(NULL);
#endif

	rnd_printf_verbose("rnd: initialised (%u)%s", RND_POOLBITS,
	    c ? " with counter\n" : "\n");
	if (boot_rsp != NULL) {
		mutex_spin_enter(&rnd_global.lock);
		rndpool_add_data(&rnd_global.pool, boot_rsp->data,
		    sizeof(boot_rsp->data),
		    MIN(boot_rsp->entropy, RND_POOLBITS / 2));
		rnd_entropy_added();
		mutex_spin_exit(&rnd_global.lock);
		rnd_printf("rnd: seeded with %d bits\n",
		    MIN(boot_rsp->entropy, RND_POOLBITS / 2));
		explicit_memset(boot_rsp, 0, sizeof(*boot_rsp));
	}
	rnd_attach_source(&rnd_printf_source, "printf", RND_TYPE_UNKNOWN,
			  RND_FLAG_NO_ESTIMATE);
	rnd_attach_source(&rnd_autoconf_source, "autoconf",
			  RND_TYPE_UNKNOWN,
			  RND_FLAG_COLLECT_TIME|RND_FLAG_ESTIMATE_TIME);
	rnd_ready = 1;
}

static rnd_sample_t *
rnd_sample_allocate(krndsource_t *source)
{
	rnd_sample_t *c;

	c = pool_cache_get(rnd_mempc, PR_WAITOK);
	if (c == NULL)
		return NULL;

	c->source = source;
	c->cursor = 0;
	c->entropy = 0;

	return c;
}

/*
 * Don't wait on allocation.  To be used in an interrupt context.
 */
static rnd_sample_t *
rnd_sample_allocate_isr(krndsource_t *source)
{
	rnd_sample_t *c;

	c = pool_cache_get(rnd_mempc, PR_NOWAIT);
	if (c == NULL)
		return NULL;

	c->source = source;
	c->cursor = 0;
	c->entropy = 0;

	return c;
}

static void
rnd_sample_free(rnd_sample_t *c)
{

	explicit_memset(c, 0, sizeof(*c));
	pool_cache_put(rnd_mempc, c);
}

/*
 * Add a source to our list of sources.
 */
void
rnd_attach_source(krndsource_t *rs, const char *name, uint32_t type,
    uint32_t flags)
{
	uint32_t ts;

	ts = rnd_counter();

	strlcpy(rs->name, name, sizeof(rs->name));
	memset(&rs->time_delta, 0, sizeof(rs->time_delta));
	rs->time_delta.x = ts;
	memset(&rs->value_delta, 0, sizeof(rs->value_delta));
	rs->total = 0;

	/*
	 * Some source setup, by type
	 */
	rs->test = NULL;
	rs->test_cnt = -1;

	if (flags == 0) {
		flags = RND_FLAG_DEFAULT;
	}

	switch (type) {
	case RND_TYPE_NET:		/* Don't collect by default */
		flags |= (RND_FLAG_NO_COLLECT | RND_FLAG_NO_ESTIMATE);
		break;
	case RND_TYPE_RNG:		/* Space for statistical testing */
		rs->test = kmem_alloc(sizeof(rngtest_t), KM_NOSLEEP);
		rs->test_cnt = 0;
		/* FALLTHRU */
	case RND_TYPE_VM:		/* Process samples in bulk always */
		flags |= RND_FLAG_FAST;
		break;
	default:
		break;
	}

	rs->type = type;
	rs->flags = flags;
	rs->refcnt = 1;

	rs->state = rnd_sample_allocate(rs);

	mutex_spin_enter(&rnd_global.lock);
	LIST_INSERT_HEAD(&rnd_global.sources, rs, list);

#ifdef RND_VERBOSE
	rnd_printf_verbose("rnd: %s attached as an entropy source (",
	    rs->name);
	if (!(flags & RND_FLAG_NO_COLLECT)) {
		rnd_printf_verbose("collecting");
		if (flags & RND_FLAG_NO_ESTIMATE)
			rnd_printf_verbose(" without estimation");
	} else {
		rnd_printf_verbose("off");
	}
	rnd_printf_verbose(")\n");
#endif

	/*
	 * Again, put some more initial junk in the pool.
	 * FreeBSD claim to have an analysis that show 4 bits of
	 * entropy per source-attach timestamp.  I am skeptical,
	 * but we count 1 bit per source here.
	 */
	rndpool_add_data(&rnd_global.pool, &ts, sizeof(ts), 1);
	mutex_spin_exit(&rnd_global.lock);
}

/*
 * Remove a source from our list of sources.
 */
void
rnd_detach_source(krndsource_t *source)
{
	rnd_sample_t *sample;

	mutex_spin_enter(&rnd_global.lock);
	LIST_REMOVE(source, list);
	if (0 < --source->refcnt) {
		do {
			cv_wait(&rnd_global.cv, &rnd_global.lock);
		} while (0 < source->refcnt);
	}
	mutex_spin_exit(&rnd_global.lock);

	/*
	 * If there are samples queued up "remove" them from the sample queue
	 * by setting the source to the no-collect pseudosource.
	 */
	mutex_spin_enter(&rnd_samples.lock);
	sample = SIMPLEQ_FIRST(&rnd_samples.q);
	while (sample != NULL) {
		if (sample->source == source)
			sample->source = &rnd_source_no_collect;

		sample = SIMPLEQ_NEXT(sample, next);
	}
	mutex_spin_exit(&rnd_samples.lock);

	if (source->state) {
		rnd_sample_free(source->state);
		source->state = NULL;
	}

	if (source->test) {
		kmem_free(source->test, sizeof(rngtest_t));
	}

	rnd_printf_verbose("rnd: %s detached as an entropy source\n",
	    source->name);
}

static inline uint32_t
rnd_estimate(krndsource_t *rs, uint32_t ts, uint32_t val)
{
	uint32_t entropy = 0, dt_est, dv_est;

	dt_est = rnd_dt_estimate(rs, ts);
	dv_est = rnd_dv_estimate(rs, val);

	if (!(rs->flags & RND_FLAG_NO_ESTIMATE)) {
		if (rs->flags & RND_FLAG_ESTIMATE_TIME) {
			entropy += dt_est;
		}

                if (rs->flags & RND_FLAG_ESTIMATE_VALUE) {
			entropy += dv_est;
		}

	}
	return entropy;
}

/*
 * Add a 32-bit value to the entropy pool.  The rs parameter should point to
 * the source-specific source structure.
 */
void
_rnd_add_uint32(krndsource_t *rs, uint32_t val)
{
	uint32_t ts;
	uint32_t entropy = 0;

	if (rs->flags & RND_FLAG_NO_COLLECT)
		return;

	/*
	 * Sample the counter as soon as possible to avoid
	 * entropy overestimation.
	 */
	ts = rnd_counter();

	/*
	 * Calculate estimates - we may not use them, but if we do
	 * not calculate them, the estimators' history becomes invalid.
	 */
	entropy = rnd_estimate(rs, ts, val);

	rnd_add_data_ts(rs, &val, sizeof(val), entropy, ts, true);
}

void
_rnd_add_uint64(krndsource_t *rs, uint64_t val)
{
	uint32_t ts;
	uint32_t entropy = 0;

	if (rs->flags & RND_FLAG_NO_COLLECT)
                return;

	/*
	 * Sample the counter as soon as possible to avoid
	 * entropy overestimation.
	 */
	ts = rnd_counter();

	/*
	 * Calculate estimates - we may not use them, but if we do
	 * not calculate them, the estimators' history becomes invalid.
	 */
	entropy = rnd_estimate(rs, ts, (uint32_t)(val & (uint64_t)0xffffffff));

	rnd_add_data_ts(rs, &val, sizeof(val), entropy, ts, true);
}

void
rnd_add_data(krndsource_t *rs, const void *const data, uint32_t len,
	     uint32_t entropy)
{

	/*
	 * This interface is meant for feeding data which is,
	 * itself, random.  Don't estimate entropy based on
	 * timestamp, just directly add the data.
	 */
	if (__predict_false(rs == NULL)) {
		mutex_spin_enter(&rnd_global.lock);
		rndpool_add_data(&rnd_global.pool, data, len, entropy);
		mutex_spin_exit(&rnd_global.lock);
	} else {
		rnd_add_data_ts(rs, data, len, entropy, rnd_counter(), true);
	}
}

void
rnd_add_data_sync(krndsource_t *rs, const void *data, uint32_t len,
    uint32_t entropy)
{

	KASSERT(rs != NULL);
	rnd_add_data_ts(rs, data, len, entropy, rnd_counter(), false);
}

static void
rnd_add_data_ts(krndsource_t *rs, const void *const data, uint32_t len,
    uint32_t entropy, uint32_t ts, bool schedule)
{
	rnd_sample_t *state = NULL;
	const uint8_t *p = data;
	uint32_t dint;
	int todo, done, filled = 0;
	int sample_count;
	struct rnd_sampleq tmp_samples = SIMPLEQ_HEAD_INITIALIZER(tmp_samples);

	if (rs &&
	    (rs->flags & RND_FLAG_NO_COLLECT ||
		__predict_false(!(rs->flags &
			(RND_FLAG_COLLECT_TIME|RND_FLAG_COLLECT_VALUE))))) {
		return;
	}
	todo = len / sizeof(dint);
	/*
	 * Let's try to be efficient: if we are warm, and a source
	 * is adding entropy at a rate of at least 1 bit every 10 seconds,
	 * mark it as "fast" and add its samples in bulk.
	 */
	if (__predict_true(rs->flags & RND_FLAG_FAST) ||
	    (todo >= RND_SAMPLE_COUNT)) {
		sample_count = RND_SAMPLE_COUNT;
	} else {
		if (!(rs->flags & RND_FLAG_HASCB) &&
		    !cold && rnd_initial_entropy) {
			struct timeval upt;

			getmicrouptime(&upt);
			if ((upt.tv_sec > 0  && rs->total > upt.tv_sec * 10) ||
			    (upt.tv_sec > 10 && rs->total > upt.tv_sec) ||
			    (upt.tv_sec > 100 &&
			      rs->total > upt.tv_sec / 10)) {
				rnd_printf_verbose("rnd: source %s is fast"
				    " (%d samples at once,"
				    " %d bits in %lld seconds), "
				    "processing samples in bulk.\n",
				    rs->name, todo, rs->total,
				    (long long int)upt.tv_sec);
				rs->flags |= RND_FLAG_FAST;
			}
		}
		sample_count = 2;
	}

	/*
	 * Loop over data packaging it into sample buffers.
	 * If a sample buffer allocation fails, drop all data.
	 */
	for (done = 0; done < todo ; done++) {
		state = rs->state;
		if (state == NULL) {
			state = rnd_sample_allocate_isr(rs);
			if (__predict_false(state == NULL)) {
				break;
			}
			rs->state = state;
		}

		state->ts[state->cursor] = ts;
		(void)memcpy(&dint, &p[done*4], 4);
		state->values[state->cursor] = dint;
		state->cursor++;

		if (state->cursor == sample_count) {
			SIMPLEQ_INSERT_HEAD(&tmp_samples, state, next);
			filled++;
			rs->state = NULL;
		}
	}

	if (__predict_false(state == NULL)) {
		while ((state = SIMPLEQ_FIRST(&tmp_samples))) {
			SIMPLEQ_REMOVE_HEAD(&tmp_samples, next);
			rnd_sample_free(state);
		}
		return;
	}

	/*
	 * Claim all the entropy on the last one we send to
	 * the pool, so we don't rely on it being evenly distributed
	 * in the supplied data.
	 *
	 * XXX The rndpool code must accept samples with more
	 * XXX claimed entropy than bits for this to work right.
	 */
	state->entropy += entropy;
	rs->total += entropy;

	/*
	 * If we didn't finish any sample buffers, we're done.
	 */
	if (!filled) {
		return;
	}

	mutex_spin_enter(&rnd_samples.lock);
	while ((state = SIMPLEQ_FIRST(&tmp_samples))) {
		SIMPLEQ_REMOVE_HEAD(&tmp_samples, next);
		SIMPLEQ_INSERT_HEAD(&rnd_samples.q, state, next);
	}
	mutex_spin_exit(&rnd_samples.lock);

	/* Cause processing of queued samples, if caller wants it.  */
	if (schedule)
		rnd_schedule_process();
}

static int
rnd_hwrng_test(rnd_sample_t *sample)
{
	krndsource_t *source = sample->source;
	size_t cmplen;
	uint8_t *v1, *v2;
	size_t resid, totest;

	KASSERT(source->type == RND_TYPE_RNG);

	/*
	 * Continuous-output test: compare two halves of the
	 * sample buffer to each other.  The sample buffer (64 ints,
	 * so either 256 or 512 bytes on any modern machine) should be
	 * much larger than a typical hardware RNG output, so this seems
	 * a reasonable way to do it without retaining extra data.
	 */
	cmplen = sizeof(sample->values) / 2;
	v1 = (uint8_t *)sample->values;
	v2 = (uint8_t *)sample->values + cmplen;

	if (__predict_false(!memcmp(v1, v2, cmplen))) {
		rnd_printf("rnd: source \"%s\""
		    " failed continuous-output test.\n",
		    source->name);
		return 1;
	}

	/*
	 * FIPS 140 statistical RNG test.  We must accumulate 20,000 bits.
	 */
	if (__predict_true(source->test_cnt == -1)) {
		/* already passed the test */
		return 0;
	}
	resid = FIPS140_RNG_TEST_BYTES - source->test_cnt;
	totest = MIN(RND_SAMPLE_COUNT * 4, resid);
	memcpy(source->test->rt_b + source->test_cnt, sample->values, totest);
	resid -= totest;
	source->test_cnt += totest;
	if (resid == 0) {
		strlcpy(source->test->rt_name, source->name,
			sizeof(source->test->rt_name));
		if (rngtest(source->test)) {
			rnd_printf("rnd: source \"%s\""
			    " failed statistical test.",
			    source->name);
			return 1;
		}
		source->test_cnt = -1;
		explicit_memset(source->test, 0, sizeof(*source->test));
	}
	return 0;
}

/*
 * Process the events in the ring buffer.  Called by rnd_timeout or
 * by the add routines directly if the callout has never fired (that
 * is, if we are "cold" -- just booted).
 *
 */
static void
rnd_process_events(void)
{
	rnd_sample_t *sample = NULL;
	krndsource_t *source;
	static krndsource_t *last_source;
	uint32_t entropy;
	size_t pool_entropy;
	int wake = 0;
	struct rnd_sampleq dq_samples = SIMPLEQ_HEAD_INITIALIZER(dq_samples);
	struct rnd_sampleq df_samples = SIMPLEQ_HEAD_INITIALIZER(df_samples);

	/*
	 * Drain to the on-stack queue and drop the lock.
	 */
	mutex_spin_enter(&rnd_samples.lock);
	while ((sample = SIMPLEQ_FIRST(&rnd_samples.q))) {
		SIMPLEQ_REMOVE_HEAD(&rnd_samples.q, next);
		/*
		 * We repeat this check here, since it is possible
		 * the source was disabled before we were called, but
		 * after the entry was queued.
		 */
		if (__predict_false(!(sample->source->flags &
			    (RND_FLAG_COLLECT_TIME|RND_FLAG_COLLECT_VALUE)))) {
			SIMPLEQ_INSERT_TAIL(&df_samples, sample, next);
		} else {
			SIMPLEQ_INSERT_TAIL(&dq_samples, sample, next);
		}
	}
	mutex_spin_exit(&rnd_samples.lock);

	/* Don't thrash the rndpool mtx either.  Hold, add all samples. */
	mutex_spin_enter(&rnd_global.lock);

	pool_entropy = rndpool_get_entropy_count(&rnd_global.pool);

	while ((sample = SIMPLEQ_FIRST(&dq_samples))) {
		int sample_count;

		SIMPLEQ_REMOVE_HEAD(&dq_samples, next);
		source = sample->source;
		entropy = sample->entropy;
		sample_count = sample->cursor;

		/*
		 * Don't provide a side channel for timing attacks on
		 * low-rate sources: require mixing with some other
		 * source before we schedule a wakeup.
		 */
		if (!wake &&
		    (source != last_source || source->flags & RND_FLAG_FAST)) {
			wake++;
		}
		last_source = source;

		/*
		 * If the source has been disabled, ignore samples from
		 * it.
		 */
		if (source->flags & RND_FLAG_NO_COLLECT)
			goto skip;

		/*
		 * Hardware generators are great but sometimes they
		 * have...hardware issues.  Don't use any data from
		 * them unless it passes some tests.
		 */
		if (source->type == RND_TYPE_RNG) {
			if (__predict_false(rnd_hwrng_test(sample))) {
				source->flags |= RND_FLAG_NO_COLLECT;
				rnd_printf("rnd: disabling source \"%s\".\n",
				    source->name);
				goto skip;
			}
		}

		if (source->flags & RND_FLAG_COLLECT_VALUE) {
			rndpool_add_data(&rnd_global.pool, sample->values,
			    sample_count * sizeof(sample->values[1]),
			    0);
		}
		if (source->flags & RND_FLAG_COLLECT_TIME) {
			rndpool_add_data(&rnd_global.pool, sample->ts,
			    sample_count * sizeof(sample->ts[1]),
			    0);
		}

		pool_entropy += entropy;
		source->total += sample->entropy;
skip:		SIMPLEQ_INSERT_TAIL(&df_samples, sample, next);
	}
	rndpool_set_entropy_count(&rnd_global.pool, pool_entropy);
	rnd_entropy_added();
	mutex_spin_exit(&rnd_global.lock);

	/*
	 * If we filled the pool past the threshold, wake anyone
	 * waiting for entropy.
	 */
	if (pool_entropy > RND_ENTROPY_THRESHOLD * 8) {
		wake++;
	}

	/* Now we hold no locks: clean up. */
	while ((sample = SIMPLEQ_FIRST(&df_samples))) {
		SIMPLEQ_REMOVE_HEAD(&df_samples, next);
		rnd_sample_free(sample);
	}

	/*
	 * Wake up any potential readers waiting.
	 */
	if (wake) {
		rnd_schedule_wakeup();
	}
}

static void
rnd_intr(void *arg)
{

	rnd_process_events();
}

static void
rnd_wake(void *arg)
{

	rndsinks_distribute();
}

static uint32_t
rnd_extract_data(void *p, uint32_t len, uint32_t flags)
{
	static int timed_in;
	uint32_t retval;

	mutex_spin_enter(&rnd_global.lock);
	if (__predict_false(!timed_in)) {
		if (boottime.tv_sec) {
			rndpool_add_data(&rnd_global.pool, &boottime,
			    sizeof(boottime), 0);
		}
		timed_in++;
	}
	if (__predict_false(!rnd_initial_entropy)) {
		uint32_t c;

		rnd_printf_verbose("rnd: WARNING! initial entropy low (%u).\n",
		    rndpool_get_entropy_count(&rnd_global.pool));
		/* Try once again to put something in the pool */
		c = rnd_counter();
		rndpool_add_data(&rnd_global.pool, &c, sizeof(c), 1);
	}

#ifdef DIAGNOSTIC
	while (!rnd_tested) {
		int entropy_count =
		    rndpool_get_entropy_count(&rnd_global.pool);
		rnd_printf_verbose("rnd: starting statistical RNG test,"
		    " entropy = %d.\n",
		    entropy_count);
		if (rndpool_extract_data(&rnd_global.pool, rnd_rt.rt_b,
			sizeof(rnd_rt.rt_b), RND_EXTRACT_ANY)
		    != sizeof(rnd_rt.rt_b)) {
			panic("rnd: could not get bits for statistical test");
		}
		/*
		 * Stash the tested bits so we can put them back in the
		 * pool, restoring the entropy count.  DO NOT rely on
		 * rngtest to maintain the bits pristine -- we could end
		 * up adding back non-random data claiming it were pure
		 * entropy.
		 */
		memcpy(rnd_testbits, rnd_rt.rt_b, sizeof(rnd_rt.rt_b));
		strlcpy(rnd_rt.rt_name, "entropy pool",
		    sizeof(rnd_rt.rt_name));
		if (rngtest(&rnd_rt)) {
			/*
			 * The probabiliity of a Type I error is 3/10000,
			 * but note this can only happen at boot time.
			 * The relevant standard says to reset the module,
			 * but developers objected...
			 */
			rnd_printf("rnd: WARNING, ENTROPY POOL FAILED "
			    "STATISTICAL TEST!\n");
			continue;
		}
		explicit_memset(&rnd_rt, 0, sizeof(rnd_rt));
		rndpool_add_data(&rnd_global.pool, rnd_testbits,
		    sizeof(rnd_testbits), entropy_count);
		explicit_memset(rnd_testbits, 0, sizeof(rnd_testbits));
		rnd_printf_verbose("rnd: statistical RNG test done,"
		    " entropy = %d.\n",
		    rndpool_get_entropy_count(&rnd_global.pool));
		rnd_tested++;
	}
#endif
	retval = rndpool_extract_data(&rnd_global.pool, p, len, flags);
	mutex_spin_exit(&rnd_global.lock);

	return retval;
}

/*
 * Fill the buffer with as much entropy as we can.  Return true if it
 * has full entropy and false if not.
 */
bool
rnd_extract(void *buffer, size_t bytes)
{
	const size_t extracted = rnd_extract_data(buffer, bytes,
	    RND_EXTRACT_GOOD);

	if (extracted < bytes) {
		rnd_getmore(bytes - extracted);
		(void)rnd_extract_data((uint8_t *)buffer + extracted,
		    bytes - extracted, RND_EXTRACT_ANY);
		return false;
	}

	return true;
}

/*
 * If we have as much entropy as is requested, fill the buffer with it
 * and return true.  Otherwise, leave the buffer alone and return
 * false.
 */

CTASSERT(RND_ENTROPY_THRESHOLD <= 0xffffffffUL);
CTASSERT(RNDSINK_MAX_BYTES <= (0xffffffffUL - RND_ENTROPY_THRESHOLD));
CTASSERT((RNDSINK_MAX_BYTES + RND_ENTROPY_THRESHOLD) <=
	    (0xffffffffUL / NBBY));

bool
rnd_tryextract(void *buffer, size_t bytes)
{
	uint32_t bits_needed, bytes_requested;

	KASSERT(bytes <= RNDSINK_MAX_BYTES);
	bits_needed = ((bytes + RND_ENTROPY_THRESHOLD) * NBBY);

	mutex_spin_enter(&rnd_global.lock);
	if (bits_needed <= rndpool_get_entropy_count(&rnd_global.pool)) {
		const uint32_t extracted __diagused =
		    rndpool_extract_data(&rnd_global.pool, buffer, bytes,
			RND_EXTRACT_GOOD);

		KASSERT(extracted == bytes);
		bytes_requested = 0;
	} else {
		/* XXX Figure the threshold into this...  */
		bytes_requested = howmany((bits_needed -
			rndpool_get_entropy_count(&rnd_global.pool)), NBBY);
		KASSERT(0 < bytes_requested);
	}
	mutex_spin_exit(&rnd_global.lock);

	if (0 < bytes_requested)
		rnd_getmore(bytes_requested);

	return bytes_requested == 0;
}

void
rnd_seed(void *base, size_t len)
{
	SHA1_CTX s;
	uint8_t digest[SHA1_DIGEST_LENGTH];

	if (len != sizeof(*boot_rsp)) {
		rnd_printf("rnd: bad seed length %d\n", (int)len);
		return;
	}

	boot_rsp = (rndsave_t *)base;
	SHA1Init(&s);
	SHA1Update(&s, (uint8_t *)&boot_rsp->entropy,
	    sizeof(boot_rsp->entropy));
	SHA1Update(&s, boot_rsp->data, sizeof(boot_rsp->data));
	SHA1Final(digest, &s);

	if (memcmp(digest, boot_rsp->digest, sizeof(digest))) {
		rnd_printf("rnd: bad seed checksum\n");
		return;
	}

	/*
	 * It's not really well-defined whether bootloader-supplied
	 * modules run before or after rnd_init().  Handle both cases.
	 */
	if (rnd_ready) {
		rnd_printf_verbose("rnd: ready,"
		    " feeding in seed data directly.\n");
		mutex_spin_enter(&rnd_global.lock);
		rndpool_add_data(&rnd_global.pool, boot_rsp->data,
		    sizeof(boot_rsp->data),
		    MIN(boot_rsp->entropy, RND_POOLBITS / 2));
		explicit_memset(boot_rsp, 0, sizeof(*boot_rsp));
		mutex_spin_exit(&rnd_global.lock);
	} else {
		rnd_printf_verbose("rnd: not ready, deferring seed feed.\n");
	}
}

static void
krndsource_to_rndsource(krndsource_t *kr, rndsource_t *r)
{

	memset(r, 0, sizeof(*r));
	strlcpy(r->name, kr->name, sizeof(r->name));
        r->total = kr->total;
        r->type = kr->type;
        r->flags = kr->flags;
}

static void
krndsource_to_rndsource_est(krndsource_t *kr, rndsource_est_t *re)
{

	memset(re, 0, sizeof(*re));
	krndsource_to_rndsource(kr, &re->rt);
	re->dt_samples = kr->time_delta.insamples;
	re->dt_total = kr->time_delta.outbits;
	re->dv_samples = kr->value_delta.insamples;
	re->dv_total = kr->value_delta.outbits;
}

static void
krs_setflags(krndsource_t *kr, uint32_t flags, uint32_t mask)
{
	uint32_t oflags = kr->flags;

	kr->flags &= ~mask;
	kr->flags |= (flags & mask);

	if (oflags & RND_FLAG_HASENABLE &&
            ((oflags & RND_FLAG_NO_COLLECT) !=
		(flags & RND_FLAG_NO_COLLECT))) {
		kr->enable(kr, !(flags & RND_FLAG_NO_COLLECT));
	}
}

int
rnd_system_ioctl(struct file *fp, u_long cmd, void *addr)
{
	krndsource_t *kr;
	rndstat_t *rst;
	rndstat_name_t *rstnm;
	rndstat_est_t *rset;
	rndstat_est_name_t *rsetnm;
	rndctl_t *rctl;
	rnddata_t *rnddata;
	uint32_t count, start;
	int ret = 0;
	int estimate_ok = 0, estimate = 0;

	switch (cmd) {
	case RNDGETENTCNT:
		break;

	case RNDGETPOOLSTAT:
	case RNDGETSRCNUM:
	case RNDGETSRCNAME:
	case RNDGETESTNUM:
	case RNDGETESTNAME:
		ret = kauth_authorize_device(curlwp->l_cred,
		    KAUTH_DEVICE_RND_GETPRIV, NULL, NULL, NULL, NULL);
		if (ret)
			return ret;
		break;

	case RNDCTL:
		ret = kauth_authorize_device(curlwp->l_cred,
		    KAUTH_DEVICE_RND_SETPRIV, NULL, NULL, NULL, NULL);
		if (ret)
			return ret;
		break;

	case RNDADDDATA:
		ret = kauth_authorize_device(curlwp->l_cred,
		    KAUTH_DEVICE_RND_ADDDATA, NULL, NULL, NULL, NULL);
		if (ret)
			return ret;
		estimate_ok = !kauth_authorize_device(curlwp->l_cred,
		    KAUTH_DEVICE_RND_ADDDATA_ESTIMATE, NULL, NULL, NULL, NULL);
		break;

	default:
#ifdef COMPAT_50
		return compat_50_rnd_ioctl(fp, cmd, addr);
#else
		return ENOTTY;
#endif
	}

	switch (cmd) {
	case RNDGETENTCNT:
		mutex_spin_enter(&rnd_global.lock);
		*(uint32_t *)addr =
		    rndpool_get_entropy_count(&rnd_global.pool);
		mutex_spin_exit(&rnd_global.lock);
		break;

	case RNDGETPOOLSTAT:
		mutex_spin_enter(&rnd_global.lock);
		rndpool_get_stats(&rnd_global.pool, addr,
		    sizeof(rndpoolstat_t));
		mutex_spin_exit(&rnd_global.lock);
		break;

	case RNDGETSRCNUM:
		rst = (rndstat_t *)addr;

		if (rst->count == 0)
			break;

		if (rst->count > RND_MAXSTATCOUNT)
			return EINVAL;

		mutex_spin_enter(&rnd_global.lock);
		/*
		 * Find the starting source by running through the
		 * list of sources.
		 */
		kr = LIST_FIRST(&rnd_global.sources);
		start = rst->start;
		while (kr != NULL && start >= 1) {
			kr = LIST_NEXT(kr, list);
			start--;
		}

		/*
		 * Return up to as many structures as the user asked
		 * for.  If we run out of sources, a count of zero
		 * will be returned, without an error.
		 */
		for (count = 0; count < rst->count && kr != NULL; count++) {
			krndsource_to_rndsource(kr, &rst->source[count]);
			kr = LIST_NEXT(kr, list);
		}

		rst->count = count;

		mutex_spin_exit(&rnd_global.lock);
		break;

	case RNDGETESTNUM:
		rset = (rndstat_est_t *)addr;

		if (rset->count == 0)
			break;

		if (rset->count > RND_MAXSTATCOUNT)
			return EINVAL;

		mutex_spin_enter(&rnd_global.lock);
		/*
		 * Find the starting source by running through the
		 * list of sources.
		 */
		kr = LIST_FIRST(&rnd_global.sources);
		start = rset->start;
		while (kr != NULL && start > 0) {
			kr = LIST_NEXT(kr, list);
			start--;
		}

		/*
		 * Return up to as many structures as the user asked
		 * for.  If we run out of sources, a count of zero
		 * will be returned, without an error.
		 */
		for (count = 0; count < rset->count && kr != NULL; count++) {
			krndsource_to_rndsource_est(kr, &rset->source[count]);
			kr = LIST_NEXT(kr, list);
		}

		rset->count = count;

		mutex_spin_exit(&rnd_global.lock);
		break;

	case RNDGETSRCNAME:
		/*
		 * Scan through the list, trying to find the name.
		 */
		mutex_spin_enter(&rnd_global.lock);
		rstnm = (rndstat_name_t *)addr;
		kr = LIST_FIRST(&rnd_global.sources);
		while (kr != NULL) {
			if (strncmp(kr->name, rstnm->name,
				MIN(sizeof(kr->name),
				    sizeof(rstnm->name))) == 0) {
				krndsource_to_rndsource(kr, &rstnm->source);
				mutex_spin_exit(&rnd_global.lock);
				return 0;
			}
			kr = LIST_NEXT(kr, list);
		}
		mutex_spin_exit(&rnd_global.lock);

		ret = ENOENT;		/* name not found */

		break;

	case RNDGETESTNAME:
		/*
		 * Scan through the list, trying to find the name.
		 */
		mutex_spin_enter(&rnd_global.lock);
		rsetnm = (rndstat_est_name_t *)addr;
		kr = LIST_FIRST(&rnd_global.sources);
		while (kr != NULL) {
			if (strncmp(kr->name, rsetnm->name,
				MIN(sizeof(kr->name), sizeof(rsetnm->name)))
			    == 0) {
				krndsource_to_rndsource_est(kr,
				    &rsetnm->source);
				mutex_spin_exit(&rnd_global.lock);
				return 0;
			}
			kr = LIST_NEXT(kr, list);
		}
		mutex_spin_exit(&rnd_global.lock);

		ret = ENOENT;           /* name not found */

		break;

	case RNDCTL:
		/*
		 * Set flags to enable/disable entropy counting and/or
		 * collection.
		 */
		mutex_spin_enter(&rnd_global.lock);
		rctl = (rndctl_t *)addr;
		kr = LIST_FIRST(&rnd_global.sources);

		/*
		 * Flags set apply to all sources of this type.
		 */
		if (rctl->type != 0xff) {
			while (kr != NULL) {
				if (kr->type == rctl->type) {
					krs_setflags(kr, rctl->flags,
					    rctl->mask);
				}
				kr = LIST_NEXT(kr, list);
			}
			mutex_spin_exit(&rnd_global.lock);
			return 0;
		}

		/*
		 * scan through the list, trying to find the name
		 */
		while (kr != NULL) {
			if (strncmp(kr->name, rctl->name,
				MIN(sizeof(kr->name), sizeof(rctl->name)))
			    == 0) {
				krs_setflags(kr, rctl->flags, rctl->mask);
				mutex_spin_exit(&rnd_global.lock);
				return 0;
			}
			kr = LIST_NEXT(kr, list);
		}

		mutex_spin_exit(&rnd_global.lock);
		ret = ENOENT;		/* name not found */

		break;

	case RNDADDDATA:
		/*
		 * Don't seed twice if our bootloader has
		 * seed loading support.
		 */
		if (!boot_rsp) {
			rnddata = (rnddata_t *)addr;

			if (rnddata->len > sizeof(rnddata->data))
				return EINVAL;

			if (estimate_ok) {
				/*
				 * Do not accept absurd entropy estimates, and
				 * do not flood the pool with entropy such that
				 * new samples are discarded henceforth.
				 */
				estimate = MIN((rnddata->len * NBBY) / 2,
				    MIN(rnddata->entropy, RND_POOLBITS / 2));
			} else {
				estimate = 0;
			}

			mutex_spin_enter(&rnd_global.lock);
			rndpool_add_data(&rnd_global.pool, rnddata->data,
			    rnddata->len, estimate);
			rnd_entropy_added();
			mutex_spin_exit(&rnd_global.lock);

			rndsinks_distribute();
		} else {
			rnd_printf_verbose("rnd"
			    ": already seeded by boot loader\n");
		}
		break;

	default:
		return ENOTTY;
	}

	return ret;
}
