/*	$NetBSD$	*/

/*
 * Copyright 2003 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Steve C. Woodford for Wasabi Systems, Inc.
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
 *      This product includes software developed for the NetBSD Project by
 *      Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

RCSID("$NetBSD$")	

	.text
	.align	0

/*
 * r0 = user space address
 * r1 = kernel space address
 * r2 = length
 *
 * Copies bytes from user space to kernel space
 */
ENTRY(copyin)
	cmp	r2, #0x00
	movle	r0, #0x00
	RETc(le)			/* Bail early if length is <= 0 */

	push	{r10-r11, lr}

	GET_CURPCB(r10)

	mov	r3, #0x00
	adr	ip, .Lcopyin_fault
	ldr	r11, [r10, #PCB_ONFAULT]
	str	ip, [r10, #PCB_ONFAULT]
	bl	.Lcopyin_guts
	str	r11, [r10, #PCB_ONFAULT]
	mov	r0, #0x00
	pop	{r10-r11, pc}

.Lcopyin_fault:
	str	r11, [r10, #PCB_ONFAULT]
	cmp	r3, #0x00
	popgt	{r4-r7}		/* r3 > 0 Restore r4-r7 */
	poplt	{r4-r9}		/* r3 < 0 Restore r4-r9 */
	pop	{r10-r11, pc}

.Lcopyin_guts:
	pld	[r0]
	/* Word-align the destination buffer */
	ands	ip, r1, #0x03		/* Already word aligned? */
	beq	.Lcopyin_wordaligned	/* Yup */
	rsb	ip, ip, #0x04
	cmp	r2, ip			/* Enough bytes left to align it? */
	blt	.Lcopyin_l4_2		/* Nope. Just copy bytewise */
	sub	r2, r2, ip
	rsbs	ip, ip, #0x03
	addne	pc, pc, ip, lsl #3
	nop
	ldrbt	ip, [r0], #0x01
	strb	ip, [r1], #0x01
	ldrbt	ip, [r0], #0x01
	strb	ip, [r1], #0x01
	ldrbt	ip, [r0], #0x01
	strb	ip, [r1], #0x01
	cmp	r2, #0x00		/* All done? */
	RETc(eq)

	/* Destination buffer is now word aligned */
.Lcopyin_wordaligned:
	ands	ip, r0, #0x03		/* Is src also word-aligned? */
	bne	.Lcopyin_bad_align	/* Nope. Things just got bad */
	cmp	r2, #0x08		/* Less than 8 bytes remaining? */
	blt	.Lcopyin_w_less_than8

	/* Quad-align the destination buffer */
	tst	r1, #0x07		/* Already quad aligned? */
	ldrtne	ip, [r0], #0x04
	push	{r4-r9}		/* Free up some registers */
	mov	r3, #-1			/* Signal restore r4-r9 */
	tst	r1, #0x07		/* XXX: bug work-around */
	subne	r2, r2, #0x04
	strne	ip, [r1], #0x04

	/* Destination buffer quad aligned, source is word aligned */
	subs	r2, r2, #0x80
	blt	.Lcopyin_w_lessthan128

	/* Copy 128 bytes at a time */
.Lcopyin_w_loop128:
	ldrt	r4, [r0], #0x04		/* LD:00-03 */
	ldrt	r5, [r0], #0x04		/* LD:04-07 */
	pld	[r0, #0x18]		/* Prefetch 0x20 */
	ldrt	r6, [r0], #0x04		/* LD:08-0b */
	ldrt	r7, [r0], #0x04		/* LD:0c-0f */
	ldrt	r8, [r0], #0x04		/* LD:10-13 */
	ldrt	r9, [r0], #0x04		/* LD:14-17 */
	strd	r4, r5, [r1], #0x08	/* ST:00-07 */
	ldrt	r4, [r0], #0x04		/* LD:18-1b */
	ldrt	r5, [r0], #0x04		/* LD:1c-1f */
	strd	r6, r7, [r1], #0x08	/* ST:08-0f */
	ldrt	r6, [r0], #0x04		/* LD:20-23 */
	ldrt	r7, [r0], #0x04		/* LD:24-27 */
	pld	[r0, #0x18]		/* Prefetch 0x40 */
	strd	r8, r9, [r1], #0x08	/* ST:10-17 */
	ldrt	r8, [r0], #0x04		/* LD:28-2b */
	ldrt	r9, [r0], #0x04		/* LD:2c-2f */
	strd	r4, r5, [r1], #0x08	/* ST:18-1f */
	ldrt	r4, [r0], #0x04		/* LD:30-33 */
	ldrt	r5, [r0], #0x04		/* LD:34-37 */
	strd	r6, r7, [r1], #0x08		/* ST:20-27 */
	ldrt	r6, [r0], #0x04		/* LD:38-3b */
	ldrt	r7, [r0], #0x04		/* LD:3c-3f */
	strd	r8, r9, [r1], #0x08	/* ST:28-2f */
	ldrt	r8, [r0], #0x04		/* LD:40-43 */
	ldrt	r9, [r0], #0x04		/* LD:44-47 */
	pld	[r0, #0x18]		/* Prefetch 0x60 */
	strd	r4, r5, [r1], #0x08	/* ST:30-37 */
	ldrt	r4, [r0], #0x04		/* LD:48-4b */
	ldrt	r5, [r0], #0x04		/* LD:4c-4f */
	strd	r6, r7, [r1], #0x08	/* ST:38-3f */
	ldrt	r6, [r0], #0x04		/* LD:50-53 */
	ldrt	r7, [r0], #0x04		/* LD:54-57 */
	strd	r8, r9, [r1], #0x08	/* ST:40-47 */
	ldrt	r8, [r0], #0x04		/* LD:58-5b */
	ldrt	r9, [r0], #0x04		/* LD:5c-5f */
	strd	r4, r5, [r1], #0x08	/* ST:48-4f */
	ldrt	r4, [r0], #0x04		/* LD:60-63 */
	ldrt	r5, [r0], #0x04		/* LD:64-67 */
	pld	[r0, #0x18]		/* Prefetch 0x80 */
	strd	r6, r7, [r1], #0x08	/* ST:50-57 */
	ldrt	r6, [r0], #0x04		/* LD:68-6b */
	ldrt	r7, [r0], #0x04		/* LD:6c-6f */
	strd	r8, r9, [r1], #0x08	/* ST:58-5f */
	ldrt	r8, [r0], #0x04		/* LD:70-73 */
	ldrt	r9, [r0], #0x04		/* LD:74-77 */
	strd	r4, r5, [r1], #0x08	/* ST:60-67 */
	ldrt	r4, [r0], #0x04		/* LD:78-7b */
	ldrt	r5, [r0], #0x04		/* LD:7c-7f */
	strd	r6, r7, [r1], #0x08	/* ST:68-6f */
	strd	r8, r9, [r1], #0x08	/* ST:70-77 */
	subs	r2, r2, #0x80
	strd	r4, r5, [r1], #0x08	/* ST:78-7f */
	bge	.Lcopyin_w_loop128

.Lcopyin_w_lessthan128:
	adds	r2, r2, #0x80		/* Adjust for extra sub */
	popeq	{r4-r9}
	RETc(eq)			/* Return now if done */
	subs	r2, r2, #0x20
	blt	.Lcopyin_w_lessthan32

	/* Copy 32 bytes at a time */
.Lcopyin_w_loop32:
	ldrt	r4, [r0], #0x04
	ldrt	r5, [r0], #0x04
	pld	[r0, #0x18]
	ldrt	r6, [r0], #0x04
	ldrt	r7, [r0], #0x04
	ldrt	r8, [r0], #0x04
	ldrt	r9, [r0], #0x04
	strd	r4, r5, [r1], #0x08
	ldrt	r4, [r0], #0x04
	ldrt	r5, [r0], #0x04
	strd	r6, r7, [r1], #0x08
	strd	r8, r9, [r1], #0x08
	subs	r2, r2, #0x20
	strd	r4, r5, [r1], #0x08
	bge	.Lcopyin_w_loop32

.Lcopyin_w_lessthan32:
	adds	r2, r2, #0x20		/* Adjust for extra sub */
	popeq	{r4-r9}
	RETc(eq)			/* Return now if done */

	and	r4, r2, #0x18
	rsb	r5, r4, #0x18
	subs	r2, r2, r4
	add	pc, pc, r5, lsl #1
	nop

	/* At least 24 bytes remaining */
	ldrt	r4, [r0], #0x04
	ldrt	r5, [r0], #0x04
	nop
	strd	r4, r5, [r1], #0x08

	/* At least 16 bytes remaining */
	ldrt	r4, [r0], #0x04
	ldrt	r5, [r0], #0x04
	nop
	strd	r4, r5, [r1], #0x08

	/* At least 8 bytes remaining */
	ldrt	r4, [r0], #0x04
	ldrt	r5, [r0], #0x04
	nop
	strd	r4, r5, [r1], #0x08

	/* Less than 8 bytes remaining */
	pop	{r4-r9}
	RETc(eq)			/* Return now if done */
	mov	r3, #0x00

.Lcopyin_w_less_than8:
	subs	r2, r2, #0x04
	ldrtge	ip, [r0], #0x04
	strge	ip, [r1], #0x04
	RETc(eq)			/* Return now if done */
	addlt	r2, r2, #0x04
	ldrbt	ip, [r0], #0x01
	cmp	r2, #0x02
	ldrbtge	r2, [r0], #0x01
	strb	ip, [r1], #0x01
	ldrbtgt	ip, [r0]
	strbge	r2, [r1], #0x01
	strbgt	ip, [r1]
	RET

/*
 * At this point, it has not been possible to word align both buffers.
 * The destination buffer (r1) is word aligned, but the source buffer
 * (r0) is not.
 */
.Lcopyin_bad_align:
	push	{r4-r7}
	mov	r3, #0x01
	bic	r0, r0, #0x03
	cmp	ip, #2
	ldrt	ip, [r0], #0x04
	bgt	.Lcopyin_bad3
	beq	.Lcopyin_bad2
	b	.Lcopyin_bad1

.Lcopyin_bad1_loop16:
#ifdef __ARMEB__
	mov	r4, ip, lsl #8
#else
	mov	r4, ip, lsr #8
#endif
	ldrt	r5, [r0], #0x04
	pld	[r0, #0x018]
	ldrt	r6, [r0], #0x04
	ldrt	r7, [r0], #0x04
	ldrt	ip, [r0], #0x04
#ifdef __ARMEB__
	orr	r4, r4, r5, lsr #24
	mov	r5, r5, lsl #8
	orr	r5, r5, r6, lsr #24
	mov	r6, r6, lsl #8
	orr	r6, r6, r7, lsr #24
	mov	r7, r7, lsl #8
	orr	r7, r7, ip, lsr #24
#else
	orr	r4, r4, r5, lsl #24
	mov	r5, r5, lsr #8
	orr	r5, r5, r6, lsl #24
	mov	r6, r6, lsr #8
	orr	r6, r6, r7, lsl #24
	mov	r7, r7, lsr #8
	orr	r7, r7, ip, lsl #24
#endif
	str	r4, [r1], #0x04
	str	r5, [r1], #0x04
	str	r6, [r1], #0x04
	str	r7, [r1], #0x04
.Lcopyin_bad1:
	subs	r2, r2, #0x10         
	bge	.Lcopyin_bad1_loop16

	adds	r2, r2, #0x10         
	popeq	{r4-r7}
	RETc(eq)			/* Return now if done */
	subs	r2, r2, #0x04
	sublt	r0, r0, #0x03
	blt	.Lcopyin_l4

.Lcopyin_bad1_loop4:
#ifdef __ARMEB__
	mov	r4, ip, lsl #8
#else
	mov	r4, ip, lsr #8
#endif
	ldrt	ip, [r0], #0x04
	subs	r2, r2, #0x04
#ifdef __ARMEB__
	orr	r4, r4, ip, lsr #24
#else
	orr	r4, r4, ip, lsl #24
#endif
	str	r4, [r1], #0x04
	bge	.Lcopyin_bad1_loop4
	sub	r0, r0, #0x03
	b	.Lcopyin_l4

.Lcopyin_bad2_loop16:
#ifdef __ARMEB__
	mov	r4, ip, lsl #16
#else
	mov	r4, ip, lsr #16
#endif
	ldrt	r5, [r0], #0x04
	pld	[r0, #0x018]
	ldrt	r6, [r0], #0x04
	ldrt	r7, [r0], #0x04
	ldrt	ip, [r0], #0x04
#ifdef __ARMEB__
	orr	r4, r4, r5, lsr #16
	mov	r5, r5, lsl #16
	orr	r5, r5, r6, lsr #16
	mov	r6, r6, lsl #16
	orr	r6, r6, r7, lsr #16
	mov	r7, r7, lsl #16
	orr	r7, r7, ip, lsr #16
#else
	orr	r4, r4, r5, lsl #16
	mov	r5, r5, lsr #16
	orr	r5, r5, r6, lsl #16
	mov	r6, r6, lsr #16
	orr	r6, r6, r7, lsl #16
	mov	r7, r7, lsr #16
	orr	r7, r7, ip, lsl #16
#endif
	str	r4, [r1], #0x04
	str	r5, [r1], #0x04
	str	r6, [r1], #0x04
	str	r7, [r1], #0x04
.Lcopyin_bad2:
	subs	r2, r2, #0x10         
	bge	.Lcopyin_bad2_loop16

	adds	r2, r2, #0x10         
	popeq	{r4-r7}
	RETc(eq)			/* Return now if done */
	subs	r2, r2, #0x04
	sublt	r0, r0, #0x02
	blt	.Lcopyin_l4

.Lcopyin_bad2_loop4:
#ifdef __ARMEB__
	mov	r4, ip, lsl #16
#else
	mov	r4, ip, lsr #16
#endif
	ldrt	ip, [r0], #0x04
	subs	r2, r2, #0x04
#ifdef __ARMEB__
	orr	r4, r4, ip, lsr #16
#else
	orr	r4, r4, ip, lsl #16
#endif
	str	r4, [r1], #0x04
	bge	.Lcopyin_bad2_loop4
	sub	r0, r0, #0x02
	b	.Lcopyin_l4

.Lcopyin_bad3_loop16:
#ifdef __ARMEB__
	mov	r4, ip, lsl #24
#else
	mov	r4, ip, lsr #24
#endif
	ldrt	r5, [r0], #0x04
	pld	[r0, #0x018]
	ldrt	r6, [r0], #0x04
	ldrt	r7, [r0], #0x04
	ldrt	ip, [r0], #0x04
#ifdef __ARMEB__
	orr	r4, r4, r5, lsr #8
	mov	r5, r5, lsl #24
	orr	r5, r5, r6, lsr #8
	mov	r6, r6, lsl #24
	orr	r6, r6, r7, lsr #8
	mov	r7, r7, lsl #24
	orr	r7, r7, ip, lsr #8
#else
	orr	r4, r4, r5, lsl #8
	mov	r5, r5, lsr #24
	orr	r5, r5, r6, lsl #8
	mov	r6, r6, lsr #24
	orr	r6, r6, r7, lsl #8
	mov	r7, r7, lsr #24
	orr	r7, r7, ip, lsl #8
#endif
	str	r4, [r1], #0x04
	str	r5, [r1], #0x04
	str	r6, [r1], #0x04
	str	r7, [r1], #0x04
.Lcopyin_bad3:
	subs	r2, r2, #0x10         
	bge	.Lcopyin_bad3_loop16

	adds	r2, r2, #0x10         
	popeq	{r4-r7}
	RETc(eq)			/* Return now if done */
	subs	r2, r2, #0x04
	sublt	r0, r0, #0x01
	blt	.Lcopyin_l4

.Lcopyin_bad3_loop4:
#ifdef __ARMEB__
	mov	r4, ip, lsl #24
#else
	mov	r4, ip, lsr #24
#endif
	ldrt	ip, [r0], #0x04
	subs	r2, r2, #0x04
#ifdef __ARMEB__
	orr	r4, r4, ip, lsr #8
#else
	orr	r4, r4, ip, lsl #8
#endif
	str	r4, [r1], #0x04
	bge	.Lcopyin_bad3_loop4
	sub	r0, r0, #0x01

.Lcopyin_l4:
	pop	{r4-r7}
	mov	r3, #0x00
	adds	r2, r2, #0x04
	RETc(eq)	
.Lcopyin_l4_2:
	rsbs	r2, r2, #0x03
	addne	pc, pc, r2, lsl #3
	nop
	ldrbt	ip, [r0], #0x01
	strb	ip, [r1], #0x01
	ldrbt	ip, [r0], #0x01
	strb	ip, [r1], #0x01
	ldrbt	ip, [r0]
	strb	ip, [r1]
	RET
END(copyin)


/*
 * r0 = kernel space address
 * r1 = user space address
 * r2 = length
 *
 * Copies bytes from kernel space to user space
 */
ENTRY(copyout)
	cmp	r2, #0x00
	movle	r0, #0x00
	RETc(le)			/* Bail early if length is <= 0 */

	push	{r10-r11, lr}

	GET_CURPCB(r10)

	mov	r3, #0x00
	adr	ip, .Lcopyout_fault
	ldr	r11, [r10, #PCB_ONFAULT]
	str	ip, [r10, #PCB_ONFAULT]
	bl	.Lcopyout_guts
	str	r11, [r10, #PCB_ONFAULT]
	mov	r0, #0x00
	pop	{r10-r11, pc}

.Lcopyout_fault:
	str	r11, [r10, #PCB_ONFAULT]
	cmp	r3, #0x00
	popgt	{r4-r7}		/* r3 > 0 Restore r4-r7 */
	poplt	{r4-r9}		/* r3 < 0 Restore r4-r9 */
	pop	{r10-r11, pc}

.Lcopyout_guts:
	pld	[r0]
	/* Word-align the destination buffer */
	ands	ip, r1, #0x03		/* Already word aligned? */
	beq	.Lcopyout_wordaligned	/* Yup */
	rsb	ip, ip, #0x04
	cmp	r2, ip			/* Enough bytes left to align it? */
	blt	.Lcopyout_l4_2		/* Nope. Just copy bytewise */
	sub	r2, r2, ip
	rsbs	ip, ip, #0x03
	addne	pc, pc, ip, lsl #3
	nop
	ldrb	ip, [r0], #0x01
	strbt	ip, [r1], #0x01
	ldrb	ip, [r0], #0x01
	strbt	ip, [r1], #0x01
	ldrb	ip, [r0], #0x01
	strbt	ip, [r1], #0x01
	cmp	r2, #0x00		/* All done? */
	RETc(eq)

	/* Destination buffer is now word aligned */
.Lcopyout_wordaligned:
	ands	ip, r0, #0x03		/* Is src also word-aligned? */
	bne	.Lcopyout_bad_align	/* Nope. Things just got bad */
	cmp	r2, #0x08		/* Less than 8 bytes remaining? */
	blt	.Lcopyout_w_less_than8

	/* Quad-align the destination buffer */
	tst	r1, #0x07		/* Already quad aligned? */
	ldrne	ip, [r0], #0x04
	push	{r4-r9}		/* Free up some registers */
	mov	r3, #-1			/* Signal restore r4-r9 */
	tst	r1, #0x07		/* XXX: bug work-around */
	subne	r2, r2, #0x04
	strtne	ip, [r1], #0x04

	/* Destination buffer quad aligned, source is word aligned */
	subs	r2, r2, #0x80
	blt	.Lcopyout_w_lessthan128

	/* Copy 128 bytes at a time */
.Lcopyout_w_loop128:
	ldr	r4, [r0], #0x04		/* LD:00-03 */
	ldr	r5, [r0], #0x04		/* LD:04-07 */
	pld	[r0, #0x18]		/* Prefetch 0x20 */
	ldr	r6, [r0], #0x04		/* LD:08-0b */
	ldr	r7, [r0], #0x04		/* LD:0c-0f */
	ldr	r8, [r0], #0x04		/* LD:10-13 */
	ldr	r9, [r0], #0x04		/* LD:14-17 */
	strt	r4, [r1], #0x04		/* ST:00-03 */
	strt	r5, [r1], #0x04		/* ST:04-07 */
	ldr	r4, [r0], #0x04		/* LD:18-1b */
	ldr	r5, [r0], #0x04		/* LD:1c-1f */
	strt	r6, [r1], #0x04		/* ST:08-0b */
	strt	r7, [r1], #0x04		/* ST:0c-0f */
	ldr	r6, [r0], #0x04		/* LD:20-23 */
	ldr	r7, [r0], #0x04		/* LD:24-27 */
	pld	[r0, #0x18]		/* Prefetch 0x40 */
	strt	r8, [r1], #0x04		/* ST:10-13 */
	strt	r9, [r1], #0x04		/* ST:14-17 */
	ldr	r8, [r0], #0x04		/* LD:28-2b */
	ldr	r9, [r0], #0x04		/* LD:2c-2f */
	strt	r4, [r1], #0x04		/* ST:18-1b */
	strt	r5, [r1], #0x04		/* ST:1c-1f */
	ldr	r4, [r0], #0x04		/* LD:30-33 */
	ldr	r5, [r0], #0x04		/* LD:34-37 */
	strt	r6, [r1], #0x04		/* ST:20-23 */
	strt	r7, [r1], #0x04		/* ST:24-27 */
	ldr	r6, [r0], #0x04		/* LD:38-3b */
	ldr	r7, [r0], #0x04		/* LD:3c-3f */
	strt	r8, [r1], #0x04		/* ST:28-2b */
	strt	r9, [r1], #0x04		/* ST:2c-2f */
	ldr	r8, [r0], #0x04		/* LD:40-43 */
	ldr	r9, [r0], #0x04		/* LD:44-47 */
	pld	[r0, #0x18]		/* Prefetch 0x60 */
	strt	r4, [r1], #0x04		/* ST:30-33 */
	strt	r5, [r1], #0x04		/* ST:34-37 */
	ldr	r4, [r0], #0x04		/* LD:48-4b */
	ldr	r5, [r0], #0x04		/* LD:4c-4f */
	strt	r6, [r1], #0x04		/* ST:38-3b */
	strt	r7, [r1], #0x04		/* ST:3c-3f */
	ldr	r6, [r0], #0x04		/* LD:50-53 */
	ldr	r7, [r0], #0x04		/* LD:54-57 */
	strt	r8, [r1], #0x04		/* ST:40-43 */
	strt	r9, [r1], #0x04		/* ST:44-47 */
	ldr	r8, [r0], #0x04		/* LD:58-5b */
	ldr	r9, [r0], #0x04		/* LD:5c-5f */
	strt	r4, [r1], #0x04		/* ST:48-4b */
	strt	r5, [r1], #0x04		/* ST:4c-4f */
	ldr	r4, [r0], #0x04		/* LD:60-63 */
	ldr	r5, [r0], #0x04		/* LD:64-67 */
	pld	[r0, #0x18]		/* Prefetch 0x80 */
	strt	r6, [r1], #0x04		/* ST:50-53 */
	strt	r7, [r1], #0x04		/* ST:54-57 */
	ldr	r6, [r0], #0x04		/* LD:68-6b */
	ldr	r7, [r0], #0x04		/* LD:6c-6f */
	strt	r8, [r1], #0x04		/* ST:58-5b */
	strt	r9, [r1], #0x04		/* ST:5c-5f */
	ldr	r8, [r0], #0x04		/* LD:70-73 */
	ldr	r9, [r0], #0x04		/* LD:74-77 */
	strt	r4, [r1], #0x04		/* ST:60-63 */
	strt	r5, [r1], #0x04		/* ST:64-67 */
	ldr	r4, [r0], #0x04		/* LD:78-7b */
	ldr	r5, [r0], #0x04		/* LD:7c-7f */
	strt	r6, [r1], #0x04		/* ST:68-6b */
	strt	r7, [r1], #0x04		/* ST:6c-6f */
	strt	r8, [r1], #0x04		/* ST:70-73 */
	strt	r9, [r1], #0x04		/* ST:74-77 */
	subs	r2, r2, #0x80
	strt	r4, [r1], #0x04		/* ST:78-7b */
	strt	r5, [r1], #0x04		/* ST:7c-7f */
	bge	.Lcopyout_w_loop128

.Lcopyout_w_lessthan128:
	adds	r2, r2, #0x80		/* Adjust for extra sub */
	popeq	{r4-r9}
	RETc(eq)			/* Return now if done */
	subs	r2, r2, #0x20
	blt	.Lcopyout_w_lessthan32

	/* Copy 32 bytes at a time */
.Lcopyout_w_loop32:
	ldr	r4, [r0], #0x04
	ldr	r5, [r0], #0x04
	pld	[r0, #0x18]
	ldr	r6, [r0], #0x04
	ldr	r7, [r0], #0x04
	ldr	r8, [r0], #0x04
	ldr	r9, [r0], #0x04
	strt	r4, [r1], #0x04
	strt	r5, [r1], #0x04
	ldr	r4, [r0], #0x04
	ldr	r5, [r0], #0x04
	strt	r6, [r1], #0x04
	strt	r7, [r1], #0x04
	strt	r8, [r1], #0x04
	strt	r9, [r1], #0x04
	subs	r2, r2, #0x20
	strt	r4, [r1], #0x04
	strt	r5, [r1], #0x04
	bge	.Lcopyout_w_loop32

.Lcopyout_w_lessthan32:
	adds	r2, r2, #0x20		/* Adjust for extra sub */
	popeq	{r4-r9}
	RETc(eq)			/* Return now if done */

	and	r4, r2, #0x18
	rsb	r5, r4, #0x18
	subs	r2, r2, r4
	add	pc, pc, r5, lsl #1
	nop

	/* At least 24 bytes remaining */
	ldr	r4, [r0], #0x04
	ldr	r5, [r0], #0x04
	strt	r4, [r1], #0x04
	strt	r5, [r1], #0x04

	/* At least 16 bytes remaining */
	ldr	r4, [r0], #0x04
	ldr	r5, [r0], #0x04
	strt	r4, [r1], #0x04
	strt	r5, [r1], #0x04

	/* At least 8 bytes remaining */
	ldr	r4, [r0], #0x04
	ldr	r5, [r0], #0x04
	strt	r4, [r1], #0x04
	strt	r5, [r1], #0x04

	/* Less than 8 bytes remaining */
	pop	{r4-r9}
	RETc(eq)			/* Return now if done */
	mov	r3, #0x00

.Lcopyout_w_less_than8:
	subs	r2, r2, #0x04
	ldrge	ip, [r0], #0x04
	strtge	ip, [r1], #0x04
	RETc(eq)			/* Return now if done */
	addlt	r2, r2, #0x04
	ldrb	ip, [r0], #0x01
	cmp	r2, #0x02
	ldrbge	r2, [r0], #0x01
	strbt	ip, [r1], #0x01
	ldrbgt	ip, [r0]
	strbtge	r2, [r1], #0x01
	strbtgt	ip, [r1]
	RET

/*
 * At this point, it has not been possible to word align both buffers.
 * The destination buffer (r1) is word aligned, but the source buffer
 * (r0) is not.
 */
.Lcopyout_bad_align:
	push	{r4-r7}
	mov	r3, #0x01
	bic	r0, r0, #0x03
	cmp	ip, #2
	ldr	ip, [r0], #0x04
	bgt	.Lcopyout_bad3
	beq	.Lcopyout_bad2
	b	.Lcopyout_bad1

.Lcopyout_bad1_loop16:
#ifdef	__ARMEB__
	mov	r4, ip, lsl #8
#else
	mov	r4, ip, lsr #8
#endif
	ldr	r5, [r0], #0x04
	pld	[r0, #0x018]
	ldr	r6, [r0], #0x04
	ldr	r7, [r0], #0x04
	ldr	ip, [r0], #0x04
#ifdef	__ARMEB__
	orr	r4, r4, r5, lsr #24
	mov	r5, r5, lsl #8
	orr	r5, r5, r6, lsr #24
	mov	r6, r6, lsl #8
	orr	r6, r6, r7, lsr #24
	mov	r7, r7, lsl #8
	orr	r7, r7, ip, lsr #24
#else
	orr	r4, r4, r5, lsl #24
	mov	r5, r5, lsr #8
	orr	r5, r5, r6, lsl #24
	mov	r6, r6, lsr #8
	orr	r6, r6, r7, lsl #24
	mov	r7, r7, lsr #8
	orr	r7, r7, ip, lsl #24
#endif
	strt	r4, [r1], #0x04
	strt	r5, [r1], #0x04
	strt	r6, [r1], #0x04
	strt	r7, [r1], #0x04
.Lcopyout_bad1:
	subs	r2, r2, #0x10         
	bge	.Lcopyout_bad1_loop16

	adds	r2, r2, #0x10         
	popeq	{r4-r7}
	RETc(eq)			/* Return now if done */
	subs	r2, r2, #0x04
	sublt	r0, r0, #0x03
	blt	.Lcopyout_l4

.Lcopyout_bad1_loop4:
#ifdef __ARMEB__
	mov	r4, ip, lsl #8
#else
	mov	r4, ip, lsr #8
#endif
	ldr	ip, [r0], #0x04
	subs	r2, r2, #0x04
#ifdef __ARMEB__
	orr	r4, r4, ip, lsr #24
#else
	orr	r4, r4, ip, lsl #24
#endif
	strt	r4, [r1], #0x04
	bge	.Lcopyout_bad1_loop4
	sub	r0, r0, #0x03
	b	.Lcopyout_l4

.Lcopyout_bad2_loop16:
#ifdef __ARMEB__
	mov	r4, ip, lsl #16
#else
	mov	r4, ip, lsr #16
#endif
	ldr	r5, [r0], #0x04
	pld	[r0, #0x018]
	ldr	r6, [r0], #0x04
	ldr	r7, [r0], #0x04
	ldr	ip, [r0], #0x04
#ifdef __ARMEB__
	orr	r4, r4, r5, lsr #16
	mov	r5, r5, lsl #16
	orr	r5, r5, r6, lsr #16
	mov	r6, r6, lsl #16
	orr	r6, r6, r7, lsr #16
	mov	r7, r7, lsl #16
	orr	r7, r7, ip, lsr #16
#else
	orr	r4, r4, r5, lsl #16
	mov	r5, r5, lsr #16
	orr	r5, r5, r6, lsl #16
	mov	r6, r6, lsr #16
	orr	r6, r6, r7, lsl #16
	mov	r7, r7, lsr #16
	orr	r7, r7, ip, lsl #16
#endif
	strt	r4, [r1], #0x04
	strt	r5, [r1], #0x04
	strt	r6, [r1], #0x04
	strt	r7, [r1], #0x04
.Lcopyout_bad2:
	subs	r2, r2, #0x10         
	bge	.Lcopyout_bad2_loop16

	adds	r2, r2, #0x10         
	popeq	{r4-r7}
	RETc(eq)			/* Return now if done */
	subs	r2, r2, #0x04
	sublt	r0, r0, #0x02
	blt	.Lcopyout_l4

.Lcopyout_bad2_loop4:
#ifdef __ARMEB__
	mov	r4, ip, lsl #16
#else
	mov	r4, ip, lsr #16
#endif
	ldr	ip, [r0], #0x04
	subs	r2, r2, #0x04
#ifdef __ARMEB__
	orr	r4, r4, ip, lsr #16
#else
	orr	r4, r4, ip, lsl #16
#endif
	strt	r4, [r1], #0x04
	bge	.Lcopyout_bad2_loop4
	sub	r0, r0, #0x02
	b	.Lcopyout_l4

.Lcopyout_bad3_loop16:
#ifdef __ARMEB__
	mov	r4, ip, lsl #24
#else
	mov	r4, ip, lsr #24
#endif
	ldr	r5, [r0], #0x04
	pld	[r0, #0x018]
	ldr	r6, [r0], #0x04
	ldr	r7, [r0], #0x04
	ldr	ip, [r0], #0x04
#ifdef __ARMEB__
	orr	r4, r4, r5, lsr #8
	mov	r5, r5, lsl #24
	orr	r5, r5, r6, lsr #8
	mov	r6, r6, lsl #24
	orr	r6, r6, r7, lsr #8
	mov	r7, r7, lsl #24
	orr	r7, r7, ip, lsr #8
#else
	orr	r4, r4, r5, lsl #8
	mov	r5, r5, lsr #24
	orr	r5, r5, r6, lsl #8
	mov	r6, r6, lsr #24
	orr	r6, r6, r7, lsl #8
	mov	r7, r7, lsr #24
	orr	r7, r7, ip, lsl #8
#endif
	strt	r4, [r1], #0x04
	strt	r5, [r1], #0x04
	strt	r6, [r1], #0x04
	strt	r7, [r1], #0x04
.Lcopyout_bad3:
	subs	r2, r2, #0x10         
	bge	.Lcopyout_bad3_loop16

	adds	r2, r2, #0x10         
	popeq	{r4-r7}
	RETc(eq)			/* Return now if done */
	subs	r2, r2, #0x04
	sublt	r0, r0, #0x01
	blt	.Lcopyout_l4

.Lcopyout_bad3_loop4:
#ifdef __ARMEB__
	mov	r4, ip, lsl #24
#else
	mov	r4, ip, lsr #24
#endif
	ldr	ip, [r0], #0x04
	subs	r2, r2, #0x04
#ifdef __ARMEB__
	orr	r4, r4, ip, lsr #8
#else
	orr	r4, r4, ip, lsl #8
#endif
	strt	r4, [r1], #0x04
	bge	.Lcopyout_bad3_loop4
	sub	r0, r0, #0x01

.Lcopyout_l4:
	pop	{r4-r7}
	mov	r3, #0x00
	adds	r2, r2, #0x04
	RETc(eq)
.Lcopyout_l4_2:
	rsbs	r2, r2, #0x03
	addne	pc, pc, r2, lsl #3
	nop
	ldrb	ip, [r0], #0x01
	strbt	ip, [r1], #0x01
	ldrb	ip, [r0], #0x01
	strbt	ip, [r1], #0x01
	ldrb	ip, [r0]
	strbt	ip, [r1]
	RET
END(copyout)

/*
 * r0 = kernel space source address
 * r1 = kernel space destination address
 * r2 = length
 *
 * Copies bytes from kernel space to kernel space, aborting on page fault
 */
ENTRY(kcopy)
	cmp	r2, #0x00
	movle	r0, #0x00
	RETc(le)			/* Bail early if length is <= 0 */

	push	{r10-r11, lr}

	GET_CURPCB(r10)

	mov	r3, #0x00
	adr	ip, .Lkcopy_fault
	ldr	r11, [r10, #PCB_ONFAULT]
	str	ip, [r10, #PCB_ONFAULT]
	bl	.Lkcopy_guts
	str	r11, [r10, #PCB_ONFAULT]
	mov	r0, #0x00
	pop	{r10-r11, pc}

.Lkcopy_fault:
	str	r11, [r10, #PCB_ONFAULT]
	cmp	r3, #0x00
	popgt	{r4-r7}		/* r3 > 0 Restore r4-r7 */
	poplt	{r4-r9}		/* r3 < 0 Restore r4-r9 */
	pop	{r10-r11, pc}

.Lkcopy_guts:
	pld	[r0]
	/* Word-align the destination buffer */
	ands	ip, r1, #0x03		/* Already word aligned? */
	beq	.Lkcopy_wordaligned	/* Yup */
	rsb	ip, ip, #0x04
	cmp	r2, ip			/* Enough bytes left to align it? */
	blt	.Lkcopy_bad_endgame2	/* Nope. Just copy bytewise */
	sub	r2, r2, ip
	rsbs	ip, ip, #0x03
	addne	pc, pc, ip, lsl #3
	nop
	ldrb	ip, [r0], #0x01
	strb	ip, [r1], #0x01
	ldrb	ip, [r0], #0x01
	strb	ip, [r1], #0x01
	ldrb	ip, [r0], #0x01
	strb	ip, [r1], #0x01
	cmp	r2, #0x00		/* All done? */
	RETc(eq)

	/* Destination buffer is now word aligned */
.Lkcopy_wordaligned:
	ands	ip, r0, #0x03		/* Is src also word-aligned? */
	bne	.Lkcopy_bad_align	/* Nope. Things just got bad */
	cmp	r2, #0x08		/* Less than 8 bytes remaining? */
	blt	.Lkcopy_w_less_than8

	/* Quad-align the destination buffer */
	tst	r1, #0x07		/* Already quad aligned? */
	ldrne	ip, [r0], #0x04
	push	{r4-r9}		/* Free up some registers */
	mov	r3, #-1			/* Signal restore r4-r9 */
	subne	r2, r2, #0x04
	strne	ip, [r1], #0x04

	/* Destination buffer quad aligned, source is word aligned */
	subs	r2, r2, #0x80
	blt	.Lkcopy_w_lessthan128

	/* Copy 128 bytes at a time */
.Lkcopy_w_loop128:
	ldr	r4, [r0], #0x04		/* LD:00-03 */
	ldr	r5, [r0], #0x04		/* LD:04-07 */
	pld	[r0, #0x18]		/* Prefetch 0x20 */
	ldr	r6, [r0], #0x04		/* LD:08-0b */
	ldr	r7, [r0], #0x04		/* LD:0c-0f */
	ldr	r8, [r0], #0x04		/* LD:10-13 */
	ldr	r9, [r0], #0x04		/* LD:14-17 */
	strd	r4, r5, [r1], #0x08	/* ST:00-07 */
	ldr	r4, [r0], #0x04		/* LD:18-1b */
	ldr	r5, [r0], #0x04		/* LD:1c-1f */
	strd	r6, r7, [r1], #0x08	/* ST:08-0f */
	ldr	r6, [r0], #0x04		/* LD:20-23 */
	ldr	r7, [r0], #0x04		/* LD:24-27 */
	pld	[r0, #0x18]		/* Prefetch 0x40 */
	strd	r8, r9, [r1], #0x08	/* ST:10-17 */
	ldr	r8, [r0], #0x04		/* LD:28-2b */
	ldr	r9, [r0], #0x04		/* LD:2c-2f */
	strd	r4, r5, [r1], #0x08	/* ST:18-1f */
	ldr	r4, [r0], #0x04		/* LD:30-33 */
	ldr	r5, [r0], #0x04		/* LD:34-37 */
	strd	r6, r7, [r1], #0x08	/* ST:20-27 */
	ldr	r6, [r0], #0x04		/* LD:38-3b */
	ldr	r7, [r0], #0x04		/* LD:3c-3f */
	strd	r8, r9, [r1], #0x08	/* ST:28-2f */
	ldr	r8, [r0], #0x04		/* LD:40-43 */
	ldr	r9, [r0], #0x04		/* LD:44-47 */
	pld	[r0, #0x18]		/* Prefetch 0x60 */
	strd	r4, r5, [r1], #0x08	/* ST:30-37 */
	ldr	r4, [r0], #0x04		/* LD:48-4b */
	ldr	r5, [r0], #0x04		/* LD:4c-4f */
	strd	r6, r7, [r1], #0x08	/* ST:38-3f */
	ldr	r6, [r0], #0x04		/* LD:50-53 */
	ldr	r7, [r0], #0x04		/* LD:54-57 */
	strd	r8, r9, [r1], #0x08	/* ST:40-47 */
	ldr	r8, [r0], #0x04		/* LD:58-5b */
	ldr	r9, [r0], #0x04		/* LD:5c-5f */
	strd	r4, r5, [r1], #0x08	/* ST:48-4f */
	ldr	r4, [r0], #0x04		/* LD:60-63 */
	ldr	r5, [r0], #0x04		/* LD:64-67 */
	pld	[r0, #0x18]		/* Prefetch 0x80 */
	strd	r6, r7, [r1], #0x08	/* ST:50-57 */
	ldr	r6, [r0], #0x04		/* LD:68-6b */
	ldr	r7, [r0], #0x04		/* LD:6c-6f */
	strd	r8, r9, [r1], #0x08	/* ST:58-5f */
	ldr	r8, [r0], #0x04		/* LD:70-73 */
	ldr	r9, [r0], #0x04		/* LD:74-77 */
	strd	r4, r5, [r1], #0x08	/* ST:60-67 */
	ldr	r4, [r0], #0x04		/* LD:78-7b */
	ldr	r5, [r0], #0x04		/* LD:7c-7f */
	strd	r6, r7, [r1], #0x08	/* ST:68-6f */
	strd	r8, r9, [r1], #0x08	/* ST:70-77 */
	subs	r2, r2, #0x80
	strd	r4, r5, [r1], #0x08	/* ST:78-7f */
	bge	.Lkcopy_w_loop128

.Lkcopy_w_lessthan128:
	adds	r2, r2, #0x80		/* Adjust for extra sub */
	popeq	{r4-r9}
	RETc(eq)			/* Return now if done */
	subs	r2, r2, #0x20
	blt	.Lkcopy_w_lessthan32

	/* Copy 32 bytes at a time */
.Lkcopy_w_loop32:
	ldr	r4, [r0], #0x04
	ldr	r5, [r0], #0x04
	pld	[r0, #0x18]
	ldr	r6, [r0], #0x04
	ldr	r7, [r0], #0x04
	ldr	r8, [r0], #0x04
	ldr	r9, [r0], #0x04
	strd	r4, r5, [r1], #0x08
	ldr	r4, [r0], #0x04
	ldr	r5, [r0], #0x04
	strd	r6, r7, [r1], #0x08
	strd	r8, r9, [r1], #0x08
	subs	r2, r2, #0x20
	strd	r4, r5, [r1], #0x08
	bge	.Lkcopy_w_loop32

.Lkcopy_w_lessthan32:
	adds	r2, r2, #0x20		/* Adjust for extra sub */
	popeq	{r4-r9}
	RETc(eq)			/* Return now if done */

	and	r4, r2, #0x18
	rsb	r5, r4, #0x18
	subs	r2, r2, r4
	add	pc, pc, r5, lsl #1
	nop

	/* At least 24 bytes remaining */
	ldr	r4, [r0], #0x04
	ldr	r5, [r0], #0x04
	nop
	strd	r4, r5, [r1], #0x08

	/* At least 16 bytes remaining */
	ldr	r4, [r0], #0x04
	ldr	r5, [r0], #0x04
	nop
	strd	r4, r5, [r1], #0x08

	/* At least 8 bytes remaining */
	ldr	r4, [r0], #0x04
	ldr	r5, [r0], #0x04
	nop
	strd	r4, r5, [r1], #0x08

	/* Less than 8 bytes remaining */
	pop	{r4-r9}
	RETc(eq)			/* Return now if done */
	mov	r3, #0x00

.Lkcopy_w_less_than8:
	subs	r2, r2, #0x04
	ldrge	ip, [r0], #0x04
	strge	ip, [r1], #0x04
	RETc(eq)			/* Return now if done */
	addlt	r2, r2, #0x04
	ldrb	ip, [r0], #0x01
	cmp	r2, #0x02
	ldrbge	r2, [r0], #0x01
	strb	ip, [r1], #0x01
	ldrbgt	ip, [r0]
	strbge	r2, [r1], #0x01
	strbgt	ip, [r1]
	RET

/*
 * At this point, it has not been possible to word align both buffers.
 * The destination buffer (r1) is word aligned, but the source buffer
 * (r0) is not.
 */
.Lkcopy_bad_align:
	push	{r4-r7}
	mov	r3, #0x01
	bic	r0, r0, #0x03
	cmp	ip, #2
	ldr	ip, [r0], #0x04
	bgt	.Lkcopy_bad3
	beq	.Lkcopy_bad2
	b	.Lkcopy_bad1

.Lkcopy_bad1_loop16:
#ifdef __ARMEB__
	mov	r4, ip, lsl #8
#else
	mov	r4, ip, lsr #8
#endif
	ldr	r5, [r0], #0x04
	pld	[r0, #0x018]
	ldr	r6, [r0], #0x04
	ldr	r7, [r0], #0x04
	ldr	ip, [r0], #0x04
#ifdef __ARMEB__
	orr	r4, r4, r5, lsr #24
	mov	r5, r5, lsl #8
	orr	r5, r5, r6, lsr #24
	mov	r6, r6, lsl #8
	orr	r6, r6, r7, lsr #24
	mov	r7, r7, lsl #8
	orr	r7, r7, ip, lsr #24
#else
	orr	r4, r4, r5, lsl #24
	mov	r5, r5, lsr #8
	orr	r5, r5, r6, lsl #24
	mov	r6, r6, lsr #8
	orr	r6, r6, r7, lsl #24
	mov	r7, r7, lsr #8
	orr	r7, r7, ip, lsl #24
#endif
	str	r4, [r1], #0x04
	str	r5, [r1], #0x04
	str	r6, [r1], #0x04
	str	r7, [r1], #0x04
.Lkcopy_bad1:
	subs	r2, r2, #0x10         
	bge	.Lkcopy_bad1_loop16

	adds	r2, r2, #0x10         
	popeq	{r4-r7}
	RETc(eq)			/* Return now if done */
	subs	r2, r2, #0x04
	sublt	r0, r0, #0x03
	blt	.Lkcopy_bad_endgame

.Lkcopy_bad1_loop4:
#ifdef __ARMEB__
	mov	r4, ip, lsl #8
#else
	mov	r4, ip, lsr #8
#endif
	ldr	ip, [r0], #0x04
	subs	r2, r2, #0x04
#ifdef __ARMEB__
	orr	r4, r4, ip, lsr #24
#else
	orr	r4, r4, ip, lsl #24
#endif
	str	r4, [r1], #0x04
	bge	.Lkcopy_bad1_loop4
	sub	r0, r0, #0x03
	b	.Lkcopy_bad_endgame

.Lkcopy_bad2_loop16:
#ifdef __ARMEB__
	mov	r4, ip, lsl #16
#else
	mov	r4, ip, lsr #16
#endif
	ldr	r5, [r0], #0x04
	pld	[r0, #0x018]
	ldr	r6, [r0], #0x04
	ldr	r7, [r0], #0x04
	ldr	ip, [r0], #0x04
#ifdef __ARMEB__
	orr	r4, r4, r5, lsr #16
	mov	r5, r5, lsl #16
	orr	r5, r5, r6, lsr #16
	mov	r6, r6, lsl #16
	orr	r6, r6, r7, lsr #16
	mov	r7, r7, lsl #16
	orr	r7, r7, ip, lsr #16
#else
	orr	r4, r4, r5, lsl #16
	mov	r5, r5, lsr #16
	orr	r5, r5, r6, lsl #16
	mov	r6, r6, lsr #16
	orr	r6, r6, r7, lsl #16
	mov	r7, r7, lsr #16
	orr	r7, r7, ip, lsl #16
#endif
	str	r4, [r1], #0x04
	str	r5, [r1], #0x04
	str	r6, [r1], #0x04
	str	r7, [r1], #0x04
.Lkcopy_bad2:
	subs	r2, r2, #0x10         
	bge	.Lkcopy_bad2_loop16

	adds	r2, r2, #0x10         
	popeq	{r4-r7}
	RETc(eq)			/* Return now if done */
	subs	r2, r2, #0x04
	sublt	r0, r0, #0x02
	blt	.Lkcopy_bad_endgame

.Lkcopy_bad2_loop4:
#ifdef __ARMEB__
	mov	r4, ip, lsl #16
#else
	mov	r4, ip, lsr #16
#endif
	ldr	ip, [r0], #0x04
	subs	r2, r2, #0x04
#ifdef __ARMEB__
	orr	r4, r4, ip, lsr #16
#else
	orr	r4, r4, ip, lsl #16
#endif
	str	r4, [r1], #0x04
	bge	.Lkcopy_bad2_loop4
	sub	r0, r0, #0x02
	b	.Lkcopy_bad_endgame

.Lkcopy_bad3_loop16:
#ifdef __ARMEB__
	mov	r4, ip, lsl #24
#else
	mov	r4, ip, lsr #24
#endif
	ldr	r5, [r0], #0x04
	pld	[r0, #0x018]
	ldr	r6, [r0], #0x04
	ldr	r7, [r0], #0x04
	ldr	ip, [r0], #0x04
#ifdef __ARMEB__
	orr	r4, r4, r5, lsr #8
	mov	r5, r5, lsl #24
	orr	r5, r5, r6, lsr #8
	mov	r6, r6, lsl #24
	orr	r6, r6, r7, lsr #8
	mov	r7, r7, lsl #24
	orr	r7, r7, ip, lsr #8
#else
	orr	r4, r4, r5, lsl #8
	mov	r5, r5, lsr #24
	orr	r5, r5, r6, lsl #8
	mov	r6, r6, lsr #24
	orr	r6, r6, r7, lsl #8
	mov	r7, r7, lsr #24
	orr	r7, r7, ip, lsl #8
#endif
	str	r4, [r1], #0x04
	str	r5, [r1], #0x04
	str	r6, [r1], #0x04
	str	r7, [r1], #0x04
.Lkcopy_bad3:
	subs	r2, r2, #0x10         
	bge	.Lkcopy_bad3_loop16

	adds	r2, r2, #0x10         
	popeq	{r4-r7}
	RETc(eq)			/* Return now if done */
	subs	r2, r2, #0x04
	sublt	r0, r0, #0x01
	blt	.Lkcopy_bad_endgame

.Lkcopy_bad3_loop4:
#ifdef __ARMEB__
	mov	r4, ip, lsl #24
#else
	mov	r4, ip, lsr #24
#endif
	ldr	ip, [r0], #0x04
	subs	r2, r2, #0x04
#ifdef __ARMEB__
	orr	r4, r4, ip, lsr #8
#else
	orr	r4, r4, ip, lsl #8
#endif
	str	r4, [r1], #0x04
	bge	.Lkcopy_bad3_loop4
	sub	r0, r0, #0x01

.Lkcopy_bad_endgame:
	pop	{r4-r7}
	mov	r3, #0x00
	adds	r2, r2, #0x04
	RETc(eq)	
.Lkcopy_bad_endgame2:
	rsbs	r2, r2, #0x03
	addne	pc, pc, r2, lsl #3
	nop
	ldrb	ip, [r0], #0x01
	strb	ip, [r1], #0x01
	ldrb	ip, [r0], #0x01
	strb	ip, [r1], #0x01
	ldrb	ip, [r0]
	strb	ip, [r1]
	RET
END(kcopy)
