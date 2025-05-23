/*	$NetBSD$	*/

/*-
 * Copyright (c) 1991, 1993, 1995,
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Havard Eidnes.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/syscall.h>
#include <mips/asm.h>
#include <mips/regdef.h>

#include "assym.h"

#if defined(LIBC_SCCS) && !defined(lint)
#if 0
	ASMSTR("from: @(#)setjmp.s	8.1 (Berkeley) 6/4/93")
#else
	ASMSTR("$NetBSD$")
#endif
#endif /* LIBC_SCCS and not lint */

/*
 * C library -- sigsetjmp, siglongjmp
 *
 *	siglongjmp(a,v)
 * will generate a "return(v)" from
 * the last call to
 *	sigsetjmp(a, savemask)
 * by restoring registers from the stack,
 * and dependent on savemask restores the
 * signal mask.
 */

LEAF(sigsetjmp)
	SETUP_GP
	SETUP_GP64(t0, sigsetjmp)
	INT_S	a1, _OFFSETOF_SC_MASK13(a0)	# save "savemask"
	bne	a1, 0x0, 1f			# do saving of signal mask?
	PTR_LA	t9, _C_LABEL(_setjmp)
	jr	t9

1:	PTR_LA	t9, _C_LABEL(setjmp)
	jr	t9
END(sigsetjmp)

LEAF(siglongjmp)
	SETUP_GP
	SETUP_GP64(t0, siglongjmp)
	INT_L	t1, _OFFSETOF_SC_MASK13(a0)	# get "savemask"
	bne	t1, 0x0, 1f			# restore signal mask?
	PTR_LA	t9, _C_LABEL(_longjmp)
	jr	t9

1:	PTR_LA	t9, _C_LABEL(longjmp)
	jr	t9
END(siglongjmp)
