/*	$NetBSD$ */

/*
 * Copyright (c) 2002, 2005 Fujitsu Component Limited
 * Copyright (c) 2002, 2005 Genetec Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of The Fujitsu Component Limited nor the name of
 *    Genetec corporation may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY FUJITSU COMPONENT LIMITED AND GENETEC
 * CORPORATION ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL FUJITSU COMPONENT LIMITED OR GENETEC
 * CORPORATION BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <machine/asm.h>
#include <arm/armreg.h>
#include "assym.h"

#include <arm/s3c2xx0/s3c2800reg.h>	/* for S3C2800_DBANK0_START */

#include <evbarm/smdk2xx0/smdk2800var.h>

RCSID("$NetBSD$")

/*
 * Kernel start routine for Samsung SMDK2800.
 * This code is excuted at the very first after the kernel is loaded
 * by boot program on ROM or gzboot.
 */
	.text

#define KERNEL_TEXT_ADDR	(SDRAM_START+0x00200000)

	.global	_C_LABEL(smdk2800_start)
_C_LABEL(smdk2800_start):
	/* Are we running on right place ? */
	adr r0, _C_LABEL(smdk2800_start)
	cmp	r0, #KERNEL_TEXT_ADDR
	beq	smdk2800_start_ram
	
	/* move me to RAM
	 * XXX: we can use memcpy if it is PIC
	 */
	ldr r1, Lcopy_size
	adr r0, _C_LABEL(smdk2800_start)
	add r1, r1, #3
	mov r1, r1, LSR #2
	mov r2, #KERNEL_TEXT_ADDR
	mov r4, r2

	cmp r0, r2
	bhs 5f

	/* src < dest. copy from top */
	add r0,r0,r1,LSL #2
	add r2,r2,r1,LSL #2

3:	ldr r3,[r0,#-4]!
	str r3,[r2,#-4]!
	subs r1,r1,#1
	bhi 3b
	b 7f

	/* src >= dest. copy from bottom */
5:	ldr r3,[r0],#4
	str r3,[r2],#4
	subs r1,r1,#1
	bhi 5b

7:	
	/* Jump to RAM */
	ldr r0, Lstart_off
	add pc, r4, r0

Lcopy_size:	.word _edata-_C_LABEL(smdk2800_start)
Lstart_off:	.word smdk2800_start_ram-_C_LABEL(smdk2800_start)

smdk2800_start_ram:	
	/*
	 *  Kernel is loaded in SDRAM (0x08200000..), and is expected to run
	 *  in VA 0xc0200000..  
	 */

	/* Disable MMU for a while */
	mrc	p15, 0, r2, c1, c0, 0
	bic	r2, r2, #CPU_CONTROL_MMU_ENABLE
	mcr	p15, 0, r2, c1, c0, 0

	nop
	nop
	nop
	
	mov	r0,#SDRAM_START      /* pagetable */
	adr	r4, mmu_init_table
	b	2f
1:
	str	r3, [r0, r2]
	add	r2, r2, #4
	add	r3, r3, #(L1_S_SIZE)
	adds	r1, r1, #-1
	bhi	1b
2:	
	ldmia	r4!, {r1,r2,r3}   /* # of sections, PA|attr, VA */
	cmp	r1, #0
	bne	1b	

	mcr	p15, 0, r0, c2, c0, 0	/* Set TTB */
	mcr	p15, 0, r0, c8, c7, 0	/* Flush TLB */

	/* Set the Domain Access register.  Very important! */
	mov	r0, #((DOMAIN_CLIENT << (PMAP_DOMAIN_KERNEL*2)) | DOMAIN_CLIENT)
	mcr	p15, 0, r0, c3, c0, 0

	/* Enable MMU */
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #CPU_CONTROL_MMU_ENABLE
	mcr	p15, 0, r0, c1, c0, 0
	
	nop
	nop
	nop

	/* Jump to kernel code in TRUE VA */
	adr	r0, Lstart
	ldr	pc, [r0]

Lstart:
	.word	start

#define MMU_INIT(va,pa,n_sec,attr) \
	.word	n_sec					    ; \
	.word	4*((va)>>L1_S_SHIFT)			    ; \
	.word	(pa)|(attr)				    ;
	
mmu_init_table:	
	/* fill all table VA==PA */
	MMU_INIT(0x00000000, 0x00000000, 1<<(32-L1_S_SHIFT), L1_TYPE_S|L1_S_AP_KRW)
	/* map SDRAM VA==PA, WT cacheable */
	MMU_INIT(SDRAM_START, SDRAM_START, 64, L1_TYPE_S|L1_S_C|L1_S_AP_KRW)
	/* map VA 0xc0000000..0xc3ffffff to PA 0x08000000..0x0bffffff */
	MMU_INIT(0xc0000000, SDRAM_START, 64, L1_TYPE_S|L1_S_C|L1_S_AP_KRW)

	/* Map all built-in peripheral registers at 0xfd000000 */
	MMU_INIT(SMDK2800_IO_AREA_VBASE, S3C2800_PERIPHERALS, 2, L1_TYPE_S|L1_S_AP_KRW)
	.word 0	/* end of table */

