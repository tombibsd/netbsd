/*	$NetBSD$	*/

/*-
 * Copyright (c) 2004, 2005 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by UCHIYAMA Yasushi.
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

#include <mips/asm.h>
#include <mips/cpuregs.h>

	.set	noreorder
/*
 * EWS4800 NMI entry. (0x200)
 */
LEAF_NOPROFILE(ews4800mips_nmi_vec)
	lui	sp, 0x2000
	la	k1, nmi_exception
	or	k1, k1, sp	/* uncached */
	la	sp, start
	jr	k1
	 nop
END(ews4800mips_nmi_vec)

LEAF_NOPROFILE(nmi_exception)
	li	a0, 0			/* x-pos */
	li	k0, 32
1:
	mfc0	v0, MIPS_COP_0_ERROR_PC	/* get Error EPC */
	addu	k0, k0, -4
	srl	v0, v0, k0
	andi	v1, v0, 0xf
	sltiu	v0, v1, 10
	bnez	v0, 2f
	 nop
	addi	v1, v1, 'a' - ('9' + 1)
2:
	addi	a2, v1, '0'
	lui	v0, 0xbfc0
	ori	v0, 0xff60
	jal	v0			/* ROM_PUTC(x, y, c) */
	 li	a1, 0			/* y-pos */
	bne	k0, zero, 1b
	 addiu	a0, a0, 12
3:
	b	3b
	 nop
	/* NOTREACHED */
END(nmi_exception)
