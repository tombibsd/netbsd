# $NetBSD$

WIDECHAR ?= yes

NOMAN=1
PROG=wtc1
CPPFLAGS=-I${.CURDIR}/..
LDADD+=-ledit -ltermlib
DPADD+=${LIBEDIT} ${LIBTERMLIB}

.if "${WIDECHAR}" == "yes"
CPPFLAGS+=-DWIDECHAR
.endif

.ifdef DEBUG
CPPFLAGS+=-DDEBUG
.endif

.include <bsd.prog.mk>
