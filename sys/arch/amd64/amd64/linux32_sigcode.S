/*	$NetBSD$ */

#include "assym.h"
#include <machine/asm.h>

	.code32
NENTRY(linux32_sigcode)
	call	*LINUX32_SF_HANDLER(%esp)
	leal	LINUX32_SF_SC(%esp),%ebx	# scp
	pushl	%eax
	movl	$LINUX32_SYS_sigreturn,%eax
	int	$0x80
	movl	$LINUX32_SYS_exit,%eax
	int	$0x80

	.balign	16
NENTRY(linux32_rt_sigcode)
	call	*LINUX32_RT_SF_HANDLER(%esp)
	leal	LINUX32_RT_SF_UC(%esp),%ebx	# scp
	pushl	%eax
	movl	$LINUX32_SYS_rt_sigreturn,%eax
	int	$0x80
	movl	$LINUX32_SYS_exit,%eax
	int	$0x80
	.balign	16
	.globl	_C_LABEL(linux32_esigcode)
_C_LABEL(linux32_esigcode):

