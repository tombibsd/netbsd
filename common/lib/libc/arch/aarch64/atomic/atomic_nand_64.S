/* $NetBSD$ */

/*-
 * Copyright (c) 2014 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Matt Thomas of 3am Software Foundry.
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

ENTRY_NP(_atomic_nand_64)
	mov	x4, x0
1:	ldxr	x0, [x4]		/* load old value (to be returned) */
	mvn	x2, x0			/* complement source */
	and	x2, x2, x1		/* calculate new value */
	stxr	w3, x2, [x4]		/* try to store */
	cbnz	w3, 1b			/*   succeed? no, try again */
	dmb	st
	ret				/* return old value */
END(_atomic_nand_64)

ATOMIC_OP_ALIAS(atomic_nand_64,_atomic_nand_64)
ATOMIC_OP_ALIAS(atomic_nand_ulong,_atomic_nand_64)
STRONG_ALIAS(__sync_fetch_and_nand_8,_atomic_nand_64)
STRONG_ALIAS(_atomic_nand_ulong,_atomic_nand_64)

ENTRY_NP(_atomic_nand_64_nv)
	mov	x4, x0			/* need r0 for return value */
1:	ldxr	x0, [x4]		/* load old value */
	mvn	x0, x0			/* complement source */
	and	x0, x0, x1		/* calculate new value (return value) */
	stxr	w3, x0, [x4]		/* try to store */
	cbnz	w3, 1b			/*   succeed? no, try again? */
	dmb	st
	ret				/* return new value */
END(_atomic_nand_64_nv)

ATOMIC_OP_ALIAS(atomic_nand_64_nv,_atomic_nand_64_nv)
ATOMIC_OP_ALIAS(atomic_nand_ulong_nv,_atomic_nand_64_nv)
STRONG_ALIAS(__sync_nand_and_fetch_8,_atomic_nand_64_nv)
STRONG_ALIAS(_atomic_nand_ulong_nv,_atomic_nand_64_nv)
