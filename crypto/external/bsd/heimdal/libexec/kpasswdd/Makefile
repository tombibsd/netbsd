# $NetBSD$

.include <bsd.own.mk>
.include <${.CURDIR}/../../Makefile.inc>

.PATH:		${HEIMDIST}/kpasswd

PROG=	kpasswdd
SRCS=	kpasswdd.c
MAN=	kpasswdd.8

CPPFLAGS+= -DINETD_SUPPORT

LDADD+= -lkadm5srv -lhdb -lutil
DPADD+= ${LIBKADM5SRV} ${LIBHDB} ${LIBUTIL}

.include <${HEIMBASE}/Makefile.rules.inc>
.include <bsd.prog.mk>
