/*	$NetBSD$	*/

/*
 * Copyright (c) 1996, 1997
 * 	Perry E. Metzger.  All rights reserved.
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
 *    must display the following acknowledgements:
 *	This product includes software developed for the NetBSD Project
 *	by Perry E. Metzger.
 * 4. The names of the authors may not be used to endorse or promote products
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
 *
 */

#include <machine/asm.h>

	.text

/*
 * BIOS call "INT 15H Function 86H" to sleep for a set number of microseconds
 *	Call with	%ah = 0x86
 *			%cx = time interval (high)
 *			%dx = time interval (low)
 *	Return:
 *		If error
 *			CF  = set
 *		else
 *			CF  = clear
 */
ENTRY(delay)
	pushl	%ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi

	movw	20(%esp), %dx
	movw	22(%esp), %cx

	call	_C_LABEL(prot_to_real)
	.code16

	movb	$0x86, %ah
	int	$0x15
	setnc	%ah

	movb	%ah, %bl	# real_to_prot uses %eax

	calll	_C_LABEL(real_to_prot)
	.code32

	xorl	%eax, %eax
	movb	%bl, %al

	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret
