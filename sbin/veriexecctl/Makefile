#	$NetBSD$

PROG=	veriexecctl
MAN=	veriexecctl.8
SRCS=	veriexecctl_parse.y veriexecctl_conf.l veriexecctl.c

YHEADER=	1
CPPFLAGS+=	-I${.CURDIR} -I.
DPADD+=		${LIBL} ${LIBPROP}
LDADD+=		-ll -lprop

.include <bsd.prog.mk>
