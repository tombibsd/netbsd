#	$NetBSD$

USE_FORT?=	yes	# setuid

PROG=		at
SRCS=		at.c panic.c parsetime.c perm.c privs.c stime.c
LINKS=		${BINDIR}/at ${BINDIR}/atq \
		${BINDIR}/at ${BINDIR}/atrm \
		${BINDIR}/at ${BINDIR}/batch
MLINKS=		at.1 batch.1 \
		at.1 atq.1 \
		at.1 atrm.1

BINOWN=		root
BINMODE=	4555

DPADD+=		${LIBUTIL}
LDADD+=		-lutil

.include <bsd.prog.mk>
