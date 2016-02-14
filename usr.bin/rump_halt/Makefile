#	$NetBSD$
#

PROG=	rump.halt

LDADD+= -lrumpclient
DPADD+=	${RUMPCLIENT}
CPPFLAGS+=	-D_KERNTYPES

.include <bsd.prog.mk>
