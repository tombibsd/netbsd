/*	$NetBSD$	*/

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * from: Header: sigprocmask.s,v 1.1 91/07/06 13:06:01 torek Exp
 */

#include <machine/asm.h>
#if defined(SYSLIBC_SCCS) && !defined(lint)
#if 0
	.asciz "@(#)sigprocmask.s	8.1 (Berkeley) 6/4/93"
#else
	RCSID("$NetBSD$")
#endif
#endif /* SYSLIBC_SCCS and not lint */

#include "SYS.h"

WARN_REFERENCES(sigprocmask, \
    "warning: reference to compatibility sigprocmask(); include <signal.h> for correct reference")

/*
 * sigprocmask(int how, sigset_t *set, sigset_t *oset)
 */
ENTRY(sigprocmask)
	brnz,a	%o1, 1f		/* if (set != NULL) */
	 ld	[%o1], %o1	/*    replace it in %o1 with *set */
#if 0
	clr	%o1		/* else block no signals ... */
#endif
	mov	1, %o0		/* ... using sigprocmask(SIG_BLOCK) */
1:
	mov	SYS_compat_13_sigprocmask13, %g1
	t	ST_SYSCALL
	bcc	2f		/* if success, */
	 tst	%o2		/*    check to see if oset requested */
	ERROR()
2:
	bne,a	3f		/* if oset != NULL, */
	 st	%o0, [%o2]	/*    *oset = oldmask */
3:
	retl			/* in any case, return 0 */
	 clr	%o0
