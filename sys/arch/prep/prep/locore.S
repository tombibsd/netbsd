/*	$NetBSD$	*/
/*	$OpenBSD: locore.S,v 1.4 1997/01/26 09:06:38 rahnds Exp $	*/

/*
 * Copyright (C) 1995, 1996 Wolfgang Solfrank.
 * Copyright (C) 1995, 1996 TooLs GmbH.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by TooLs GmbH.
 * 4. The name of TooLs GmbH may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY TOOLS GMBH ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TOOLS GMBH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "opt_ddb.h"
#include "opt_ipkdb.h"
#include "opt_lockdebug.h"
#include "opt_modular.h"
#include "opt_multiprocessor.h"
#include "opt_ppcparam.h"
#include "assym.h"

#include <sys/syscall.h>

#include <machine/param.h>
#include <machine/psl.h>
#include <machine/trap.h>
#include <machine/asm.h>

#include <powerpc/spr.h>
#include <powerpc/oea/spr.h>

#include "ksyms.h"

/*
 * Some instructions gas doesn't understand (yet?)
 */
#define	bdneq	bdnzf 2,

/*
 * cache bit
 */
#define	HID0_BTCD	(1<<1)
#define	HID0_BHTE	(1<<2)
#define	HID0_SIED	(1<<7)
#define	HID0_DCI	(1<<10)
#define	HID0_ICFI	(1<<11)
#define	HID0_DCE	(1<<14)
#define	HID0_ICE	(1<<15)

/*
 * Globals
 */
GLOBAL(startsym)
	.long	0			/* start symbol table */
GLOBAL(endsym)
	.long	0			/* end symbol table */
GLOBAL(oeacpufeat)
	.long	0			/* cpu features */

/*
 * This symbol is here for the benefit of kvm_mkdb, and is supposed to
 * mark the start of kernel text.
 */
	.text
	.globl	_C_LABEL(kernel_text)
_C_LABEL(kernel_text):

/*
 * Startup entry.  Note, this must be the first thing in the text
 * segment!
 */
	.text
	.globl	__start
__start:
	li	0,0
	mtmsr	0			/* Disable FPU/MMU/exceptions */
	isync

/* compute end of kernel memory */
#if NKSYMS || defined(DDB) || defined(MODULAR)
	lis	7,_C_LABEL(startsym)@ha
	addi	7,7,_C_LABEL(startsym)@l
	stw	3,0(7)
	lis	7,_C_LABEL(endsym)@ha
	addi	7,7,_C_LABEL(endsym)@l
	stw	4,0(7)
#else
	lis	4,_C_LABEL(end)@ha
	addi	4,4,_C_LABEL(end)@l
#endif

	INIT_CPUINFO(4,1,9,0)
	
	lis	3,__start@ha
	addi	3,3,__start@l

	bl	_C_LABEL(initppc)

/* enable internal i/d-cache */
	mfpvr	9
	rlwinm	9,9,16,16,31
	cmpwi	%r9,1
	beq	3f			/* not needed for 601 */
	mfspr	11,SPR_HID0
	andi.	0,11,HID0_DCE
	ori	11,11,HID0_ICE|HID0_DCE
	ori	8,11,HID0_ICFI
	bne	1f			/* don't invalidate the D-cache */
	ori	8,8,HID0_DCI		/* unless it wasn't enabled */
1:
	sync
	mtspr	SPR_HID0,8		/* enable and invalidate caches */
	sync
	mtspr	SPR_HID0,11		/* enable caches */
	sync
	isync
	cmpwi	%r9,4			/* check for 604 */
	cmpwi	%cr1,%r9,9		/* or 604e */
	cmpwi	%cr2,%r9,10		/* or mach5 */
	cror	2,2,6
	cror	2,2,10
	bne	3f
	ori	11,11,HID0_SIED|HID0_BHTE /* for 604[e], enable */
	bne	2,2f
	ori	11,11,HID0_BTCD
2:
	mtspr	SPR_HID0,11
3:
	sync
	isync

	bl	_C_LABEL(main)

loop:
	b	loop			/* not reached */

	.globl	_C_LABEL(enable_intr)
_C_LABEL(enable_intr):
	mfmsr	3
	ori	4,3,PSL_EE@l
	mtmsr	4
	blr

	.globl	_C_LABEL(disable_intr)
_C_LABEL(disable_intr):
	mfmsr	3
	andi.	4,3,~PSL_EE@l
	mtmsr	4
	blr

/*
 * Pull in common switch / setfault code.
 */
#include <powerpc/powerpc/locore_subr.S>

/*
 * Pull in common trap vector code.
 */
#include <powerpc/powerpc/trap_subr.S>

/*
 * Pull in common pio / bus_space code.
 */
#include <powerpc/powerpc/pio_subr.S>
