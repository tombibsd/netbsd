|	Writes Human68k DOS error number to  dos_errno.
|	Called on errors of DOS calls.
|
|	written by ITOH Yasufumi
|	public domain
|
|	$NetBSD$

#include "dos_asm.h"
#include "dos_errno.h"

ASENTRY_NOPROFILE(CERROR)
	movel	%d0,%sp@-
	negl	%d0
	cmpil	#_DOS_EEXIST,%d0
	bnes	Lno_eexisists
	moveq	#DOS_EEXIST,%d0
Lno_eexisists:
	movel	%d0,_C_LABEL(dos_errno)
	movel	%sp@+,%d0
	rts
