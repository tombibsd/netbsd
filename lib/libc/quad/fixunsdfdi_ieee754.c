/*	$NetBSD$	*/

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
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

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD$");
#endif /* LIBC_SCCS and not lint */

#if defined(SOFTFLOAT) || defined(__ARM_EABI__)
#include "softfloat/softfloat-for-gcc.h"
#endif

#include "quad.h"
#include <limits.h>
#include <machine/ieee.h>

/*
 * Convert double to unsigned quad.
 * Not sure what to do with negative numbers---for now, anything out
 * of range becomes UQUAD_MAX.
 */
u_quad_t
__fixunsdfdi(double x)
{
	const union ieee_double_u ux = { .dblu_d = x };
	signed int exp = ux.dblu_exp - DBL_EXP_BIAS;
	u_quad_t r;

	if (ux.dblu_sign)
		return UQUAD_MAX;
	if (exp > 63)
		return UQUAD_MAX;
	if (exp < 0)
		return 0;

	r = 1 << DBL_FRACHBITS;		/* implicit bit */
	r |= ux.dblu_frach;
	exp -= DBL_FRACHBITS;
	if (exp == 0)
		return r;
	if (exp < 0)
		return r >> -exp;

	r <<= DBL_FRACLBITS;
	r |= ux.dblu_fracl;
	exp -= DBL_FRACLBITS;
	if (exp == 0)
		return r;
	if (exp < 0)
		return r >> -exp;

	return r << exp;
}
