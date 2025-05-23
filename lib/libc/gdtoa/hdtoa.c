/*	$NetBSD$	*/

/*-
 * Copyright (c) 2004, 2005 David Schultz <das@FreeBSD.ORG>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#if 0
__FBSDID("$FreeBSD: src/lib/libc/gdtoa/_hdtoa.c,v 1.4 2007/01/03 04:57:58 das Exp $");
#else
__RCSID("$NetBSD$");
#endif

#include <float.h>
#include <limits.h>
#include <math.h>
#ifndef __vax__
#include <machine/ieee.h>
#else
#include <machine/vaxfp.h>
#define ieee_double_u vax_dfloating_u
#define dblu_d dfltu_d
#define dblu_dbl dfltu_dflt
#define dbl_sign dflt_sign
#define dbl_exp dflt_exp
#define dbl_frach dflt_frach
#define dbl_fracm dflt_fracm
#define dbl_fracl dflt_fracl
#define DBL_FRACHBITS	DFLT_FRACHBITS
#define DBL_FRACMBITS	DFLT_FRACMBITS
#define DBL_FRACLBITS	DFLT_FRACLBITS
#define DBL_EXPBITS	DFLT_EXPBITS
#endif
#include "gdtoaimp.h"

/* Strings values used by dtoa() */
#define	INFSTR	"Infinity"
#define	NANSTR	"NaN"

#define	DBL_ADJ		(DBL_MAX_EXP - 2 + ((DBL_MANT_DIG - 1) % 4))
#define	LDBL_ADJ	(LDBL_MAX_EXP - 2 + ((LDBL_MANT_DIG - 1) % 4))

/*
 * Round up the given digit string.  If the digit string is fff...f,
 * this procedure sets it to 100...0 and returns 1 to indicate that
 * the exponent needs to be bumped.  Otherwise, 0 is returned.
 */
static int
roundup(char *s0, int ndigits)
{
	char *s;

	for (s = s0 + ndigits - 1; *s == 0xf; s--) {
		if (s == s0) {
			*s = 1;
			return (1);
		}
		*s = 0;
	}
	++*s;
	return (0);
}

/*
 * Round the given digit string to ndigits digits according to the
 * current rounding mode.  Note that this could produce a string whose
 * value is not representable in the corresponding floating-point
 * type.  The exponent pointed to by decpt is adjusted if necessary.
 */
static void
dorounding(char *s0, int ndigits, int sign, int *decpt)
{
	int adjust = 0;	/* do we need to adjust the exponent? */

	switch (FLT_ROUNDS) {
	case 0:		/* toward zero */
	default:	/* implementation-defined */
		break;
	case 1:		/* to nearest, halfway rounds to even */
		if ((s0[ndigits] > 8) ||
		    (s0[ndigits] == 8 && s0[ndigits - 1] & 1))
			adjust = roundup(s0, ndigits);
		break;
	case 2:		/* toward +inf */
		if (sign == 0)
			adjust = roundup(s0, ndigits);
		break;
	case 3:		/* toward -inf */
		if (sign != 0)
			adjust = roundup(s0, ndigits);
		break;
	}

	if (adjust)
		*decpt += 4;
}

/*
 * This procedure converts a double-precision number in IEEE format
 * into a string of hexadecimal digits and an exponent of 2.  Its
 * behavior is bug-for-bug compatible with dtoa() in mode 2, with the
 * following exceptions:
 *
 * - An ndigits < 0 causes it to use as many digits as necessary to
 *   represent the number exactly.
 * - The additional xdigs argument should point to either the string
 *   "0123456789ABCDEF" or the string "0123456789abcdef", depending on
 *   which case is desired.
 * - This routine does not repeat dtoa's mistake of setting decpt
 *   to 9999 in the case of an infinity or NaN.  INT_MAX is used
 *   for this purpose instead.
 *
 * Note that the C99 standard does not specify what the leading digit
 * should be for non-zero numbers.  For instance, 0x1.3p3 is the same
 * as 0x2.6p2 is the same as 0x4.cp3.  This implementation chooses the
 * first digit so that subsequent digits are aligned on nibble
 * boundaries (before rounding).
 *
 * Inputs:	d, xdigs, ndigits
 * Outputs:	decpt, sign, rve
 */
char *
hdtoa(double d, const char *xdigs, int ndigits, int *decpt, int *sign,
    char **rve)
{
	static const int sigfigs = (DBL_MANT_DIG + 3) / 4;
	union ieee_double_u u;
	char *s, *s0;
	size_t bufsize;

	u.dblu_d = d;
	*sign = u.dblu_dbl.dbl_sign;

	switch (fpclassify(d)) {
	case FP_NORMAL:
		*decpt = u.dblu_dbl.dbl_exp - DBL_ADJ;
		break;
	case FP_ZERO:
		*decpt = 1;
		return (nrv_alloc("0", rve, 1));
	case FP_SUBNORMAL:
#ifdef __vax__
		/* (DBL_MAX_EXP=127 / 2) + 2 = 65? */
		u.dblu_d *= 0x1p65;
		*decpt = u.dblu_dbl.dbl_exp - (65 + DBL_ADJ);
#else
		/* (DBL_MAX_EXP=1024 / 2) + 2 = 514? */
		u.dblu_d *= 0x1p514;
		*decpt = u.dblu_dbl.dbl_exp - (514 + DBL_ADJ);
#endif
		break;
	case FP_INFINITE:
		*decpt = INT_MAX;
		return (nrv_alloc(INFSTR, rve, sizeof(INFSTR) - 1));
	case FP_NAN:
		*decpt = INT_MAX;
		return (nrv_alloc(NANSTR, rve, sizeof(NANSTR) - 1));
	default:
		abort();
	}

	/* FP_NORMAL or FP_SUBNORMAL */

	if (ndigits == 0)		/* dtoa() compatibility */
		ndigits = 1;

	/*
	 * For simplicity, we generate all the digits even if the
	 * caller has requested fewer.
	 */
	bufsize = (sigfigs > ndigits) ? sigfigs : ndigits;
	s0 = rv_alloc(bufsize);
	if (s0 == NULL)
		return NULL;

	/*
	 * We work from right to left, first adding any requested zero
	 * padding, then the least significant portion of the
	 * mantissa, followed by the most significant.  The buffer is
	 * filled with the byte values 0x0 through 0xf, which are
	 * converted to xdigs[0x0] through xdigs[0xf] after the
	 * rounding phase.
	 */
	for (s = s0 + bufsize - 1; s > s0 + sigfigs - 1; s--)
		*s = 0;
	for (; s > s0 + sigfigs - (DBL_FRACLBITS / 4) - 1 && s > s0; s--) {
		*s = u.dblu_dbl.dbl_fracl & 0xf;
		u.dblu_dbl.dbl_fracl >>= 4;
	}
#ifdef DBL_FRACMBITS
	for (; s > s0; s--) {
		*s = u.dblu_dbl.dbl_fracm & 0xf;
		u.dblu_dbl.dbl_fracm >>= 4;
	}
#endif
	for (; s > s0; s--) {
		*s = u.dblu_dbl.dbl_frach & 0xf;
		u.dblu_dbl.dbl_frach >>= 4;
	}

	/*
	 * At this point, we have snarfed all the bits in the
	 * mantissa, with the possible exception of the highest-order
	 * (partial) nibble, which is dealt with by the next
	 * statement.  We also tack on the implicit normalization bit.
	 */
	*s = u.dblu_dbl.dbl_frach | (1U << ((DBL_MANT_DIG - 1) % 4));

	/* If ndigits < 0, we are expected to auto-size the precision. */
	if (ndigits < 0) {
		for (ndigits = sigfigs; s0[ndigits - 1] == 0; ndigits--)
			continue;
	}

	if (sigfigs > ndigits && s0[ndigits] != 0)
		dorounding(s0, ndigits, u.dblu_dbl.dbl_sign, decpt);

	s = s0 + ndigits;
	if (rve != NULL)
		*rve = s;
	*s-- = '\0';
	for (; s >= s0; s--)
		*s = xdigs[(unsigned int)*s];

	return (s0);
}

#if (LDBL_MANT_DIG > DBL_MANT_DIG)

/*
 * This is the long double version of hdtoa().
 */
char *
hldtoa(long double e, const char *xdigs, int ndigits, int *decpt, int *sign,
    char **rve)
{
	static const int sigfigs = (LDBL_MANT_DIG + 3) / 4;
	union ieee_ext_u u;
	char *s, *s0;
	size_t bufsize;

	memset(&u, 0, sizeof u);
	u.extu_ld = e;
	*sign = u.extu_ext.ext_sign;

	switch (fpclassify(e)) {
	case FP_NORMAL:
		*decpt = u.extu_ext.ext_exp - LDBL_ADJ;
		break;
	case FP_ZERO:
		*decpt = 1;
		return (nrv_alloc("0", rve, 1));
	case FP_SUBNORMAL:
		u.extu_ld *= 0x1p514L;
		*decpt = u.extu_ext.ext_exp - (514 + LDBL_ADJ);
		break;
	case FP_INFINITE:
		*decpt = INT_MAX;
		return (nrv_alloc(INFSTR, rve, sizeof(INFSTR) - 1));
	case FP_NAN:
		*decpt = INT_MAX;
		return (nrv_alloc(NANSTR, rve, sizeof(NANSTR) - 1));
	default:
		abort();
	}

	/* FP_NORMAL or FP_SUBNORMAL */

	if (ndigits == 0)		/* dtoa() compatibility */
		ndigits = 1;

	/*
	 * For simplicity, we generate all the digits even if the
	 * caller has requested fewer.
	 */
	bufsize = (sigfigs > ndigits) ? sigfigs : ndigits;
	s0 = rv_alloc(bufsize);
	if (s0 == NULL)
		return NULL;

	/*
	 * We work from right to left, first adding any requested zero
	 * padding, then the least significant portion of the
	 * mantissa, followed by the most significant.  The buffer is
	 * filled with the byte values 0x0 through 0xf, which are
	 * converted to xdigs[0x0] through xdigs[0xf] after the
	 * rounding phase.
	 */
	for (s = s0 + bufsize - 1; s > s0 + sigfigs - 1; s--)
		*s = 0;
	for (; s > s0 + sigfigs - (EXT_FRACLBITS / 4) - 1 && s > s0; s--) {
		*s = u.extu_ext.ext_fracl & 0xf;
		u.extu_ext.ext_fracl >>= 4;
	}
#ifdef EXT_FRACHMBITS
	for (; s > s0; s--) {
		*s = u.extu_ext.ext_frachm & 0xf;
		u.extu_ext.ext_frachm >>= 4;
	}
#endif
#ifdef EXT_FRACLMBITS
	for (; s > s0; s--) {
		*s = u.extu_ext.ext_fraclm & 0xf;
		u.extu_ext.ext_fraclm >>= 4;
	}
#endif
	for (; s > s0; s--) {
		*s = u.extu_ext.ext_frach & 0xf;
		u.extu_ext.ext_frach >>= 4;
	}

	/*
	 * At this point, we have snarfed all the bits in the
	 * mantissa, with the possible exception of the highest-order
	 * (partial) nibble, which is dealt with by the next
	 * statement.  We also tack on the implicit normalization bit.
	 */
	*s = u.extu_ext.ext_frach | (1U << ((LDBL_MANT_DIG - 1) % 4));

	/* If ndigits < 0, we are expected to auto-size the precision. */
	if (ndigits < 0) {
		for (ndigits = sigfigs; s0[ndigits - 1] == 0; ndigits--)
			continue;
	}

	if (sigfigs > ndigits && s0[ndigits] != 0)
		dorounding(s0, ndigits, u.extu_ext.ext_sign, decpt);

	s = s0 + ndigits;
	if (rve != NULL)
		*rve = s;
	*s-- = '\0';
	for (; s >= s0; s--)
		*s = xdigs[(unsigned int)*s];

	return (s0);
}

#else	/* (LDBL_MANT_DIG == DBL_MANT_DIG) */

char *
hldtoa(long double e, const char *xdigs, int ndigits, int *decpt, int *sign,
    char **rve)
{

	return (hdtoa((double)e, xdigs, ndigits, decpt, sign, rve));
}

#endif	/* (LDBL_MANT_DIG == DBL_MANT_DIG) */
