#	$NetBSD$

CPPFLAGS+= -I${.CURDIR}
PROG=	fgen
SRCS=	fgen.l
MAN=	fgen.1

.if !defined(HOSTPROG)
LDADD+=	-lutil
DPADD+=	${LIBUTIL}
.endif

.include <bsd.prog.mk>
