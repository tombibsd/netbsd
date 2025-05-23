/*
 * Copyright (c) 1989, 1993
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
	/* .asciz "@(#)sigprocmask.s	8.1 (Berkeley) 6/4/93" */
RCSID("$NetBSD$")
#endif /* SYSLIBC_SCCS and not lint */

WARN_REFERENCES(sigprocmask, \
    "warning: reference to compatibility sigprocmask(); include <signal.h> for correct reference")

ENTRY(sigprocmask, 0)
	tstl	8(%ap)			# check new sigset pointer
	bneq	1f			# if not null, indirect
/*	movl	$0,8(%ap)		# null mask pointer: block empty set */
	movl	$1,4(%ap)		# SIG_BLOCK
	jbr	2f
1:	movl	*8(%ap),8(%ap)		# indirect to new mask arg
2:	chmk	$ SYS_compat_13_sigprocmask13
	jcc	3f
	jmp	CERROR+2

3:	tstl	12(%ap)			# test if old mask requested
	beql	out
	movl	%r0,*12(%ap)		# store old mask
out:
	clrl	%r0
	ret
END(sigprocmask)
