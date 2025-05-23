/*	$NetBSD$	*/

/*-
 * Copyright (c) 2001 Ben Harris
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
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Relocatable module header for boot32.
 */

#include <riscoscalls.h>

rmbase:
	b	rmstart			/* Start code */
	.word	0			/* Initialisation code */
	.word	0			/* Finalisation code */
	.word	0			/* Service call handler */
	.word	rmtitle - rmbase	/* Title string */
	.word	rmhelp - rmbase		/* Help string */
	.word	rmcmdtbl - rmbase	/* Help and command keyword table */
	.word	0			/* SWI chunk base number */
	.word	0			/* SWI handler code offset */
	.word	0			/* SWI decoding table offset */
	.word	0			/* SWI decoding code offset */
	.word	0			/* Messages filename offset */
	.word	flags - rmbase		/* Module flags extension offset */

rmtitle:
	.asciz	"boot32"
	.align
flags:
	.word	1			/* 32 bit compatible */

rmstart:
	/*
	 * This entry point is called when we're started up as an application,
	 * so we're allowed to use application workspace.  This is good,
	 * because most of our code is linked to run at 0x8000.
	 */
	mov	r0, #0x8000
	adr	r1, rmbase
	ldr	r2, Lsize
Lcopyloop:
	ldr	r3, [r1], #4
	str	r3, [r0], #4
	subs	r2, r2, #4
	bgt	Lcopyloop
	mov	r0, #0x8000
	mov	r1, #128*1024
LDsync_loop:
	ldrb	r3, [r0], #1
	subs	r1, r1, #1
	bgt	LDsync_loop
	ldr	r0, Lstart
	mov	pc, r0

	/*
	 * The size of the code/data to be moved is not `end - rmbase' but
	 * `__bss_start__ - rmbase' for the module is loaded into RISC OS
	 * based on the filesize where as NetBSD doesn't have to include all
	 * the bss space into the file itself. In some odd cases the
	 * relocatable module area can be smaller than the module + bss and
	 * thus bomb out.
	 */
Lsize:
	.word	__bss_start__ - rmbase
Lstart:
	.word	_start

rmcmdtbl:
	.asciz	"boot32"		/* Command text */
	.align
	.word	rmcmd_boot32 - rmbase	/* Command code */
	.byte	0x00			/* Min 0 parameters */
	.byte	0x00			/* No GSTransed parameters */
	.byte	0xff			/* Max 255 parameters */
	.byte	0x00			/* Normal command */
	.word	0			/* No syntax message */
	.word	0			/* No help message */

rmcmd_boot32:
	/* We're called with r0 -> command tail. */
	stmfd	r13!, {r14}
	mov	r2, r0
	mov	r0, #2
	adr	r1, rmtitle
	swi	XOS_Module
	ldmfd	r13!, {pc}
