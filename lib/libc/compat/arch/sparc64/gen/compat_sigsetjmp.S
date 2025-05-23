/*	$NetBSD$	*/
/*
 * Copyright (c) 1995 Paul Kranenburg
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Paul Kranenburg.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
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

#include "SYS.h"

#if __PIC__ - 0 >= 2

ENTRY(sigsetjmp)
	PIC_PROLOGUE(%g1,%o2)		! %g1 = _GLOBAL_OFFSET_TABLE
	set	_C_LABEL(setjmp), %o2	! if (%o1 != 0) goto _setjmp;
	set	_C_LABEL(_setjmp), %o3	! else goto __setjmp;
	movrnz	%o1, %o2, %o3
	ldx [%g1+%o3], %g1
	jmp %g1
	 st	%o1,[%o0+0x48]		! jmpbuf[JBLEN]


ENTRY(siglongjmp)
	PIC_PROLOGUE(%g1,%o2)		! %g1 = _GLOBAL_OFFSET_TABLE
	ld	[%o0 + 0x48],%o2	! jmpbuf[JBLEN]
	set	_C_LABEL(longjmp), %o3	! if (%o2 != 0) goto _longjmp;
	set	_C_LABEL(_longjmp), %o4	! else goto __longjmp;
	movrnz	%o2, %o3, %o4 
	ldx	[%g1 + %o4], %g1
	jmp %g1
	 nop
	unimp 0

#elif __PIC__ - 0 >= 1

ENTRY(sigsetjmp)
	PIC_PROLOGUE(%g1,%o2)		! %g1 = _GLOBAL_OFFSET_TABLE
	cmp	%o1, 0
	bne,a	1f
	 ldx	[%g1 + _C_LABEL(setjmp)], %g1	! if (%o1 != 0) goto _setjmp;
	ldx	[%g1 + _C_LABEL(_setjmp)], %g1	! else goto __setjmp;
1:	
	jmp %g1
	 st	%o1,[%o0+0x48]		! jmpbuf[JBLEN]


ENTRY(siglongjmp)
	PIC_PROLOGUE(%g1,%o2)		! %g1 = _GLOBAL_OFFSET_TABLE
	cmp	%o2, 0
	ld	[%o0 + 0x48],%o2	! jmpbuf[JBLEN]
	bne,a	1f
	 ldx	[%g1 + _C_LABEL(longjmp)], %g1	! if (%o2 != 0) goto _longjmp;
	ldx	[%g1 + _C_LABEL(_longjmp)], %g1	! else goto __longjmp;
1:	
	jmp %g1
	 nop
	unimp 0

#else /* !__PIC__ */

ENTRY(sigsetjmp)
	brz,pt	%o1,_C_LABEL(_setjmp)
	 st	%o1,[%o0+0x48]		! jmpbuf[JBLEN]
	ba,a,pt	%icc,_C_LABEL(setjmp)
	 nop				! spitfire bug
	unimp 0

ENTRY(siglongjmp)
	ld	[%o0 + 0x48], %g1
	brz,pt	%g1, _C_LABEL(_longjmp)
	 nop
	ba,a,pt	%icc, _C_LABEL(longjmp)
	unimp 0

#endif /* !__PIC__ */
