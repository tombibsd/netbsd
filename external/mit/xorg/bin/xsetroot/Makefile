#	$NetBSD$

.include <bsd.own.mk>

PROG=	xsetroot

LDADD+=	-lXmuu -lXt -lSM -lICE -lXext -lX11 -lXcursor
DPADD+=	${LIBXMUU} ${LIBXT} ${LIBSM} ${LIBICE} ${LIBXEXT} ${LIBX11} ${LIBXCURSOR}

.PATH:	${X11SRCDIR.${PROG}}
.PATH:	${X11SRCDIR.${PROG}}/man

.include "../../xorg-pkg-ver.mk"

.include <bsd.x11.mk>
.include <bsd.prog.mk>
