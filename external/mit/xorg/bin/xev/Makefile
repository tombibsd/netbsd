#	$NetBSD$

.include <bsd.own.mk>

PROG=	xev

LDADD+=	-lX11 -lXrandr
DPADD+=	${LIBX11} ${LIBXRANDR}

.PATH:	${X11SRCDIR.${PROG}}
.PATH:	${X11SRCDIR.${PROG}}/man

.include "../../xorg-pkg-ver.mk"

.include <bsd.x11.mk>
.include <bsd.prog.mk>
