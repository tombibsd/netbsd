/* $NetBSD$ */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD$");
#endif /* LIBC_SCCS and not lint */

#include <math.h>

/* bytes for quiet NaN (IEEE single precision) */
const union __float_u __nanf =
#ifdef __AARCH64EB__
		{ { 0x7f, 0xc0,    0,    0 } };
#else
		{ {    0,    0, 0xc0, 0x7f } };
#endif

__warn_references(__nanf, "warning: <math.h> defines NAN incorrectly for your compiler.")
