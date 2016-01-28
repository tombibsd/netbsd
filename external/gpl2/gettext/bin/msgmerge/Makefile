#	$NetBSD$

PROG=	msgmerge
SRCS=msgmerge.c msgl-fsearch.c

LDADD+=	-lm
DPADD+= ${LIBM}

COPTS.msgmerge.c = -Wno-stack-protector

.include "${.CURDIR}/../Makefile.inc.prog"
