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
 * from: Header: ffs.s,v 1.3 92/07/07 00:23:57 torek Exp
 */

#include <machine/asm.h>
#if defined(LIBC_SCCS) && !defined(lint)
#if 0
	.asciz "@(#)ffs.s	8.1 (Berkeley) 6/4/93"
#else
	RCSID("$NetBSD$")
#endif
#endif  /* LIBC_SCCS and not lint */

#if 0
	/*
	 * We have a popcount instruction -- use it.
	 * only uses %o0, %o1, %o2
	 *
	 * Here's the pseudo-code from the v9 spec:
	 *
	 * int ffs(unsigned zz) {
	 *	return popc( zz ^ ( ~ (-zz)));
	 * }
	 *
	 * XXXX sptifires and blackbirds don't implement popc,
	 * so we won't use this nice clean code 8^(.
	 */
ENTRY(ffs)
	neg	%o0, %o1				! %o1 = -zz
	xnor	%o0, %o1, %o2				! %o2 = zz ^ ~ -zz
	popc	%o2, %o1
	movrz	%o0, %g0, %o1				! result of ffs(0) should be zero
	retl
	 mov	%o1, %o0
#endif
/*
 * ffs returns the number of the rightmost bit set in its argument,
 * i.e., the lowest value such that (x & (ffs(x) - 1)) is nonzero.
 * If no bits are set, ffs returns 0.
 *
 * We use a table lookup on each byte.
 *
 * In each section below, %o1 is the current byte (0, 1, 2, or 3).
 * The last byte is handled specially: for the first three,
 * if that byte is nonzero, we return the table value
 * (plus 0, 8, or 16 for the byte number), but for the last
 * one, we just return the table value plus 24.  This means
 * that ffstab[0] must be -24 so that ffs(0) will return 0.
 */
ENTRY(ffs)
#ifdef __PIC__
	PICCY_SET(ffstab, %o2, %o3)
#else
	set	ffstab, %o2
#endif
	andcc	%o0, 0xff, %o1	! get low byte
	be,a	1f		! try again if 0
	 srl	%o0, 8, %o0	! delay slot, get ready for next byte

	retl			! return ffstab[%o1]
	 ldsb	[%o2 + %o1], %o0

1:
	andcc	%o0, 0xff, %o1	! byte 1 like byte 0...
	be,a	2f
	 srl	%o0, 8, %o0	! (use delay to prepare for byte 2)

	ldsb	[%o2 + %o1], %o0
	retl			! return ffstab[%o1] + 8
	 add	%o0, 8, %o0

2:
	andcc	%o0, 0xff, %o1
	be,a	3f
	 srl	%o0, 8, %o0	! (prepare for byte 3)

	ldsb	[%o2 + %o1], %o0
	retl			! return ffstab[%o1] + 16
	 add	%o0, 16, %o0

3:				! just return ffstab[%o0] + 24
	ldsb	[%o2 + %o0], %o0
	retl
	 add	%o0, 24, %o0

ffstab:
	.byte	-24,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1 /* 00-0f */
	.byte	5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* 10-1f */
	.byte	6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* 20-2f */
	.byte	5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* 30-3f */
	.byte	7,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* 40-4f */
	.byte	5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* 50-5f */
	.byte	6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* 60-6f */
	.byte	5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* 70-7f */
	.byte	8,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* 80-8f */
	.byte	5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* 10-9f */
	.byte	6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* a0-af */
	.byte	5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* b0-bf */
	.byte	7,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* c0-cf */
	.byte	5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* d0-df */
	.byte	6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* e0-ef */
	.byte	5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1	/* f0-ff */
