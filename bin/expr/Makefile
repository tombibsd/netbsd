#	$NetBSD$

PROG=		expr
SRCS=		expr.y

.include <bsd.prog.mk>

# XXXGCC5
.if defined(HAVE_GCC) && ${HAVE_GCC} == 53 && ${ACTIVE_CC} == "gcc"
CFLAGS+= -fwrapv
.endif

