/*	$NetBSD$ */

#include <machine/asm.h>

	.globl	return_one_end

ENTRY_NP(return_one)
	li	a0, 1
	ret
return_one_end:
END(return_one)
