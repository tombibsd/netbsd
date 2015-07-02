#	$NetBSD$
#
.include <bsd.own.mk>

.if (${MKRUMP} != "no") && !defined(BSD_MK_COMPAT_FILE)
TESTS_SUBDIRS=		pthread
.endif

TESTSDIR=	${TESTSBASE}/lib/semaphore

.include <bsd.test.mk>
