/*	$NetBSD$	*/

/*-
 * Copyright (c) 2009 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by David Laight.
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

/* See comments in strlen.S about checking words for byte values */

#include <machine/asm.h>

#if defined(LIBC_SCCS)
	RCSID("$NetBSD$")
#endif

/*
 * On entry %rdi is the buffer and the low byte of %rsi (%sil) the
 * character to search for.
 *
 * Registers %rdx, %rcx, %r8-%r11 and %rax are also usable
 */

/* Uncomment below to get regression test to run this version but
 * have everything else use the trivial one below. */
/* #define TEST_STRCHR */

#ifdef TEST_STRCHR
ENTRY(test_strchr)
#else
ENTRY(strchr)
#endif
	movabsq	$0x0101010101010101,%r8

	movzbq	%sil,%rdx	/* value to search for (c) */
	/* These imul are 'directpath' on athlons, so are fast */
	imul	$0x80,%r8,%r9	/* 0x8080808080808080 */
	imul	%r8,%rdx	/* (c) copied to all bytes */
	test	$7,%dil
	jnz	20f		/* jump if misaligned */

	_ALIGN_TEXT		/* one byte nop */
1:
	movq	(%rdi),%rax	/* bytes to check (x) */
2:
	addq	$8,%rdi
	mov	%rax,%r10
	mov	%rax,%r11	/* for 'char' check */
	not	%r10		/* invert of data (~x) */

	xorq	%rdx,%r11	/* convert 'char' test to one for NUL */
	subq	%r8,%rax	/* x - 0x10 */
	movq	%r10,%rsi	/* ~x */
	subq	%r8,%r11	/* (x ^ c) - 0x10 */
/*
 * Here we could check ((x - 0x10) | ((x ^ c) - 0x10)) & 0x80
 * and short-circuit the case where no top bits are set, and
 * we continue the loop.
 * However it needs 3 more clocks that are difficult to interleave
 * in the existing dependency chain ...
 */
	andq	%r9,%rax	/* (x - 0x10) & 0x80 */
	xorq	%rdx,%rsi	/* c ^ ~x == ~(c ^ x) */
	andq	%r9,%r11	/* ((x ^ c) - 0x10) & 0x80 */
	andq	%r10,%rax	/* (x - 0x10) & 0x80 & ~x */
	jne	10f		/* jump if string ends */
	andq	%rsi,%r11	/* ((x ^ c) - 0x10) & 0x80 & ~(x ^ c) */
	je	1b		/* jump if no match */

	/* Found char, since LE can use bit scan */
	bsf	%r11,%r11	/* 7, 15, 23 ... 63 */
8:	shr	$3,%r11		/* 0, 1, 2 .. 7 */
	lea	-8(%r11,%rdi),%rax
	ret

/* End of string, check whether char is before NUL */
	_ALIGN_TEXT		/* adds three byte nop */
10:
	bsf	%rax,%rax	/* count to NUL */
	andq	%rsi,%r11	/* check for char in last 8 bytes */
	je	11f
	bsf	%r11,%r11	/* NUL and char - see which was first */
	cmp	%r11,%rax
	jae	8b		/* return 'found' if same - searching for NUL */
11:	xor	%eax,%eax	/* char not found */
	ret

/* Source misaligned: read aligned word and make low bytes invalid */
/* I (dsl) think a _ALIGN_TEXT here will slow things down! */
20:
	xor	%rcx,%rcx
	sub	%dil,%cl	/* Convert low address values 1..7 ... */
	sbb	%rsi,%rsi	/* carry was set, so %rsi now ~0u! */
	and	$7,%cl		/* ... to 7..1 */
	and	$~7,%dil	/* move address to start of word */
	shl	$3,%cl		/* now 56, 48 ... 16, 8 */
	movq	(%rdi),%rax	/* aligned word containing first data */
	xor	%rdx,%rsi	/* invert of search pattern (~c) */
	je	22f		/* searching for 0xff */
21:	shr	%cl,%rsi	/* ~c in low bytes */
	or	%rsi,%rax	/* set some bits making low bytes invalid */
	jmp	2b

/* We are searching for 0xff, so can't use ~pattern for invalid value */
22:
	mov	%r8,%r10	/* 0x01 pattern */
	lea	(%r8,%r8),%rsi	/* 0x02 - bits gets set (above) */
	not	%r10		/* now 0xfe */
	sar	%cl,%r10	/* top bytes 0xff */
	and	%r10,%rax	/* clear lsb from unwanted low bytes */
	jmp	21b
#ifdef TEST_STRCHR
END(test_strchr)
#else
END(strchr)
#endif

#ifdef TEST_STRCHR
/* Trivial version for bug-fixing above */
ENTRY(strchr)
	movq	%rsi,%rdx
	movq	%rdi,%rsi
1:
	lodsb
	cmp	%al,%dl
	je	2f
	test	%al,%al
	jne	1b
	xor	%eax,%eax
	ret
2:	lea	-1(%rsi),%rax
	ret
END(strchr)
#endif

STRONG_ALIAS(index,strchr)
