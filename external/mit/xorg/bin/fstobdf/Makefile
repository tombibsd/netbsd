#	$NetBSD$

.include <bsd.own.mk>

PROG=	fstobdf
SRCS=	chars.c fstobdf.c header.c props.c

LDADD+=	-lFS -lXext -lX11
DPADD+=	${LIBFS} ${LIBXEXT} ${LIBX11}

.PATH:	${X11SRCDIR.${PROG}}
.PATH:	${X11SRCDIR.${PROG}}/man

.include "../../xorg-pkg-ver.mk"

.include <bsd.x11.mk>
.include <bsd.prog.mk>
