# $NetBSD$

KYUA_LIBS=	utils

.include <bsd.init.mk>

TESTSDIR=	${TESTSBASE}/kyua-cli/utils/sqlite

.PATH:		${SRCDIR}/utils/sqlite

TESTS_CXX=	c_gate_test \
		database_test \
		exceptions_test \
		statement_test \
		transaction_test

.include <bsd.test.mk>
