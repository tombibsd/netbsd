/*	$NetBSD$	*/

/*	$OpenBSD: sbrk.S,v 1.7 2001/06/04 23:14:04 mickey Exp $	*/

/*
 * Copyright (c) 1999 Michael Shalayeff
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
 * IN NO EVENT SHALL THE AUTHOR OR HIS RELATIVES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF MIND, USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "SYS.h"

#if defined(LIBC_SCCS) && !defined(lint)
	RCSID("$NetBSD$")
#endif /* LIBC_SCCS and not lint */

	.global	_end

#ifdef WEAK_ALIAS
	WEAK_ALIAS(sbrk, _sbrk)
#endif

	.data
	.export	curbrk, data
curbrk:
	.long	_end

ENTRY(_sbrk,0)
#ifdef __PIC__
	addil	LT%curbrk, %r19
	ldw	RT%curbrk(%r1), %r1
	ldw	0(%r1), %t2
#else  /* !__PIC__ */
	ldil	L%curbrk, %t1
	ldw	R%curbrk(%t1), %t2
#endif /* !__PIC__ */
	add	%arg0, %t2, %arg0
	stw	%arg0, HPPA_FRAME_ARG(0)(%sp)

	SYSCALL(break)

#ifdef __PIC__
	addil	LT%curbrk, %r19
	ldw	RT%curbrk(%r1), %r1
#else  /* !__PIC__ */
	ldil	L%curbrk, %t1
	ldo	R%curbrk(%t1), %r1
#endif /* !__PIC__ */
	ldw	0(%r1), %ret0
	ldw	HPPA_FRAME_ARG(0)(%sp), %arg0
	bv	%r0(%rp)
	stw	%arg0, 0(%r1)

EXIT(sbrk)

	.end
