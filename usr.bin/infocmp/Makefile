#	$NetBSD$

PROG=		infocmp

CPPFLAGS+=	-I${.CURDIR}/../../lib/libterminfo
LDADD+=		-lterminfo -lutil
DPADD+=		${LIBTERMINFO} ${LIBUTIL}

.include <bsd.prog.mk>
