#	$NetBSD$

USE_FORT?=	yes	# setgid
PROG=		pmap
SRCS=		main.c pmap.c
BINGRP=		kmem
BINMODE=	2555
LDADD=		-lutil -lkvm
DPADD=		${LIBUTIL} ${LIBKVM}

CWARNFLAGS.clang+=	-Wno-format-extra-args
CPPFLAGS+=	-D_KMEMUSER

.include <bsd.prog.mk>
