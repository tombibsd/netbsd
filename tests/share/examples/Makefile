# $NetBSD$

.include <bsd.own.mk>

TESTSDIR=	${TESTSBASE}/share/examples

TESTS_SH=	t_asm

.if ${MKKYUA} != "no"
# These examples come from external/bsd/lutok/tests/share/examples/lutok.
# Ensure they get registered in the Atffile only when installed.
ATFFILE_EXTRA_TPS=	lutok
.endif

.include <bsd.test.mk>
