# $NetBSD$

USE_FORT?= yes	# network client

TRACEROUTE?= ${.CURDIR}/../traceroute

SRCS=	traceroute6.c as.c
PROG=	traceroute6
MAN=	traceroute6.8

BINMODE=4555
BINOWN=	root

CPPFLAGS+=-DINET6 -DIPSEC
CPPFLAGS+=-DUSE_RFC3542
LDADD+=	-lipsec
DPADD+=	${LIBIPSEC}

CPPFLAGS+=-DHAVE_POLL
CPPFLAGS+=-I${TRACEROUTE}

.PATH: ${TRACEROUTE}

.include <bsd.prog.mk>
