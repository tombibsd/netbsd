#	$NetBSD$
#	from: @(#)Makefile	5.8 (Berkeley) 7/28/90

USE_FORT?= yes	# network client

PROG=	ypbind
MAN=	ypbind.8
CPPFLAGS+=-DHEURISTIC -DDEBUG

LDADD+=	-lutil
DPADD+=	${LIBUTIL}

.include <bsd.prog.mk>
