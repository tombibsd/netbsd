#	$NetBSD$
#	from: @(#)Makefile	8.1 (Berkeley) 6/6/93

.include <bsd.own.mk>

USE_FORT?= yes	# setuid
PROG=	fstat
SRCS=	fstat.c isofs.c ntfs.c ptyfs.c tmpfs.c misc.c
DPADD=	${LIBKVM}
LDADD=	-lkvm
BINGRP=	kmem
BINMODE=2555

CPPFLAGS+= -I${NETBSDSRCDIR}/sys

.if (${USE_INET6} != "no")
CPPFLAGS+=-DINET6
.endif
CPPFLAGS+=-D_KMEMUSER

LDADD+=-lutil
DPADD+=${LIBUTIL}

.include <bsd.prog.mk>
