/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 */

#include "SYS.h"

#if defined(SYSLIBC_SCCS) && !defined(lint)
	/* .asciz "@(#)Ovfork.s	8.1 (Berkeley) 6/4/93" */
RCSID("$NetBSD$")
#endif /* SYSLIBC_SCCS and not lint */

/*
 * @(#)vfork.s	4.1 (Berkeley) 12/21/80
 * C library -- vfork
 */

/*
 * pid = vfork();
 *
 * %r1 == 0 in parent process, %r1 == 1 in child process.
 * %r0 == pid of child in parent, %r0 == pid of parent in child.
 *
 * trickery here, due to keith sklower, uses ret to clear the stack,
 * and then returns with a jump indirect, since only one person can return
 * with a ret off this stack... we do the ret before we vfork!
 */

ENTRY(__vfork14, 0)
	movl	16(%fp),%r0	# save return address before we smash it
	movab	here,16(%fp)
	ret
here:
	movl	%r0,%r2
	chmk	$ SYS___vfork14
	bcs	err		# if failed, set errno and return -1
	/* this next trick is Chris Torek's fault */
	mnegl	%r1,%r1		# %r1 = 0xffffffff if child, 0 if parent
	bicl2	%r1,%r0		# %r0 &= ~%r1, i.e., 0 if child, else unchanged
	jmp	(%r2)

err:
#ifdef _REENTRANT
	pushr	$0x5
	calls	$0,_C_LABEL(__errno)
	movl	(%sp)+,(%r0)
	mnegl	$1,%r0
	rsb
#else
	movl	%r0,_C_LABEL(errno)
	mnegl	$1,%r0
	jmp	(%r2)
#endif
END(__vfork14)
