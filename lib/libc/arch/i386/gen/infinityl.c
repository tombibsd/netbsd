/*	$NetBSD$	*/

/*
 * IEEE-compatible infinityl.c for little-endian 80-bit format -- public domain.
 * Note that the representation includes 16 bits of tail padding per i386 ABI.
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD$");
#endif /* LIBC_SCCS and not lint */

#include <math.h>

const union __long_double_u __infinityl =
	{ { 0, 0, 0, 0, 0, 0, 0, 0x80, 0xff, 0x7f, 0, 0 } };
