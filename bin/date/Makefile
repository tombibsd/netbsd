#	$NetBSD$
#	@(#)Makefile	8.1 (Berkeley) 5/31/93

PROG=	date
SRCS=	date.c netdate.c
DPADD+=	${LIBUTIL}
LDADD+=	-lutil
CPPFLAGS+=-I${.CURDIR}

COPTS.date.c = -Wno-format-nonliteral

.include <bsd.prog.mk>
