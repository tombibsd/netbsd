/*	$NetBSD$	*/

/*
 * Copyright (C) 2012  Internet Systems Consortium, Inc. ("ISC")
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* Id */

#ifndef DNS_DSDIGEST_H
#define DNS_DSDIGEST_H 1

/*! \file dns/dsdigest.h */

#include <isc/lang.h>

#include <dns/types.h>

ISC_LANG_BEGINDECLS

isc_result_t
dns_dsdigest_fromtext(dns_dsdigest_t *dsdigestp, isc_textregion_t *source);
/*%<
 * Convert the text 'source' refers to into a DS/DLV digest type value.
 * The text may contain either a mnemonic digest name or a decimal
 * digest number.
 *
 * Requires:
 *\li	'dsdigestp' is a valid pointer.
 *
 *\li	'source' is a valid text region.
 *
 * Returns:
 *\li	ISC_R_SUCCESS			on success
 *\li	ISC_R_RANGE			numeric type is out of range
 *\li	DNS_R_UNKNOWN			mnemonic type is unknown
 */

isc_result_t
dns_dsdigest_totext(dns_dsdigest_t dsdigest, isc_buffer_t *target);
/*%<
 * Put a textual representation of the DS/DLV digest type 'dsdigest'
 * into 'target'.
 *
 * Requires:
 *\li	'dsdigest' is a valid dsdigest.
 *
 *\li	'target' is a valid text buffer.
 *
 * Ensures,
 *	if the result is success:
 *\li		The used space in 'target' is updated.
 *
 * Returns:
 *\li	ISC_R_SUCCESS			on success
 *\li	ISC_R_NOSPACE			target buffer is too small
 */

#define DNS_DSDIGEST_FORMATSIZE 20
void
dns_dsdigest_format(dns_dsdigest_t typ, char *cp, unsigned int size);
/*%<
 * Wrapper for dns_dsdigest_totext(), writing text into 'cp'
 */

ISC_LANG_ENDDECLS

#endif /* DNS_DSDIGEST_H */
