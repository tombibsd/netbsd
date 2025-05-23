/*	$NetBSD$	*/

/*
 * Copyright (c) 2000, 2016 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by RedBack Networks Inc. (Author: Bill Sommerfeld), and Maxime Villard.
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

/*
 * Copyright (c) 1999 Stefan Grefen
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
 *      This product includes software developed by the NetBSD 
 *      Foundation, Inc. and its contributors.  
 * 4. Neither the name of The NetBSD Foundation nor the names of its 
 *    contributors may be used to endorse or promote products derived  
 *    from this software without specific prior written permission.   
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR AND CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * MP startup ...
 * the stuff from cpu_spinup_trampoline to mp_startup is copied into the
 * first 640 KB.
 *
 * We startup the processors now when the kthreads become ready.
 * The steps are:
 *     1) Get the processors running kernel-code from a special
 *        page-table and stack page, do chip identification.
 *     2) halt the processors waiting for them to be enabled
 *        by a idle-thread
 */

#include "opt_mpbios.h"		/* for MPDEBUG */

#include "assym.h"
#include <machine/asm.h>
#include <machine/specialreg.h>
#include <machine/segments.h>
#include <machine/mpbiosvar.h>
#include <machine/i82489reg.h>
#include <machine/gdt.h>

#define _RELOC(x)	((x) - KERNBASE)
#define RELOC(x)	_RELOC(_C_LABEL(x))

#define _TRMP_LABEL(a)	a = . - _C_LABEL(cpu_spinup_trampoline) + MP_TRAMPOLINE

#ifdef MPDEBUG
/*
 * Debug code to stop aux. processors in various stages based on the
 * value in cpu_trace.
 *
 * %edi points at cpu_trace;
 * cpu_trace[0] is the "hold point";
 * cpu_trace[1] is the point which the CPU has reached;
 * cpu_trace[2] is the last value stored by HALTT.
 */
#define HALT(x) \
1:	movl (%edi),%ebx	;	\
	cmpl $x,%ebx		;	\
	jle 1b			;	\
	movl $x,4(%edi)		;
#define HALTT(x,y) \
	movl y,8(%edi)		;	\
	HALT(x)			;
#else
#define HALT(x)
#define HALTT(x,y)
#endif /* MPDEBUG */

	.global _C_LABEL(cpu_spinup_trampoline)
	.global _C_LABEL(cpu_spinup_trampoline_end)
	.global _C_LABEL(cpu_hatch)
	.global _C_LABEL(mp_pdirpa)
	.global _C_LABEL(local_apic)

	.text
	.align 4,0x0
	.code16
/* XXX ENTRY() */
LABEL(cpu_spinup_trampoline)
	cli
	xorw	%ax,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%ss
#ifdef __clang__
	lgdt (mptramp_gdt32_desc)	/* load flat descriptor table */
#else
	data32 addr32 lgdt (mptramp_gdt32_desc) /* load flat descriptor table */
#endif
	movl	%cr0, %eax	/* get cr0 */
	orl	$CR0_PE, %eax	/* enable protected mode */
	movl	%eax, %cr0	/* doit */
	ljmpl	$0x8, $mp_startup

_TRMP_LABEL(mp_startup)
	.code32

	movl	$0x10, %eax	/* data segment */
	movw	%ax, %ds
	movw	%ax, %ss
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs

	/* bootstrap stack end, with scratch space.. */
	movl	$(MP_TRAMPOLINE+PAGE_SIZE-16),%esp

#ifdef MPDEBUG
	leal	RELOC(cpu_trace),%edi
#endif

	HALT(0x1)
	/* First, reset the PSL. */
	pushl	$PSL_MBO
	popfl

	/* Enable PAE, SSE, and PSE if available */
	movl	%cr4,%eax
	orl	$(CR4_PAE|CR4_OSFXSR|CR4_OSXMMEXCPT),%eax
	movl	RELOC(pmap_largepages),%ecx
	orl	%ecx,%ecx
	jz	no_PSE
	orl	$CR4_PSE,%eax
no_PSE:
	movl	%eax,%cr4

	/*
	 * Set Long Mode Enable in EFER. Also enable the syscall extensions,
	 * and NOX if available.
	 */
	movl	$MSR_EFER,%ecx
	rdmsr
	xorl	%eax,%eax
	orl	$(EFER_LME|EFER_SCE),%eax
	movl	RELOC(nox_flag),%ebx
	cmpl	$0,%ebx
	je 	no_NOX
	orl	$(EFER_NXE),%eax
no_NOX:
	wrmsr

	/* Load %cr3. */
	movl	RELOC(mp_pdirpa),%ecx	/* guaranteed < 4G */
	movl	%ecx,%cr3		/* load PTD addr into MMU */

	/* Enable paging and the rest of it. */
	movl	%cr0,%eax
	orl	$(CR0_PE|CR0_PG|CR0_NE|CR0_TS|CR0_MP|CR0_WP|CR0_AM),%eax
	movl	%eax,%cr0

	jmp	mptramp_compat
mptramp_compat:

	movl	$GSEL(GDATA_SEL, SEL_KPL),%eax	/* switch to new segment */
	movl	%eax,%ds
	movl	%eax,%es
	movl	%eax,%ss

	movl	$mptramp_gdt64_desc,%eax
	lgdt	(%eax)
	movl	$mptramp_jmp64,%eax
	ljmp	*(%eax)

_TRMP_LABEL(mptramp_jmp64)
	.long	mptramp_longmode
	.word	GSEL(GCODE_SEL, SEL_KPL)

#define GDT_LIMIT	0x17	/* 23 = 3 * 8 - 1 */
_TRMP_LABEL(mptramp_gdt32)
	.quad 0x0000000000000000
	.quad 0x00cf9f000000ffff	/* CS */
	.quad 0x00cf93000000ffff	/* DS */
_TRMP_LABEL(mptramp_gdt32_desc)
	.word GDT_LIMIT
	.long mptramp_gdt32

_TRMP_LABEL(mptramp_gdt64)
	.quad 0x0000000000000000
	.quad 0x00af9a000000ffff	/* CS */
	.quad 0x00cf92000000ffff	/* DS */
_TRMP_LABEL(mptramp_gdt64_desc)
	.word GDT_LIMIT
	.long mptramp_gdt64
#undef	GDT_LIMIT

_TRMP_LABEL(mptramp_longmode)
	.code64
	movabsq	$_C_LABEL(cpu_spinup_trampoline_end),%rax
	jmp	*%rax


_C_LABEL(cpu_spinup_trampoline_end):	/* end of code copied to MP_TRAMPOLINE */

	/* Don't touch lapic until BP has done init sequence. */
1:
	movq	_C_LABEL(cpu_starting),%rdi
	pause
	testq	%rdi, %rdi
	jz	1b

	movq	CPU_INFO_IDLELWP(%rdi),%rsi
	movq	L_PCB(%rsi),%rsi

	movq	PCB_RSP(%rsi),%rsp
	movq	PCB_RBP(%rsi),%rbp

	movq	CPU_INFO_GDT(%rdi),%rax
	movw	$(MAXGDTSIZ-1),-10(%rsp)
	movq	%rax,-8(%rsp)
	lgdt	-10(%rsp)

	/* Switch address space. */
	movq	PCB_CR3(%rsi),%rax
	movq	%rax,%cr3
	movl	PCB_CR0(%rsi),%eax
	movq	%rax,%cr0
	call	_C_LABEL(cpu_hatch)
END(cpu_spinup_trampoline)

	.data
LABEL(mp_pdirpa)
	.quad	0
END(mp_pdirpa)
#ifdef MPDEBUG
LABEL(cpu_trace)
	.quad	0x40
	.quad	0xff
	.quad	0xff
END(cpu_trace)
#endif
