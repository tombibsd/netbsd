# $NetBSD$

.include <bsd.own.mk>

TESTSDIR=	${TESTSBASE}/lib/libc/db

TESTS_SH+=	t_db

TESTS_C+=	t_db_hash_seq

BINDIR=		${TESTSDIR}
MKMAN=		no

PROGS+=		h_db
PROGS+=		h_lfsr

FILESDIR=	${TESTSDIR}

FILES+=		README

.include <bsd.test.mk>
