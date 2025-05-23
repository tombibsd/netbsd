/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Donn Seeley at UUNET Technologies, Inc.
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

#include <machine/asm.h>

	/* .asciz "@(#)udiv.s	8.1 (Berkeley) 6/4/93" */
RCSID("$NetBSD$")

/*
 * Unsigned division, PCC flavor.
 * udiv() takes an ordinary dividend/divisor pair;
 * audiv() takes a pointer to a dividend and an ordinary divisor.
 */

#define	DIVIDEND	4(%ap)
#define	DIVISOR		8(%ap)

ASENTRY(__udiv,0)
	movl	DIVISOR,%r2
	jlss	Leasy		# big divisor: settle by comparison
	movl	DIVIDEND,%r0
	jlss	Lhard		# big dividend: extended division
	divl2	%r2,%r0		# small divisor and dividend: signed division
	ret
Lhard:
	clrl	%r1
	ediv	%r2,%r0,%r0,%r1
	ret
Leasy:
	cmpl	DIVIDEND,%r2
	jgequ	Lone		# if dividend is as big or bigger, return 1
	clrl	%r0		# else return 0
	ret
Lone:
	movl	$1,%r0
	ret
END(__udiv)

ASENTRY(__audiv,0)
	movl	DIVIDEND,%r3
	movl	DIVISOR,%r2
	jlss	La_easy		# big divisor: settle by comparison
	movl	(%r3),%r0
	jlss	La_hard		# big dividend: extended division
	divl2	%r2,%r0		# small divisor and dividend: signed division
	movl	%r0,(%r3)		# leave the value of the assignment in %r0
	ret
La_hard:
	clrl	%r1
	ediv	%r2,%r0,%r0,%r1
	movl	%r0,(%r3)
	ret
La_easy:
	cmpl	(%r3),%r2
	jgequ	La_one		# if dividend is as big or bigger, return 1
	clrl	%r0		# else return 0
	clrl	(%r3)
	ret
La_one:
	movl	$1,%r0
	movl	%r0,(%r3)
	ret
END(__audiv)
