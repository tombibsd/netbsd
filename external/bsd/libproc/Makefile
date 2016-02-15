# $NetBSD$

.include <bsd.own.mk>

TEST_SUBDIRS=
.if ${MKATF} != "no"
TEST_SUBDIRS+=	tests
.endif

SUBDIR= lib .WAIT ${TEST_SUBDIRS}

.include <bsd.subdir.mk>
