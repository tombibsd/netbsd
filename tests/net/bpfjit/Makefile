# $NetBSD$
#

.include <bsd.own.mk>

TESTSDIR=	${TESTSBASE}/net/bpfjit

TESTS_C=	t_bpfjit
TESTS_C+=	t_cop
TESTS_C+=	t_extmem
TESTS_C+=	t_mbuf

PAXCTL_FLAGS=	+m

LDADD+=		-lrumpnet_bpfjit -lrumpkern_sljit
LDADD+=		-lrumpdev_bpf -lrumpnet_net -lrumpnet
LDADD+=		-lrump -lrumpuser -lpthread

.include <bsd.test.mk>
