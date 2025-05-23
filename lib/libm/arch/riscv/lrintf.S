/* $NetBSD$ */

/* The contents of this file are in the public domain. */

#include <machine/asm.h>

#ifdef _LP64_X
STRONG_ALIAS(llrintf,lrintf)
#endif

ENTRY(lrintf)
#ifdef _LP64
	fcvt.l.s	a0, fa0
#else
	fcvt.w.s	a0, fa0
#endif
	ret
END(lrintf)
