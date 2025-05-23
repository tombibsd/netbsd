/*	$NetBSD$	*/

#include "SYS.h"

ENTRY(__syscall)
	mv	t6, a0
	mv	a0, a1
	mv	a1, a2
	mv	a2, a3
	mv	a3, a4
	mv	a4, a5
	mv	a5, a6
	mv	a6, a7
	REG_L	a7, (sp)
        SYSTRAP(__syscall)
        JUMP_TO_CERROR()	/* error */
        ret			/* success */
END(__syscall)
STRONG_ALIAS(syscall, __syscall)
