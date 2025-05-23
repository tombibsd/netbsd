/*	$NetBSD$	*/
/*
 * Copyright (c) 1985, 1993
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
 *
 *	@(#)cbrt.s	8.1 (Berkeley) 6/4/93
 */

#include <machine/asm.h>

/*
 * double cbrt(double arg)
 * W. Kahan, 10/13/80. revised 1/13/84 for keeping sign symmetry
 * error check by E LeBlanc, 8/18/82
 * Revised and tested by K.C. Ng, 5/2/85
 * Max error less than 0.667 ulps (unit in the last places)
 */

ENTRY(cbrtf, 0)
	cvtfd	4(%ap),-(%sp)
	calls	$2,_C_LABEL(cbrt)
	cvtdf	%r0,%r0
	ret

#ifdef WEAK_ALIAS
WEAK_ALIAS(cbrtl, cbrt)
#endif

ALTENTRY(cbrt)
ENTRY(d_cbrt, 0x00c0)		# save %r6 & %r7
	movq	4(%ap),%r0	# %r0 = argument x
	jbr 	dcbrt2

ENTRY(dcbrt_, 0x00c0)		# save %r6 & %r7
	movq	*4(%ap),%r0	# %r0 = argument x

dcbrt2:	bicw3	$0x807f,%r0,%r2	# biased exponent of x
	jeql	return		# dcbrt(0)=0  dcbrt(res)=res. operand
	bicw3	$0x7fff,%r0,%ap	# ap has sign(x)
	xorw2	%ap,%r0		# %r0 is abs(x)
	movl	%r0,%r2		# %r2 has abs(x)
	rotl	$16,%r2,%r2	# %r2 = |x| with bits unscrambled
	divl2	$3,%r2		# rough dcbrt with bias/3
	addl2	B,%r2		# restore bias, diminish fraction
	rotl	$16,%r2,%r2	# %r2=|q|=|dcbrt| to 5 bits
	mulf3	%r2,%r2,%r3	# %r3 =qq
	divf2	%r0,%r3		# %r3 = qq/x
	mulf2	%r2,%r3
	addf2	C,%r3		# %r3 = s = C + qqq/x
	divf3	%r3,D,%r4		# %r4 = D/s
	addf2	E,%r4
	addf2	%r4,%r3		# %r3 = s + E + D/s
	divf3	%r3,F,%r3		# %r3 = F / (s + E + D/s)
	addf2	G,%r3		# %r3 = G + F / (s + E + D/s)
	mulf2	%r3,%r2		# %r2 = q%r3 = new q to 23 bits
	clrl	%r3		# %r2:%r3 = q as double float
	muld3	%r2,%r2,%r4	# %r4:%r5 = qq exactly
	divd2	%r4,%r0		# %r0:%r1 = x/(q*q) rounded
	subd3	%r2,%r0,%r6	# %r6:%r7 = x/(q*q) - q exactly
	movq    %r2,%r4		# %r4:%r5 = q
	addw2	$0x80,%r4	# %r4:%r5 = 2 * q
	addd2	%r0,%r4		# %r4:%r5 = 2*q + x/(q*q)
	divd2	%r4,%r6		# %r6:%r7 = (x/(q*q)-q)/(2*q+x/(q*q))
	muld2	%r2,%r6		# %r6:%r7 = q*(x/(q*q)-q)/(2*q+x/(q*q))
	addd3	%r6,%r2,%r0	# %r0:%r1 = q + %r6:%r7
	bisw2	%ap,%r0		# restore the sign bit
return:
	ret			# error less than 0.667 ulps

	_ALIGN_TEXT
B :	.long		 721142941		# (86-0.03306235651)*(2^23)
C :	.float		0f0.5428571429		# 19/35
D :	.float		0f-0.7053061224		# -864/1225
E :	.float		0f1.414285714		# 99/70
F :	.float		0f1.607142857		# 45/28
G :	.float		0f0.3571428571		# 5/14
