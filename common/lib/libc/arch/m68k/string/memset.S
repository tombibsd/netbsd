/*	$NetBSD$	*/

/*-
 * Copyright (c) 1997 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by J.T. Conklin.
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

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department.
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

#include <machine/asm.h>

#if defined(LIBC_SCCS) && !defined(lint)
#if 0
	RCSID("from: @(#)bzero.s	5.1 (Berkeley) 5/12/90")
#else
	RCSID("$NetBSD$")
#endif
#endif /* LIBC_SCCS and not lint */

ENTRY(memset)
	movl	%d2,-(%sp)
	movl	8(%sp),%a0		| destination
	movl	16(%sp),%d1		| count
	movb	15(%sp),%d2		| get fill character

	/* 
	 * It isn't worth the overhead of aligning to {long}word boundries
	 * if the string is too short.
	 */
	cmpl	#15,%d1
	jlt	.Lbzbyte

	clrl %d0
	moveb	%d2,%d0
	movel	%d0,%d2
	lsll	#8,%d0
	orl	%d0,%d2
	lsll	#8,%d0	
	orl	%d0,%d2	
	lsll	#8,%d0	
	orl	%d0,%d2	

	/* word align */
	movl	%a0,%d0
	btst	#0,%d0			| if (dst & 1)
	jeq	.Lbzalgndw		| 
	movb	%d2,(%a0)+		|	*(char *)dst++ = X
	subql	#1,%d1			|	len--
	addql	#1,%d0
.Lbzalgndw:
#ifndef	__mc68010__
	/* long word align */
	btst	#1,%d0			| if (dst & 2)
	jeq	.Lbzalgndl		|
	movw	%d2,(%a0)+		|	*(short *)dst++ = X
	subql	#2,%d1			|	len -= 2
.Lbzalgndl:
	/* set by 8 longwords */
	movel	%d1,%d0
	lsrl	#5,%d0			| cnt = len / 32
	jeq	.Lbzlong			| if (cnt)
	andl	#31,%d1			|	len %= 32
	subql	#1,%d0			|	set up for dbf
.Lbz32loop:
	movl	%d2,(%a0)+		|	set 8 long words
	movl	%d2,(%a0)+
	movl	%d2,(%a0)+
	movl	%d2,(%a0)+
	movl	%d2,(%a0)+
	movl	%d2,(%a0)+
	movl	%d2,(%a0)+
	movl	%d2,(%a0)+
#ifndef __mcoldfire__
	dbf	%d0,.Lbz32loop		|	till done
	clrw	%d0
#endif
	subql	#1,%d0
	jcc	.Lbz32loop
#endif	/* !__mc68010__ */

.Lbzlong:
	/* set by longwords */
	movel	%d1,%d0
	lsrl	#2,%d0			| cnt = len / 4
	jeq	.Lbzbyte			| if (cnt)
	subql	#1,%d0			|	set up for dbf
.Lbzlloop:
	movl	%d2,(%a0)+		|	clear longwords
#ifndef __mcoldfire__
	dbf	%d0,.Lbzlloop		|	till done
#endif
#ifdef	__mc68010__
	clrw	%d0
#endif	/* __mc68010__ */
#if defined(__m68010) || defined(__mcoldfire__)
	subql	#1,%d0
	jcc	.Lbzlloop
#endif	/* __mc68010__ || __mcoldfire__ */
	andl	#3,%d1			|	len %= 4
	jeq	.Lbzdone

	subql	#1,%d1			| set up for dbf
.Lbzbloop:
	movb	%d2,(%a0)+		| set bytes
.Lbzbyte:
#ifdef __mcoldfire__
	subql	#1,%d1			| decrement
	jcc	.Lbzbloop		| till done
#else
	dbf	%d1,.Lbzbloop		| till done
#endif
.Lbzdone:
	movl	8(%sp),%d0		| return destination
#ifdef __SVR4_ABI__
	moveal	%d0,%a0
#endif
	movl	(%sp)+,%d2
	rts
END(memset)
