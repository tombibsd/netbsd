/*	$NetBSD$	*/

/*-
 * Copyright (c) 2007 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe.
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

	.text

ENTRY(_atomic_or_32)
	movl	4(%sp), %a0
	movl	8(%sp), %d0
	orl	%d0, (%a0)
	rts
END(_atomic_or_32)

ATOMIC_OP_ALIAS(atomic_or_32,_atomic_or_32)
ATOMIC_OP_ALIAS(atomic_or_uint,_atomic_or_32)
STRONG_ALIAS(_atomic_or_uint,_atomic_or_32)
ATOMIC_OP_ALIAS(atomic_or_ulong,_atomic_or_32)
STRONG_ALIAS(_atomic_or_ulong,_atomic_or_32)
ATOMIC_OP_ALIAS(atomic_or_ptr,_atomic_or_32)
STRONG_ALIAS(_atomic_or_ptr,_atomic_or_32)

ENTRY(_atomic_or_32_nv)
	movl	4(%sp), %a0
1:	movl	(%a0), %d0
	movl	%d0, %d1
	orl	8(%sp), %d0
	casl	%d1, %d0, (%a0)
	bne	1b
	movl	%d0, %a0
	rts
END(_atomic_or_32_nv)

ATOMIC_OP_ALIAS(atomic_or_32_nv,_atomic_or_32_nv)
ATOMIC_OP_ALIAS(atomic_or_uint_nv,_atomic_or_32_nv)
STRONG_ALIAS(_atomic_or_uint_nv,_atomic_or_32_nv)
ATOMIC_OP_ALIAS(atomic_or_ulong_nv,_atomic_or_32_nv)
STRONG_ALIAS(_atomic_or_ulong_nv,_atomic_or_32_nv)
ATOMIC_OP_ALIAS(atomic_or_ptr_nv,_atomic_or_32_nv)
STRONG_ALIAS(_atomic_or_ptr_nv,_atomic_or_32_nv)
CRT_ALIAS(__sync_or_and_fetch_4, _atomic_or_32_nv)


ENTRY(__sync_fetch_and_or_4)
	movl	4(%sp), %a0
1:	movl	(%a0), %d0
	movl	%d0, %d1
	orl	8(%sp), %d1
	casl	%d0, %d1, (%a0)
	bne	1b
	movl	%d0, %a0
	rts
END(__sync_fetch_and_or_4)


ENTRY(_atomic_or_16_nv)
	movl	4(%sp), %a0
1:	movw	(%a0), %d0
	movw	%d0, %d1
	orw	8(%sp), %d0
	casw	%d1, %d0, (%a0)
	bne	1b
	rts
END(_atomic_or_16_nv)
CRT_ALIAS(__sync_or_and_fetch_2, _atomic_or_16_nv)

ENTRY(__sync_fetch_and_or_2)
	movl	4(%sp), %a0
1:	movw	(%a0), %d0
	movw	%d0, %d1
	orw	8(%sp), %d1
	casw	%d0, %d1, (%a0)
	bne	1b
	rts
END(__sync_fetch_and_or_2)


ENTRY(_atomic_or_8_nv)
	movl	4(%sp), %a0
1:	movb	(%a0), %d0
	movb	%d0, %d1
	orb	8(%sp), %d0
	casb	%d1, %d0, (%a0)
	bne	1b
	rts
END(_atomic_or_8_nv)
CRT_ALIAS(__sync_or_and_fetch_1, _atomic_or_8_nv)

ENTRY(__sync_fetch_and_or_1)
	movl	4(%sp), %a0
1:	movb	(%a0), %d0
	movb	%d0, %d1
	orb	8(%sp), %d1
	casb	%d0, %d1, (%a0)
	bne	1b
	rts
END(__sync_fetch_and_or_1)

CRT_ALIAS(__atomic_fetch_or_1,__sync_fetch_and_or_1)
CRT_ALIAS(__atomic_fetch_or_2,__sync_fetch_and_or_2)
CRT_ALIAS(__atomic_fetch_or_4,__sync_fetch_and_or_4)
