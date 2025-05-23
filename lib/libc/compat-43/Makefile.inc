#	$NetBSD$
#	from: @(#)Makefile.inc	8.1 (Berkeley) 6/2/93

# compat-43 sources
.PATH: ${ARCHDIR}/compat-43 ${.CURDIR}/compat-43

SRCS+=	creat.c getdtablesize.c gethostid.c \
	killpg.c sethostid.c setpgrp.c \
	setrgid.c setruid.c sigcompat.c
.if !defined(AUDIT)
SRCS+=	getwd.c
.endif

# XXX this is to get <compat/sys/signal.h>
CPPFLAGS.sigcompat.c+=	-I${NETBSDSRCDIR}/sys

MAN+=	creat.3 getdtablesize.3 gethostid.3 killpg.3 \
	setruid.3 sigblock.3 sigpause.3 sigsetmask.3 sigvec.3

MLINKS+=setruid.3 setrgid.3
MLINKS+=gethostid.3 sethostid.3
