/*	$NetBSD$	*/

/*-
 * Copyright (C) 2005 NONAKA Kimihiro <nonaka@netbsd.org>
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
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <machine/asm.h>
#include <sys/bootblock.h>

ENTRY(start)
	mova	mbr_end, r0
	mov	r0, r11			/* r11: relocate address */

	mov.w	mbr_size, r2
	sub	r2, r0
	mov	r0, r10			/* r10: loaded address */

	mov.w	stack_offset, r1
	add	r1, r0
	mov	r0, r15			/* r15: stack pointer */

	/* relocate code */
	mova	jmp_start, r0
	mov	r0, r13
	add	r2, r13			/* calc jump address */

	mov	r10, r0
	mov	r11, r1
1:	mov.b	@r0+, r3
	dt	r2
	mov.b	r3, @r1
	bf/s	1b
	 add	#1, r1

	jmp	@r13
	 nop

	.align	2
jmp_start:
	/* enable cache */
	mov	#0, r4
	mov	#6, r0
	trapa	#0x3f

#ifndef	NO_BANNER
	/* print banner */
	mova	banner, r0
	bsr	message_crlf
	 mov	r0, r4
#endif

	/* search bootable partition */
	mov.w	part_offset, r12
	add	r11, r12		/* r12: pointer to partition entry */
	mov	#MBR_PART_COUNT, r8	/* r8: partition loop counter */
loop_part:
	mov.b	@(4, r12), r0
	cmp/eq	#MBR_PTYPE_UNUSED, r0
	bt	next_part

	/* check active partition */
	mov.b	@(0, r12), r0
	cmp/eq	#0x80, r0
	bf	next_part

	/* found bootable partition */
	mov.w	@(8, r12), r0		/* load unaligned 32bit data */
	mov	r0, r1
	mov.w	@(10, r12), r0
	extu.w	r1, r1
	shll16	r0
	or	r1, r0

	mov	r0, r3
	mova	found_sector, r0
	bra	boot_lba
	 mov.l	r3, @r0

next_part:
	dt	r8
	bf/s	loop_part
	 add	#16, r12

noos_error:
	/* Not found bootable partition */
	mova	ERR_NOOS, r0
error:
	bsr	message_crlf
	 mov	r0, r4
99:	bra	99b
	 nop

read_error:
	bra	error
	 mova	ERR_READ, r0

message_crlf:
	mov	#32, r0
	trapa	#0x3f
	mova	crlf, r0
	mov	r0, r4
	mov	#32, r0
	trapa	#0x3f
	rts
	 nop

read_sector_lba:
	mov	#1, r7
	mov	#2, r0
	trapa	#0x3f
	tst	r0, r0
	bf	read_error
	rts
	 nop

boot_lba:
	/* read PBR sector */
	mova	found_sector, r0
	mov	#0x40, r4
	mov.l	@r0, r5
	bsr	read_sector_lba
	 mov	r10, r6

	/* flush cache */
	mov	#0, r4
	mov	#6, r0
	trapa	#0x3f

	/* check signature */
	mov.b	@(0, r10), r0
	tst	r0, r0
	bt	noos_error		/* first byte non-zero */
	mov.w	magic_offset, r0
	mov.w	@(r0, r10), r1
	mov.w	magic, r2
	cmp/eq	r1, r2
	bf	noos_error		/* magic */

	/* now jump to PBR */
	mov	r10, r0
	jmp	@r10
	 nop


	.align	1
mbr_size:	.word	mbr_end - _C_LABEL(start)
	.align	1
stack_offset:	.word	0x1000
	.align	1
part_offset:	.word	MBR_PART_OFFSET
	.align	1
magic_offset:	.word	MBR_MAGIC_OFFSET

	.align	2
found_sector:	.long	0

#ifndef	NO_BANNER
	.align	2
banner:		.asciz	"\r\nNetBSD MBR boot"
#endif
	.align	2
crlf:		.asciz	"\r\n"

	.align	2
ERR_INVPART:	.asciz	"No active partition"
	.align	2
ERR_READ:	.asciz	"Disk read error"
	.align	2
ERR_NOOS:	.asciz	"No operating system"


/* space for mbr_dsn */
	. = _C_LABEL(start) + MBR_DSN_OFFSET
	.long	0

/* mbr_bootsel_magic */
	. = _C_LABEL(start) + MBR_BS_MAGIC_OFFSET
	.word	MBR_BS_MAGIC

/*
 * MBR partition table
 */
	. = _C_LABEL(start) + MBR_PART_OFFSET
_pbr_part0:
	.byte	0, 0, 0, 0, 0, 0, 0, 0	
	.byte	0, 0, 0, 0, 0, 0, 0, 0
_pbr_part1:
	.byte	0, 0, 0, 0, 0, 0, 0, 0	
	.byte	0, 0, 0, 0, 0, 0, 0, 0
_pbr_part2:
	.byte	0, 0, 0, 0, 0, 0, 0, 0	
	.byte	0, 0, 0, 0, 0, 0, 0, 0
_pbr_part3:
	.byte	0, 0, 0, 0, 0, 0, 0, 0	
	.byte	0, 0, 0, 0, 0, 0, 0, 0

	. = _C_LABEL(start) + MBR_MAGIC_OFFSET
magic:
	.word	MBR_MAGIC
mbr_end:
