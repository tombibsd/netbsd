/* $NetBSD$ */

/* The contents of this file are in the public domain. */

#include <machine/asm.h>

#ifdef _LP64_X
STRONG_ALIAS(llrint,lrint)
#endif

ENTRY(lrint)
#ifdef _LP64
	fcvt.l.d	a0, fa0
#else
	fcvt.w.d	a0, fa0
#endif
	ret
END(lrint)
