|	Writes Human68k DOS process error number to  dos_errno.
|	Called on errors of DOS calls for processes.
|
|	written by ITOH Yasufumi
|	public domain
|
|	$NetBSD$

#include "dos_asm.h"
#include "dos_errno.h"

ASENTRY_NOPROFILE(PRCERROR)
	movel	%d0,%sp@-
	cmpil	#0xffff0100,%d0
	jcs	Lnoterrcode
	negl	%d0
Lwerr:
	movel	%d0,_C_LABEL(dos_errno)
	movel	%sp@+,%d0
	rts

Lnoterrcode:
	swap	%d0
	addqw	#1,%d0
	jeq	Lillid

	moveq	#DOS_EBUFOVER,%d0
	jra	Lwerr

Lillid:
	moveq	#DOS_ESRCH,%d0
	jra	Lwerr
