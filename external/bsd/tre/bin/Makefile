#	$NetBSD$

TREDIST=	${.CURDIR}/../dist

.PATH: ${TREDIST}/src

CPPFLAGS+=	-I${TREDIST}/lib
CPPFLAGS+=	-DHAVE_CONFIG_H=1

BINDIR=		/usr/bin
PROG=		agrep
SRCS=		agrep.c
LDADD+=		-ltre
DPADD+=		${LIBTRE}

WARNS=	4

.include <bsd.prog.mk>
