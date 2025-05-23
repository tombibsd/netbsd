/*	$NetBSD$	*/
/*-
 * Copyright (c) 2011 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Matt Thomas of 3am Software Foundry.
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

#include <sys/cdefs.h>

#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD$");
#endif /* LIBC_SCCS and not lint */

#include <stdbool.h>
#include <stdint.h>
#include <float.h>
#include <machine/ieee.h>

#ifdef SOFTFLOAT
#include "softfloat/softfloat-for-gcc.h"
#endif

#if defined(__x86_64__) || defined(__i486__)
#define	FIXUNS	__fixunsxfsi
#else
#define	FIXUNS	__fixunstfsi
#endif

uint32_t __fixunsgen32(int, bool, size_t, size_t, const uint32_t *);

uint32_t FIXUNS(long double);

/*
 * Convert long double to (unsigned) int.  All operations are done module 2^32.
 */
uint32_t
FIXUNS(long double x)
{
	const union ieee_ext_u extu = { .extu_ld = x };
	uint32_t frac[(EXT_FRACBITS + 31)/32 + 1];

	frac[0] = 0;

	EXT_TO_ARRAY32(extu, &frac[1]);

	return __fixunsgen32(
		extu.extu_ext.ext_exp - EXT_EXP_BIAS,
		extu.extu_ext.ext_sign != 0,
		LDBL_MANT_DIG,
		EXT_FRACHBITS,
		&frac[__arraycount(frac)-1]);
}
