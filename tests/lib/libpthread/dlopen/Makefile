# $NetBSD$

NOMAN=		# defined

.include <bsd.own.mk>

TESTSDIR=	${TESTSBASE}/lib/libpthread/dlopen
CPPFLAGS+=	-DTESTDIR=\"${TESTSDIR:Q}/\"
RPATH=-Wl,-rpath=${TESTDIR}

LDADD+=${RPATH}

.if (${MKPIC:Uno} != "no")
SUBDIR+=	dso
TESTS_C+=	t_dlopen
TESTS_C+=	t_main_pthread_create
LDADD.t_main_pthread_create+=-lpthread 
PDADD.t_main_pthread_create+=${LIBPTHREAD}
TESTS_C+=	t_dso_pthread_create
.endif

.include <bsd.test.mk>
