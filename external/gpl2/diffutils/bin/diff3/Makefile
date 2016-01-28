#	$NetBSD$

.include <bsd.init.mk>

DIST=		${IDIST}/src
.PATH:		${DIST} ${IDIST}/man

PROG=		diff3
SRCS=		diff3.c version.c

CWARNFLAGS.clang+=	-Wno-unused-value

.include <bsd.prog.mk>
