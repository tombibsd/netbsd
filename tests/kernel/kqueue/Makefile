# $NetBSD$

WARNS?=6
NOMAN=		# defined

.include <bsd.own.mk>

TESTSDIR=	${TESTSBASE}/kernel/kqueue

TESTS_SUBDIRS=	read
TESTS_SUBDIRS+=	write

TESTS_C=	t_ioctl
TESTS_C+=	t_proc1
TESTS_C+=	t_proc2
TESTS_C+=	t_proc3
TESTS_C+=	t_sig
TESTS_C+=	t_vnode

.include <bsd.test.mk>
