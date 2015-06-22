#	$NetBSD$
#	@(#)Makefile	8.1 (Berkeley) 6/6/93

.include <bsd.own.mk>

PROG=	xinstall
SRCS=	xinstall.c getid.c metachar.c
MAN=	install.1

.PATH:		${NETBSDSRCDIR}/usr.bin/make
CPPFLAGS+=	-I${NETBSDSRCDIR}/usr.bin/make

.PATH:		${NETBSDSRCDIR}/usr.sbin/mtree
CPPFLAGS+=	-I${NETBSDSRCDIR}/usr.sbin/mtree
CPPFLAGS+=	-DHAVE_POSIX_SPAWN

.if (${HOSTPROG:U} == "")
DPADD+= ${LIBUTIL}
LDADD+= -lutil
.endif

COPTS.xinstall.c += -Wno-format-nonliteral


PROGNAME=install

.include <bsd.prog.mk>
