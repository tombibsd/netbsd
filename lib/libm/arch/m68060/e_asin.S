/* $NetBSD$ */

/*
 * FPLSP wrapper for __ieee754_asin
 * DO NOT EDIT - this file is automatically generated.
 */

#include <machine/asm.h>

ENTRY(__ieee754_asin)
#ifdef __SVR4_ABI__
	bral PIC_PLT(_C_LABEL(__fplsp060_0020))
#else
	movel %sp@(8),%sp@-
	movel %sp@(8),%sp@-
	bsrl PIC_PLT(_C_LABEL(__fplsp060_0020))
	fmoved %fp0,%sp@
	movel %sp@+,%d0
	movel %sp@+,%d1
	rts
#endif
