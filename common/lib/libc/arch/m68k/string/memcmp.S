/*	$NetBSD$	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department.
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

#if defined(LIBC_SCCS) && !defined(lint)
#if 0
	RCSID("from: @(#)bcmp.s	5.1 (Berkeley) 5/12/90")
#else
	RCSID("$NetBSD$")
#endif
#endif /* LIBC_SCCS and not lint */

/* memcmp(s1, s2, n) */

#ifdef __mcoldfire__
#define	CMPMB(a,b)	movb b,%d2; cmpb a,%d2
#define	CMPMW(a,b)	movw b,%d2; cmpw a,%d2
#define	CMPML(a,b)	movl b,%d2; cmpl a,%d2
#else
#define	CMPMB(a,b)	cmpmb a,b
#define	CMPMW(a,b)	cmpmw a,b
#define	CMPML(a,b)	cmpml a,b
#endif

/*
 * This is probably not the best we can do, but it is still 2-10 times
 * faster than the C version in the portable gen directory.
 *
 * Things that might help:
 *	- longword align when possible (only on the 68020)
 *	- use nested DBcc instructions or use one and limit size to 64K
 */
ENTRY(memcmp)
	movl	4(%sp),%a0		| string 1
	movl	8(%sp),%a1		| string 2
	movl	12(%sp),%d0		| length
#ifdef __mcoldfire__
	movl	%d2,-(%sp)		| save temp
#endif
	jeq	.Lbcdone		| if zero, nothing to do
	movl	%a0,%d1
	btst	#0,%d1			| string 1 address odd?
	jeq	.Lbceven		| no, skip alignment
	CMPMB((%a0)+,(%a1)+)		| yes, compare a byte
	jne	.Lbcnoteq		| not equal, return non-zero
	subql	#1,%d0			| adjust count
	jeq	.Lbcdone		| count 0, reutrn zero
.Lbceven:
	movl	%a1,%d1
	btst	#0,%d1			| string 2 address odd?
	jne	.Lbcbloop		| yes, no hope for alignment, compare bytes
	movl	%d0,%d1			| no, both even
	lsrl	#2,%d1			| convert count to longword count
	jeq	.Lbcbloop		| count 0, skip longword loop
.Lbclloop:
	CMPML((%a0)+,(%a1)+)		| compare a longword
	jne	.Lbcnoteql		| not equal, return non-zero
	subql	#1,%d1			| adjust count
	jne	.Lbclloop		| still more, keep comparing
	andl	#3,%d0			| what remains
	jeq	.Lbcdone		| nothing, all done
.Lbcbloop:
	CMPMB((%a0)+,(%a1)+)		| compare a byte
	jne	.Lbcnoteq		| not equal, return non-zero
	subql	#1,%d0			| adjust count
	jne	.Lbcbloop		| still more, keep going
	rts
.Lbcnoteql:
	subql	#4,%a0
	subql	#4,%a1
	movl	#4,%d0
	jra	.Lbcbloop
.Lbcnoteq:
	clrl	%d0
	clrl	%d1
	movb	-(%a0),%d0
	movb	-(%a1),%d1
	subl	%d1,%d0
.Lbcdone:
#ifdef __mcoldfire__
	movl	(%sp)+,%sp		| restore temp
#endif
	rts
END(memcmp)
