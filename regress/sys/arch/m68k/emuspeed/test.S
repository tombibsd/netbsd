/*	$NetBSD$	*/

#include <m68k/asm.h>

ENTRY_NOPROFILE(mul32smem)
	movl	%d2,%sp@-
	movl	%sp@(8),%d2
L1:
	mulsl	%sp@(8),%d1
	subql	#1,%d2
	jne	L1
	movl	%sp@+,%d2
	rts

ENTRY_NOPROFILE(mul32sreg)
	movl	%d2,%sp@-
	movl	%sp@(8),%d2
L2:
	mulsl	%d0,%d1
	subql	#1,%d2
	jne	L2
	movl	%sp@+,%d2
	rts

ENTRY_NOPROFILE(illegal)
	illegal
	rts
