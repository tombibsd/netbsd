#	$NetBSD$
#	@(#)Makefile	8.1 (Berkeley) 6/2/93

PROG=		ps
SRCS=		fmt.c keyword.c nlist.c print.c ps.c
DPADD=		${LIBM} ${LIBKVM}
LDADD=		-lm -lkvm

CWARNFLAGS+=	-Wno-format-y2k
COPTS.print.c = -Wno-format-nonliteral

.include <bsd.prog.mk>
