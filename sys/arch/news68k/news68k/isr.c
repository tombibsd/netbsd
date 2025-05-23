/*	$NetBSD$	*/

/*-
 * Copyright (c) 1996 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Adam Glass, Gordon W. Ross, and Jason R. Thorpe.
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

/*
 * from mvme68k/mvme68k/isr.c and sun3/sun3/isr.c
 * This should be in /sys/arch/m68k/m68k?
 */

/*
 * Link and dispatch interrupts.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/cpu.h>
#include <sys/intr.h>

#include <uvm/uvm_extern.h>

#include <news68k/news68k/isr.h>

isr_autovec_list_t isr_autovec[NISRAUTOVEC];
struct	isr_vectored isr_vectored[NISRVECTORED];
int idepth;

void set_vector_entry(int, void *);
void *get_vector_entry(int);

void
isrinit(void)
{
	int i;

	/* Initialize the autovector lists. */
	for (i = 0; i < NISRAUTOVEC; ++i) {
		LIST_INIT(&isr_autovec[i]);
	}
}

/*
 * Establish an autovectored interrupt handler.
 * Called by driver attach functions.
 */
void
isrlink_autovec(int (*func)(void *), void *arg, int ipl, int priority)
{
	struct isr_autovec *newisr, *curisr;
	isr_autovec_list_t *list;

	if ((ipl < 0) || (ipl >= NISRAUTOVEC))
		panic("isrlink_autovec: bad ipl %d", ipl);

	newisr = (struct isr_autovec *)malloc(sizeof(struct isr_autovec),
	    M_DEVBUF, M_NOWAIT);
	if (newisr == NULL)
		panic("isrlink_autovec: can't allocate space for isr");

	/* Fill in the new entry. */
	newisr->isr_func = func;
	newisr->isr_arg = arg;
	newisr->isr_ipl = ipl;
	newisr->isr_priority = priority;

	/*
	 * Some devices are particularly sensitive to interrupt
	 * handling latency.  The SCC, for example, can lose many
	 * characters if its interrupt isn't handled with reasonable
	 * speed.
	 *
	 * To work around this problem, each device can give itself a
	 * "priority".  An unbuffered SCC would give itself a higher
	 * priority than a SCSI device, for example.
	 *
	 * This solution was originally developed for the hp300, which
	 * has a flat spl scheme (by necessity).  Thankfully, the
	 * MVME systems don't have this problem, though this may serve
	 * a useful purpose in any case.
	 */

	/*
	 * Get the appropriate ISR list.  If the list is empty, no
	 * additional work is necessary; we simply insert ourselves
	 * at the head of the list.
	 */
	list = &isr_autovec[ipl];
	if (LIST_EMPTY(list)) {
		LIST_INSERT_HEAD(list, newisr, isr_link);
		return;
	}

	/*
	 * A little extra work is required.  We traverse the list
	 * and place ourselves after any ISRs with our current (or
	 * higher) priority.
	 */
	for (curisr = LIST_FIRST(list); LIST_NEXT(curisr, isr_link) != NULL;
	    curisr = LIST_NEXT(curisr, isr_link)) {
		if (newisr->isr_priority > curisr->isr_priority) {
			LIST_INSERT_BEFORE(curisr, newisr, isr_link);
			return;
		}
	}

	/*
	 * We're the least important entry, it seems.  We just go
	 * on the end.
	 */
	LIST_INSERT_AFTER(curisr, newisr, isr_link);
}

/*
 * Establish a vectored interrupt handler.
 * Called by bus interrupt establish functions.
 */
void
isrlink_vectored(int (*func)(void *), void *arg, int ipl, int vec)
{
	struct isr_vectored *isr;

	if ((ipl < 0) || (ipl >= NISRAUTOVEC))
		panic("isrlink_vectored: bad ipl %d", ipl);
	if ((vec < ISRVECTORED) || (vec >= ISRVECTORED + NISRVECTORED))
		panic("isrlink_vectored: bad vec 0x%x", vec);

	isr = &isr_vectored[vec - ISRVECTORED];

	if ((vectab[vec] != badtrap) || (isr->isr_func != NULL))
		panic("isrlink_vectored: vec 0x%x not available", vec);

	/* Fill in the new entry. */
	isr->isr_func = func;
	isr->isr_arg = arg;
	isr->isr_ipl = ipl;

	/* Hook into the vector table. */
	vectab[vec] = intrhand_vectored;
}

/*
 * Unhook a vectored interrupt.
 */
void
isrunlink_vectored(int vec)
{

	if ((vec < ISRVECTORED) || (vec >= ISRVECTORED + NISRVECTORED))
		panic("isrunlink_vectored: bad vec 0x%x", vec);

	if (vectab[vec] != intrhand_vectored)
		panic("isrunlink_vectored: not vectored interrupt");

	vectab[vec] = badtrap;
	memset(&isr_vectored[vec - ISRVECTORED], 0,
	    sizeof(struct isr_vectored));
}

/*
 * This is the dispatcher called by the low-level
 * assembly language autovectored interrupt routine.
 */
void
isrdispatch_autovec(int evec)
{
	struct isr_autovec *isr;
	isr_autovec_list_t *list;
	int handled = 0, ipl, vec;
	static int straycount, unexpected;

	vec = (evec & 0xfff) >> 2;
	if ((vec < ISRAUTOVEC) || (vec >= (ISRAUTOVEC + NISRAUTOVEC)))
		panic("isrdispatch_autovec: bad vec 0x%x", vec);
	ipl = vec - ISRAUTOVEC;

	intrcnt[ipl]++;
	curcpu()->ci_data.cpu_nintr++;

	list = &isr_autovec[ipl];
	if (LIST_EMPTY(list)) {
		printf("isrdispatch_autovec: ipl %d unexpected\n", ipl);
		if (++unexpected > 10)
			panic("too many unexpected interrupts");
		return;
	}

	/* Give all the handlers a chance. */
	for (isr = LIST_FIRST(list); isr != NULL;
	    isr = LIST_NEXT(isr, isr_link))
		handled |= (*isr->isr_func)(isr->isr_arg);

	if (handled)
		straycount = 0;
	else if (++straycount > 50)
		panic("isr_dispatch_autovec: too many stray interrupts");
	else
		printf("isrdispatch_autovec: stray level %d interrupt\n", ipl);
}

/*
 * This is the dispatcher called by the low-level
 * assembly language vectored interrupt routine.
 */
void
isrdispatch_vectored(int pc, int evec, void *frame)
{
	struct isr_vectored *isr;
	int ipl, vec;

	vec = (evec & 0xfff) >> 2;
	ipl = (getsr() >> 8) & 7;

	intrcnt[ipl]++;
	curcpu()->ci_data.cpu_nintr++;

	if ((vec < ISRVECTORED) || (vec >= (ISRVECTORED + NISRVECTORED)))
		panic("isrdispatch_vectored: bad vec 0x%x", vec);
	isr = &isr_vectored[vec - ISRVECTORED];

	if (isr->isr_func == NULL) {
		printf("isrdispatch_vectored: no handler for vec 0x%x\n", vec);
		vectab[vec] = badtrap;
		return;
	}

	/*
	 * Handler gets exception frame if argument is NULL.
	 */
	if ((*isr->isr_func)(isr->isr_arg ? isr->isr_arg : frame) == 0)
		printf("isrdispatch_vectored: vec 0x%x not claimed\n", vec);
}

void
isrlink_custom(int level, void *handler)
{

	set_vector_entry(ISRAUTOVEC + level, handler);
}

/*
 * XXX - could just kill these... [from sun3]
 */
void
set_vector_entry(int entry, void *handler)
{

	if ((entry < 0) || (entry >= NVECTORS))
		panic("set_vector_entry: setting vector too high or low");
	vectab[entry] = handler;
}

void *
get_vector_entry(int entry)
{

	if ((entry < 0) || (entry >= NVECTORS))
		panic("get_vector_entry: setting vector too high or low");
	return (void *)vectab[entry];
}

const uint16_t ipl2psl_table[NIPL] = {
	[IPL_NONE]       = PSL_S | PSL_IPL0,
	[IPL_SOFTCLOCK]  = PSL_S | PSL_IPL2,
	[IPL_SOFTBIO]    = PSL_S | PSL_IPL2,
	[IPL_SOFTNET]    = PSL_S | PSL_IPL2,
	[IPL_SOFTSERIAL] = PSL_S | PSL_IPL2,
	[IPL_VM]         = PSL_S | PSL_IPL5,
	[IPL_SCHED]      = PSL_S | PSL_IPL7,
	[IPL_HIGH]       = PSL_S | PSL_IPL7,
};
