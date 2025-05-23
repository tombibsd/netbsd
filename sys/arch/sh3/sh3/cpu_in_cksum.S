/*	$NetBSD$	*/

/*-
 * Copyright (c) 2000 SHIMIZU Ryo <ryo@misakimix.org>
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
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LOCORE
#define _LOCORE
#endif
#include <machine/endian.h>
#include <machine/asm.h>
#include "assym.h"

__KERNEL_RCSID(0, "$NetBSD$")


#define	reg_tmp0		r0
#define	reg_byte_swapped	r1
#define	reg_mlen		r2
#define	reg_tmp3		r3
#define	reg_m			r4
#define	reg_len			r5
#define	reg_off			r6
#define	reg_w			r6	/* recycle */
#define	reg_sum			r7


#define	REDUCE	\
	swap.w	reg_sum,reg_tmp0	; \
	extu.w	reg_sum,reg_sum		; \
	extu.w	reg_tmp0,reg_tmp0	; \
	add	reg_tmp0,reg_sum

#define	ROL	\
	shll8	reg_sum

#if _BYTE_ORDER == BIG_ENDIAN
#define	ADDB	\
	mov.b	@reg_w+,reg_tmp0	; \
	ROL				; \
	extu.b	reg_tmp0,reg_tmp0	; \
	add	reg_tmp0,reg_sum	; \
	not	reg_byte_swapped,reg_byte_swapped
#else
#define	ADDB	\
	mov.b	@reg_w+,reg_tmp0	; \
	extu.b	reg_tmp0,reg_tmp0	; \
	add	reg_tmp0,reg_sum	; \
	ROL				; \
	not	reg_byte_swapped,reg_byte_swapped
#endif


#define	ADDS	\
	mov.w	@reg_w+,reg_tmp0	; \
	extu.w	reg_tmp0,reg_tmp0	; \
	add	reg_tmp0,reg_sum

#define	ADDCL	\
	mov.l	@reg_w+,reg_tmp0	; \
	addc	reg_tmp0,reg_sum

#define	FORWARD1	\
	add	#-1,reg_mlen

#define	FORWARD2	\
	add	#-2,reg_mlen


/*
 * LINTSTUB: include <sys/param.h>
 * LINTSTUB: include <sys/mbuf.h>
 *
 * LINTSTUB: Func: int cpu_in_cksum(struct mbuf *m, int len, int off, uint32_t initial_sum);
 */
ENTRY(cpu_in_cksum)
	sts.l	pr,@-sp

	tst	reg_len, reg_len
	bt/s	mbuf_loop_done
	 mov	#0, reg_byte_swapped

.L_mbuf_skip:
	tst	reg_m, reg_m
	bt	out_of_mbufs

	mov.l	@(M_LEN, reg_m), reg_mlen
	cmp/gt	reg_off, reg_mlen	/* mlen > off ? */
	bt	.L_mbuf_found

	!! while (off >= mlen)
	mov.l	@(M_NEXT, reg_m), reg_m	! m = m->m_next
	bra	.L_mbuf_skip
	 sub	reg_mlen, reg_off	! off -= mlen


.L_mbuf_found: !! if (mlen > off)
	mov.l	@(M_DATA, reg_m), reg_tmp3
	sub	reg_off, reg_mlen	! mlen -= off
	bra	.L_mbuf_loop_enter
	 add	reg_tmp3, reg_off	! w = m->m_data + off

#undef	reg_off /* it is dead now and we recycle it for reg_w */


mbuf_loop:
	tst	reg_m,reg_m
	bt	out_of_mbufs

	mov.l	@(M_LEN,reg_m),reg_mlen
	tst	reg_mlen,reg_mlen
	bt/s	mbuf_loop_continue
	mov.l	@(M_DATA,reg_m),reg_w


	!! Entry point for mbuf loop.  We jump here after we have
	!! found the mbuf (reg_m) that contains data at the specified
	!! offset.  reg_mlen and reg_w were adjusted to point at the
	!! first interesting byte of data.
.L_mbuf_loop_enter:
	cmp/ge	reg_mlen,reg_len
	bt	1f
	mov	reg_len,reg_mlen
1:
	sub	reg_mlen,reg_len


	mov	reg_w,reg_tmp0
	tst	#1,reg_tmp0
	bt/s	1f
	REDUCE		/* 1st instruction break only reg_tmp0(r0) */
	ADDB
	FORWARD1
1:


	mov	#1,reg_tmp0
	cmp/gt	reg_tmp0,reg_mlen
	bf/s	1f
	mov	reg_w,reg_tmp0
	tst	#2,reg_tmp0
	bt/s	1f
	REDUCE		/* 1st instruction break only reg_tmp0(r0) */
	ADDS
	FORWARD2
1:



	mov	#127,reg_tmp0
	cmp/hi	reg_tmp0,reg_mlen
	bf	1f

do_cksum128:
	bsr	cksum128
	 nop

	mov	#127,reg_tmp0
	cmp/hi	reg_tmp0,reg_mlen
	bt	do_cksum128
1:


	bsr	cksum128mod
	 nop

	REDUCE

	mov	#1,reg_tmp0
	cmp/gt	reg_tmp0,reg_mlen
	bf	1f
	ADDS
	FORWARD2
1:

	mov	reg_mlen,r0
	tst	#1,r0
	bt	1f
	ADDB
1:


mbuf_loop_continue:
	mov.l	@(M_NEXT,reg_m),reg_m

	tst	reg_len,reg_len
	bf/s	mbuf_loop
mbuf_loop_done:


	tst	reg_byte_swapped,reg_byte_swapped
	bt/s	1f
	REDUCE		/* 1st instruction break only reg_tmp0(r0) */
	ROL
1:

	REDUCE
	REDUCE

in_cksum_return:
	not	reg_sum,r0
	lds.l	@sp+,pr
	rts
	 extu.w	r0,r0


out_of_mbufs:
	mova	.L_message_out_of_data,reg_tmp0
	mov.l	.L_printf,reg_tmp3

	mov.l	reg_sum,@-sp	/* save: call clobbered register */

	jsr	@reg_tmp3
	 mov	reg_tmp0,r4

	bra	in_cksum_return
	 mov.l	@sp+,reg_sum	/* restore */

	.align 2
.L_printf:
	.long	_C_LABEL(printf)

	.align 2	/* mova target */
.L_message_out_of_data:
	.asciz "cksum: out of data (%d byte short)\n"

	SET_ENTRY_SIZE(cpu_in_cksum)


	.align	2
cksum128mod:
	mov	reg_mlen,reg_tmp0
	and	#124,reg_tmp0
	sub	reg_tmp0,reg_mlen

	mov.l	.L_cksum128_tail_p,reg_tmp3
	sub	reg_tmp0,reg_tmp3
	jmp	@reg_tmp3
	 clrt

	.align	2
.L_cksum128_tail_p:
	.long	cksum128_tail


	.align	2
cksum128:
	add	#-128,reg_mlen
	clrt

cksum128_unroll:
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
	ADDCL
cksum128_tail:
	mov	#0,reg_tmp0
	rts
	 addc	reg_tmp0,reg_sum
