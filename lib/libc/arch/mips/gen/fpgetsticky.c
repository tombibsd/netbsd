/*	$NetBSD$	*/

/*
 * Written by J.T. Conklin, Apr 11, 1995
 * Public domain.
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD$");
#endif /* LIBC_SCCS and not lint */

#include "namespace.h"

#include <ieeefp.h>

#ifdef __weak_alias
__weak_alias(fpgetsticky,_fpgetsticky)
#endif

fp_except
fpgetsticky(void)
{
	fp_except x;

	__asm(".set push; .set noat; cfc1 %0,$31; .set pop" : "=r" (x));
	return (x >> 2) & 0x1f;
}
