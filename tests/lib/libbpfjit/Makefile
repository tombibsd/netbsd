# $NetBSD$

.include <bsd.own.mk>
.include <../../../external/bsd/sljit/Makefile.inc>

TESTSDIR=	${TESTSBASE}/lib/libbpfjit

TESTS_C+=	t_bpfjit
TESTS_C+=	t_extmem
TESTS_C+=	t_cop

PAXCTL_FLAGS=	+m

# XXX this variable doesn't belong to here
LIBBPFJITDIR!=	cd ${NETBSDSRCDIR}/lib/libbpfjit && ${PRINTOBJDIR}

LDADD+=		-L${LIBBPFJITDIR} -lbpfjit
DPADD+=		${LIBBPFJITDIR}/libbpfjit.a

LDADD+=		-L${LIBSLJITDIR} -lsljit
DPADD+=		${LIBSLJITDIR}/libsljit.a

LDADD+=		${LIBPCAP}

.include <bsd.test.mk>
