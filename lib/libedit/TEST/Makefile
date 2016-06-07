# $NetBSD$

NOMAN=1
PROG=wtc1
CPPFLAGS=-I${.CURDIR}/..
LDADD+=-ledit -ltermlib
DPADD+=${LIBEDIT} ${LIBTERMLIB}

.ifdef DEBUG
CPPFLAGS+=-DDEBUG
.endif

.include <bsd.prog.mk>
