#	from: @(#)Makefile	8.1 (Berkeley) 6/6/93
#	$NetBSD$

.include <bsd.own.mk>

USE_FORT?= yes	# network server

PROG=	inetd
SRCS=	inetd.c
MAN=	inetd.8
MLINKS=	inetd.8 inetd.conf.5

CPPFLAGS+=-DLIBWRAP
# Use LIBWRAP_INTERNAL for libwrap checking of inetd's `internal' services.
#CPPFLAGS+=-DLIBWRAP_INTERNAL
LDADD+= -lwrap -lutil
DPADD+= ${LIBWRAP} ${LIBUTIL}

.if (${USE_INET6} != "no")
CPPFLAGS+=-DINET6
.endif

CPPFLAGS+=-DIPSEC
SRCS+=	ipsec.c
LDADD+= -lipsec
DPADD+=	${LIBIPSEC}

.include <bsd.prog.mk>
