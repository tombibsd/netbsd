/*	$NetBSD$	*/
/*-
 * Copyright (c) 1990, 1993
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

/* int memcmp(s1, s2, n) */

#include "DEFS.h"

#ifdef LIBC_SCCS
RCSID("$NetBSD$")
#endif

ENTRY(memcmp, 0)
	movl	12(%ap),%r0
	jeql	9f
	movq	4(%ap),%r1
	ashl	$-2,%r0,%r3	# convert len to long words
	jeql	2f
1:
	cmpl	(%r1)+,(%r2)+	# no "cmpq" alas
	jneq	7f
	sobgtr	%r3,1b
2:
	bicl3	$-4,%r0,%r3	# handle at most 3 extra bytes
	jeql	4f
3:
	cmpb	(%r1)+,(%r2)+
	jneq	8f
	sobgtr	%r3,3b
4:
	clrl	%r0		# we had a match
	ret
7:				# backup, and do a byte compare
	tstl	-(%r1)
	tstl	-(%r2)
	movl	$4,%r3
	jbr	3b
8:
	movzbl	-(%r1),%r3
	movzbl	-(%r2),%r4
	subl3	%r4,%r3,%r0
9:
	ret
END(memcmp)
