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
#include <machine/math.h>

;_sccsid:
;.asciz	"from: @(#)support.s	5.2 (Berkeley) 5/17/90"

RCSID("$NetBSD$")

#if !defined(__HAVE_LONG_DOUBLE)
STRONG_ALIAS(_copysignl, copysign)
WEAK_ALIAS(copysignl, _copysignl)
#endif

| copysign(x,y)
| returns x with the sign of y.
ENTRY(copysign)
#ifdef __SVR4_ABI__
	tstw	%sp@(12)
	jmi	Lneg
	bclr	#31,%sp@(4)
	fmoved	%sp@(4),%fp0
	rts
Lneg:
	bset	#31,%sp@(4)
	fmoved	%sp@(4),%fp0
	rts
#else
	movl	%sp@(4),%d0
	movl	%sp@(8),%d1
	tstw	%sp@(12)
	jmi	Lneg
	bclr	#31,%d0
	rts
Lneg:
	bset	#31,%d0
	rts
#endif
