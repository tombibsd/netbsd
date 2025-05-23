/*	$NetBSD$	*/

/*-
 * Copyright (c) 1998, 2007, 2009 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center, and by Andrew Doran.
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

#ifndef _SYS_KTHREAD_H_
#define	_SYS_KTHREAD_H_

#if !defined(_KERNEL)
#error "not supposed to be exposed to userland"
#endif

/*
 * Kernel thread handling.
 */

#include <sys/proc.h>

#define	KTHREAD_IDLE		0x01	/* Do not run on creation */
#define	KTHREAD_MPSAFE		0x02	/* Do not acquire kernel_lock */
#define	KTHREAD_INTR		0x04	/* Software interrupt handler */
#define	KTHREAD_TS		0x08	/* Time-sharing priority range */
#define	KTHREAD_MUSTJOIN	0x10	/* Must join on exit */

void	kthread_sysinit(void);

int	kthread_create(pri_t, int, struct cpu_info *,
    void (*)(void *), void *, lwp_t **, const char *, ...) __printflike(7, 8);
void	kthread_exit(int) __dead;
int	kthread_join(lwp_t *);

#endif /* _SYS_KTHREAD_H_ */
