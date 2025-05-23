/*	$NetBSD$	*/

/*
 * Copyright (c) 1988 University of Utah.
 * Copyright (c) 1980, 1990 The Regents of the University of California.
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
 *
 *	from: Utah Hdr: locore.s 1.58 91/04/22
 *	@(#)locore.s	7.11 (Berkeley) 5/9/91
 */

#include "DEFS.h"

/*
 * Emulate fancy VAX string operations:
 *	scanc(count, startc, table, mask)
 */
ENTRY(scanc)
	movl	4(%sp),%d0	| get length
	jeq	Lscdone		| nothing to do, return
	movl	8(%sp),%a0	| start of scan
	movl	12(%sp),%a1	| table to compare with
	movb	19(%sp),%d1	| and mask to use
	movl	%d2,-(%sp)	| need a scratch register
	clrl	%d2		| clear it out
#ifndef __mcoldfire__
	subqw	#1,%d0		| adjust for dbra
#endif
Lscloop:
	movb	(%a0)+,%d2	| get character
	movb	(%a1,%d2),%d2	| get table entry
	andl	%d1,%d2		| mask it
#ifdef __mcoldfire__
	jne	1f		| break out if mask matched
	subql	#1,%d0		| decrement
	jne	Lscloop		| keep going til no more
1:
#else
	dbne	%d0,Lscloop	| keep going til no more or non-zero
	addqw	#1,%d0		| overshot by one
#endif
	movl	(%sp)+,%d2	| restore scratch
Lscdone:
	rts
