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
 * from: Header: brk.s,v 1.3 92/06/25 12:56:05 mccanne Exp
 */

#include <machine/asm.h>
#if defined(SYSLIBC_SCCS) && !defined(lint)
#if 0
	.asciz "@(#)brk.s	8.1 (Berkeley) 6/4/93"
#else
	RCSID("$NetBSD$")
#endif
#endif /* SYSLIBC_SCCS and not lint */

#include "SYS.h"

	.globl	_C_LABEL(__curbrk)
	.globl	_C_LABEL(__minbrk)
	.globl	_C_LABEL(_end)

#ifdef WEAK_ALIAS
WEAK_ALIAS(brk, _brk)
#endif

	.data
	.align	8
_C_LABEL(__minbrk):
	.xword	_C_LABEL(_end)		/* lower brk limit; also for gmon code */
	.text

#if __PIC__ - 0 >= 2

ENTRY(_brk)
	PIC_PROLOGUE(%o5,%o4)
	set	_C_LABEL(__minbrk), %o4
	ldx	[%o5 + %o4], %o4
	ldx	[%o4], %o1		/* %o1 = minbrk */
	cmp	%o1, %o0		/* if (minbrk > %o0) */
	movgu	%xcc, %o1, %o0		/*	%o0 = minbrk */
	mov	%o0, %o2		/* save argument to syscall */
	mov	SYS_break, %g1
	t	ST_SYSCALL
	set	_C_LABEL(__curbrk), %o3
	bcc,a,pt	%icc, 1f
	 ldx	[%o5 + %o3], %o4
	ERROR()
1:
	retl				/* success, return 0 & record new break */
	 stx	%o2, [%o4]

#elif __PIC__ - 0 >= 1

ENTRY(_brk)
	PIC_PROLOGUE(%o5,%o4)
	ldx	[%o5 + _C_LABEL(__minbrk)], %o4
	ldx	[%o4], %o1		/* %o1 = minbrk */
	cmp	%o1, %o0		/* if (minbrk > %o0) */
	movgu	%xcc, %o1, %o0		/*	%o0 = minbrk */
	mov	%o0, %o2		/* save argument to syscall */
	mov	SYS_break, %g1
	t	ST_SYSCALL
	bcc,a,pt	%icc, 1f
	 ldx	[%o5 + _C_LABEL(__curbrk)], %o4
	ERROR()
1:
	retl				/* success, return 0 & record new break */
	 stx	%o2, [%o4]

#else

ENTRY(_brk)
	sethi	%hi(_C_LABEL(__minbrk)), %o1	/* %o1 = minbrk */
	ldx	[%o1 + %lo(_C_LABEL(__minbrk))], %o1
	cmp	%o1, %o0		/* if (minbrk > %o0) */
	movgu	%xcc, %o1, %o0		/*	%o0 = minbrk */
	mov	%o0, %o2		/* save argument to syscall */
	mov	SYS_break, %g1
	t	ST_SYSCALL
	bcc,a,pt	%icc,1f
	 sethi	%hi(_C_LABEL(__curbrk)), %g1
	ERROR()
1:
	retl				/* success, return 0 & record new break */
	 stx	%o2, [%g1 + %lo(_C_LABEL(__curbrk))]

#endif
