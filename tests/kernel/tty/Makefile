# $NetBSD$

.include <bsd.own.mk>

TESTSDIR=	${TESTSBASE}/kernel/tty

TESTS_C=	t_pr

CPPFLAGS+=	-D_KERNTYPES
LDADD+=		-lrumpkern_tty -lrumpvfs -lrump -lrumpuser
LDADD+=		-lrump
LDADD+=		-lpthread

.include <bsd.test.mk>
