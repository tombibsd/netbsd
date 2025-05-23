/*	$NetBSD$	*/

/*
 * Copyright (c) 1998 Mark Brinicombe.
 * Copyright (c) 1998 Causality Limited
 * All rights reserved.
 *
 * This code is derived from software contributed to the NetBSD Foundation
 * by IWAMOTO Toshihiro.
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
 *      This product includes software developed by Mark Brinicombe
 *      for the NetBSD Project.
 * 4. The name of the company nor the name of the author may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "opt_irqstats.h"

#include "assym.h"

#include <arm/asm.h>
#include <arm/locore.h>
#include <arm/sa11x0/sa11x0_reg.h>


	.text
	.align 0

Lspl_masks:
	.word	_C_LABEL(spl_masks)

Lcpu_info_store:
        .word   _C_LABEL(cpu_info_store)

	.globl	_C_LABEL(saipic_base)
_C_LABEL(saipic_base):
	.word	0x00000000

#ifdef INTR_DEBUG
Ldbg_str:
	.asciz	"irq_entry %x %x\n"
	.align 5
#endif

LOCK_CAS_CHECK_LOCALS

AST_ALIGNMENT_FAULT_LOCALS

/*
 * Register usage
 *
 *  r4  - Pointer to cpu_info
 *  r5  - Pointer to handler pointer list
 *  r6  - Address of current handler
 *  r7	- pspr mode
 *  r8  - Current IRQ requests.
 *  r9  - Used to count through possible IRQ bits.
 *  r10 - Base address of SAIP
 */

ASENTRY_NP(irq_entry)
	sub	lr, lr, #0x00000004	/* Adjust the lr */

	PUSHFRAMEINSVC			/* Push an interrupt frame */
	ENABLE_ALIGNMENT_FAULTS

	/* Load r8 with the SAIPIC interrupt requests */

	ldr	r10, _C_LABEL(saipic_base)
	ldr	r8, [r10, #(SAIPIC_IP)]	/* Load IRQ pending register */

#ifdef INTR_DEBUG
	ldr	r2, [r10, #(SAIPIC_MR)]
	adr	r0, Ldbg_str
	mov	r1, r8
	bl	_C_LABEL(printf)
#endif
	/*
	 * Note that we have entered the IRQ handler.
	 * We are in SVC mode so we cannot use the processor mode
	 * to determine if we are in an IRQ. Instead we will count the
	 * each time the interrupt handler is nested.
	 */

	ldr	r1, [r4, #CI_INTR_DEPTH]
	add	r1, r1, #1
	str	r1, [r4, #CI_INTR_DEPTH]

	/*
 	 * Need to block all interrupts at the IPL or lower for
	 * all asserted interrupts.
	 * This basically emulates hardware interrupt priority levels.
	 * Means we need to go through the interrupt mask and for
	 * every asserted interrupt we need to mask out all other
	 * interrupts at the same or lower IPL.
	 * If only we could wait until the main loop but we need to sort
	 * this out first so interrupts can be re-enabled.
	 *
	 * This would benefit from a special ffs type routine
	 */

	mov	r9, #(NIPL - 1)
	ldr	r5, Lspl_masks

Lfind_highest_ipl:
	ldr	r2, [r5, r9, lsl #2]
	tst	r8, r2
	subeq	r9, r9, #1
	beq	Lfind_highest_ipl

	/* r9 = SPL level of highest priority interrupt */
	add	r9, r9, #1
	ldr	r2, [r5, r9, lsl #2]

	ldr	r1, [r4, #CI_CPL]
	str	r9, [r4, #CI_CPL]
	stmfd	sp!, {r1}

	/* Update the SAIP irq masks */
	bl	_C_LABEL(irq_setmasks)

#ifdef INTR_DEBUG
	stmfd	sp!, {r0,r1,r2}
	adr	r0, Ldbg_str
	mov	r1, #1
	mov	r2, r9
	bl	_C_LABEL(printf)
	ldmia	sp!, {r0,r1,r2}
#endif
        mrs     r0, cpsr		/* Enable IRQs */
	bic	r0, r0, #I32_bit
	msr	cpsr_all, r0

	ldr	r5, Lirqhandlers
        mov	r9, #0x00000001

irqloop:
	/* This would benefit from a special ffs type routine */
	tst	r8, r9			/* Is a bit set ? */
	beq	nextirq			/* No ? try next bit */

	ldr	r6, [r5]		/* Get address of first handler structure */

	teq	r6, #0x00000000		/* Do we have a handler */
	moveq	r0, r8			/* IRQ requests as arg 0 */
	beq	_C_LABEL(stray_irqhandler) /* call special handler */

	ldr	r0, [r4, #(CI_CC_NINTR)]
	ldr	r1, [r4, #(CI_CC_NINTR+4)]
#ifdef _ARMEL
	adds	r0, r0, #0x00000001
	adc	r1, r1, #0x00000001
#else
	adds	r1, r1, #0x00000001
	adc	r0, r0, #0x00000000
#endif
	str	r0, [r4, #(CI_CC_NINTR)]
	str	r1, [r4, #(CI_CC_NINTR+4)]

	/*
	 * XXX: Should stats be accumulated for every interrupt routine
	 * called or for every physical interrupt that is serviced.
	 */

#ifdef IRQSTATS
	ldr	r0, Lintrcnt
	ldr	r1, [r6, #(IH_COUNT)]

	add	r0, r0, r1, lsl #2
	ldr	r1, [r0]
	add	r1, r1, #0x00000001
	str	r1, [r0]
#endif	/* IRQSTATS */

irqchainloop:
#ifdef INTR_DEBUG
	stmfd	sp!, {r0,r1,r2}
	adr	r0, Ldbg_str
	mov	r1, #2
	bl	_C_LABEL(printf)
	ldmia	sp!, {r0,r1,r2}
#endif
	ldr	r0, [r6, #(IH_ARG)]	/* Get argument pointer */
	teq	r0, #0x00000000		/* If arg is zero pass stack frame */
	addeq	r0, sp, #4		/* ... stack frame [XXX needs care] */
	mov	lr, pc			/* return address */
	ldr	pc, [r6, #(IH_FUNC)]	/* Call handler */

	teq	r0, #0x00000001		/* Was the irq serviced ? */
	beq	irqdone

	ldr	r6, [r6, #(IH_NEXT)]
	teq	r6, #0x00000000
	bne	irqchainloop

irqdone:
nextirq:
	add	r5, r5, #0x00000004	/* update pointer to handlers */
	mov	r9, r9, lsl #1		/* move on to next bit */
	teq	r9, #(1 << 31)		/* done the last bit ? */
	bne	irqloop			/* no - loop back. */

	ldmfd	sp!, {r2}
	str	r2, [r4, #CI_CPL]

	/* Restore previous disabled mask */
	bl	_C_LABEL(irq_setmasks)

#ifdef __HAVE_FAST_SOFTINTS
	bl	_C_LABEL(dosoftints)	/* Handle the soft interrupts */
#endif

	/* Kill IRQ's in preparation for exit */
        mrs     r0, cpsr
        orr     r0, r0, #(I32_bit)
        msr     cpsr_all, r0

#ifdef INTR_DEBUG
	adr	r0, Ldbg_str
	mov	r1, #3
	ldr	r2, [r10, #(SAIPIC_MR)]
	bl	_C_LABEL(printf)
#endif

	/* Decrement the nest count */
	ldr	r1, [r4, #CI_INTR_DEPTH]
	sub	r1, r1, #1
	str	r1, [r4, #CI_INTR_DEPTH]

	LOCK_CAS_CHECK

	DO_AST_AND_RESTORE_ALIGNMENT_FAULTS
	PULLFRAMEFROMSVCANDEXIT

	/* NOT REACHED */
	b	. - 8

ENTRY(irq_setmasks)
	stmfd	sp!, {r0, r1, r4, lr}	/* Preserve registers */

	/* Disable interrupts */
	mrs	r1, cpsr
	orr	r3, r1,  #(I32_bit)
	msr	cpsr_all, r3

	/* Calculate interrupt mask */
	ldr	r0, Lspl_masks
	ldr	r4, Lcpu_info_store
	ldr	r2, [r4, #CI_CPL]
	ldr	r2, [r0, r2, lsl #2]

	ldr	r0, _C_LABEL(saipic_base)
	str	r2, [r0, #(SAIPIC_MR)]	/* Set mask register */

	/* Restore old cpsr and exit */
	msr	cpsr_all, r1
	ldmia	sp!, {r0, r1, r4, pc}	/* Restore registers */

#ifdef IRQSTATS
Lintrcnt:
	.word	_C_LABEL(intrcnt)
#endif

Lirqhandlers:
	.word	_C_LABEL(irqhandlers)	/* Pointer to array of irqhandlers */


#ifdef IRQSTATS
	.global _C_LABEL(intrnames), _C_LABEL(eintrnames)
	.global _C_LABEL(eintrcnt)
_C_LABEL(intrnames): 
_C_LABEL(eintrnames):
_C_LABEL(eintrcnt):

	.globl	_C_LABEL(intrcnt), _C_LABEL(sintrcnt)

_C_LABEL(intrcnt):
	.space	ICU_LEN*4  /* XXX Should be linked to number of interrupts */

_C_LABEL(sintrcnt):
	.space 32*4
#endif
