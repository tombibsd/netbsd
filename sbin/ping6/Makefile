# $NetBSD$

.include <bsd.own.mk>

USE_FORT?=	yes	# setuid
RUMPPRG=	ping6
MAN=		ping6.8

BINOWN=		root
BINMODE=	4555

CPPFLAGS+=	-DINET6
CPPFLAGS+=	-DIPSEC

LDADD+=		-lipsec -lm
DPADD+=		${LIBIPSEC} ${LIBM}

.PATH:		${.CURDIR}/../../lib/libc/net
RUMPSRCS=	getnameinfo.c
.if (${MKRUMP} != "no")
CPPFLAGS+= 	-DRUMP_ACTION
.endif

.include <bsd.prog.mk>
