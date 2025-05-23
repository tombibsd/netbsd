/*	$NetBSD$	*/
/*-
 * Copyright (c) 2013 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Matt Thomas <matt@3am-software.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "atomic_op_asm.h"

#ifdef _ARM_ARCH_6

ENTRY_NP(_atomic_nand_64_nv)
	push	{r3,r4}			/* save temporary */
#ifndef __ARM_EABI__
	mov	r3, r2
	mov	r2, r1
#endif
	mov	ip, r0			/* need r0 for return value */
1:	ldrexd	r0, r1, [ip]		/* load old value */
	ands	r0, r0, r2		/* calculate new value step 1 */
	ands	r1, r1, r3		/* calculate new value step 2 */
	mvns	r0, r0			/* ... complement for new value */
	mvns	r1, r1			/* ... complement for new value */
	strexd	r4, r0, r1, [ip]	/* try to store */
	cmp	r4, #0			/*   succeed? */
	bne	1b			/*     no, try again? */
#ifdef _ARM_ARCH_7
	dmb
#else
	mcr	p15, 0, r4, c7, c10, 5	/* data memory barrier */
#endif
	pop	{r3,r4}			/* restore temporary */
	RET				/* return new value */
END(_atomic_nand_64_nv)

STRONG_ALIAS(_atomic_nand_64,_atomic_nand_64_nv)
ATOMIC_OP_ALIAS(atomic_nand_64_nv,_atomic_nand_64_nv)
ATOMIC_OP_ALIAS(atomic_nand_64,_atomic_nand_64_nv)
CRT_ALIAS(__sync_nand_and_fetch_8,_atomic_nand_64_nv)

#endif /* _ARM_ARCH_6 */
