#	$NetBSD$

.include <bsd.own.mk>

PROG=	xlsfonts
SRCS=	xlsfonts.c dsimple.c

LDADD+=	-lXext -lX11
DPADD+=	${LIBXEXT} ${LIBX11}

.PATH:	${X11SRCDIR.${PROG}}
.PATH:	${X11SRCDIR.${PROG}}/man

COPTS.xlsfonts.c+=      -Wno-error

.include "../../xorg-pkg-ver.mk"

.include <bsd.x11.mk>
.include <bsd.prog.mk>
