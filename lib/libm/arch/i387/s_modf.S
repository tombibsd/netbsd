/*	$NetBSD$	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Sean Eric Fagan.
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
 *	from: @(#)modf.s	5.5 (Berkeley) 3/18/91
 */

#include <machine/asm.h>
#if defined(LIBC_SCCS)
	RCSID("$NetBSD$")
#endif

/*
 * modf(value, iptr): return fractional part of value, and stores the
 * integral part into iptr (a pointer to double).
 *
 * Written by Sean Eric Fagan (sef@kithrup.COM)
 * Sun Mar 11 20:27:30 PST 1990
 */

/* With CHOP mode on, frndint behaves as TRUNC does.  Useful. */
ENTRY(modf)
#ifdef __x86_64__
	pushq	%rbp
	movq	%rsp,%rbp
	subq	$24,%rsp

	/* Set chop mode. */
	fnstcw	-12(%rbp)
	movw	-12(%rbp),%dx
	orw	$3072,%dx
	movw	%dx,-16(%rbp)
	fldcw	-16(%rbp)

	/* Get integral part. */
	movsd	%xmm0,-24(%rbp)
	fldl	-24(%rbp)
	frndint
	fstpl	-8(%rbp)

	/* Restore control word. */
	fldcw	-12(%rbp)

	/* Store integral part. */
	movsd	-8(%rbp),%xmm0
	movsd	%xmm0,(%rdi)

	/* Get fractional part and return it. */
	fldl	-24(%rbp)
	fsubl	-8(%rbp)
	fstpl	-24(%rbp)
	movsd	-24(%rbp),%xmm0
#else
	pushl	%ebp
	movl	%esp,%ebp
	subl	$16,%esp
	fnstcw	-12(%ebp)
	movw	-12(%ebp),%dx
	orw	$3072,%dx
	movw	%dx,-16(%ebp)
	fldcw	-16(%ebp)
	fldl	8(%ebp)
	frndint
	fstpl	-8(%ebp)
	fldcw	-12(%ebp)
	movl	16(%ebp),%eax
	movl	-8(%ebp),%edx
	movl	-4(%ebp),%ecx
	movl	%edx,(%eax)
	movl	%ecx,4(%eax)
	fldl	8(%ebp)
	fsubl	-8(%ebp)
	jmp	L1
L1:
#endif
	leave
	ret
