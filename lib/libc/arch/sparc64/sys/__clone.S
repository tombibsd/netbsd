/*	$NetBSD$	*/

/*-     
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe.
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

#include <sys/errno.h>

#include "SYS.h"

#ifdef WEAK_ALIAS
WEAK_ALIAS(clone, __clone)
#endif

/*
 * int __clone(int (*fn)(void *), void *stack, int flags, void *arg);
 */
ENTRY(__clone)
	save	%sp, -CC64FSZ, %sp

	/*
	 * Sanity checks: func and stack may not be NULL.
	 */
	brz,pn	%i0,8f		! func == NULL, bail
	 orcc	%i1, %g0, %o1	! setup stack arg for syscall, test
	bz,pn	%xcc, 8f	! stack == NULL, bail
	 mov	%i2, %o0	! setup flags arg for syscall

	/*
	 * Allocate "caller's" frame in the child stack as ABI
	 * requires.  Subtract BIAS - it will be 64-bit code.
	 *
	 * We pass the function and the argument to the child by
	 * stashing them at the bottom of the frame.  There they are
	 * safe from window spill would we need to take one as it's
	 * below the window save area.
	 */
	sub	%o1, CC64FSZ+BIAS, %o1		! make space
	stx	%i0, [%o1+CC64FSZ-16+BIAS]	! save func
	stx	%i3, [%o1+CC64FSZ-8+BIAS]	! save arg

	/*
	 * Args are now set up for system call as (flags, stack).
	 */
	mov	SYS___clone, %g1
	t	ST_SYSCALL
	bcs,pn	%xcc, 9f
	 tst	%o1		! %o1 (rv[1]) == 0 in parent
	bz	%xcc, 2f	! yes, parent
	 ldx	[%sp+CC64FSZ-16+BIAS], %l0	! grab the function...
	call	%l0 		! Call the clone's entry point.
	 ldx	[%sp+CC64FSZ-8+BIAS], %o0	! ...and the argument

	JUMP(_exit)
	/* NOTREACHED */

2:	ret
	 restore %g0, %o0, %o0

8:	restore %g0, EINVAL, %o0
	ERROR()
	/* NOTREACHED */

9:	restore %o0, %g0, %o0
	ERROR()
	/* NOTREACHED */
