# $NetBSD$

KYUA_LIBS=	utils

.include <bsd.init.mk>

TESTSDIR=	${TESTSBASE}/kyua-cli/utils/process

.PATH:		${SRCDIR}/utils/process

TESTS_CXX=	child_test \
		exceptions_test \
		fdstream_test \
		status_test \
		systembuf_test

PROGS_CXX=	helpers
BINDIR.helpers=	${TESTSDIR}
MAN.helpers=	# none

.include <bsd.test.mk>
