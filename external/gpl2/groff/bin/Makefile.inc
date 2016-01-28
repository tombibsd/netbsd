# $NetBSD$

.include "${.PARSEDIR}/../Makefile.inc"

DPADD+=		${LIBM}
LDADD+=		-lm
