# $NetBSD$

.include <bsd.own.mk>

SRCDIR=		${NETBSDSRCDIR}/external/bsd/kyua-testers/dist
.PATH:		${SRCDIR}

BINDIR=		/usr/libexec
PROG_CXX=	kyua-atf-tester
SRCS=		atf_main.c
MAN=		kyua-atf-tester.1
MAN+=		kyua-atf-interface.7

PRIVATELIBDIR!=	cd ${.CURDIR}/../../lib; ${PRINTOBJDIR}
LDADD+=		${PRIVATELIBDIR}/libatf_tester/libatf_tester.a
DPADD+=		${PRIVATELIBDIR}/libatf_tester/libatf_tester.a
LDADD+=		${PRIVATELIBDIR}/libtester/libtester.a
DPADD+=		${PRIVATELIBDIR}/libtester/libtester.a

CPPFLAGS+=	-DHAVE_CONFIG_H
CPPFLAGS+=	-I${.CURDIR}/../../lib/libtester
CPPFLAGS+=	-I${SRCDIR}

WARNS?=		3

CLEANFILES+=	kyua-atf-tester.1
kyua-atf-tester.1:	kyua-atf-tester.1.in
	sed -e 's,__PACKAGE__,kyua-testers,g' \
	    -e 's,__TMPDIR__,/tmp,g' \
	    <${.ALLSRC} >${.TARGET}.tmp
	mv ${.TARGET}.tmp ${.TARGET}

.include <bsd.prog.mk>
