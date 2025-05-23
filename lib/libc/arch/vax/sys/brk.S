/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
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

#include "SYS.h"

#if defined(SYSLIBC_SCCS) && !defined(lint)
	/* .asciz "@(#)brk.s	8.1 (Berkeley) 6/4/93" */
RCSID("$NetBSD$")
#endif /* SYSLIBC_SCCS and not lint */

	.globl	_C_LABEL(__curbrk)
	.globl	_C_LABEL(__minbrk)
	.hidden _C_LABEL(__curbrk)
	.hidden	_C_LABEL(__minbrk)

#ifdef WEAK_ALIAS
WEAK_ALIAS(brk, _brk)
#endif

ENTRY(_brk, 0)
	moval	_C_LABEL(__minbrk),%r5		# gtr > _end 
	cmpl	(%r5),4(%ap)			# gtr > _end 
	blequ	1f				#   is fine
	movl	(%r5),4(%ap)			# shrink back to _end
1:	chmk	$ SYS_break			# do it
	jcs	err
	movl	4(%ap),_C_LABEL(__curbrk)
	clrl	%r0
	ret
err:
	jmp	CERROR+2
END(_brk)
