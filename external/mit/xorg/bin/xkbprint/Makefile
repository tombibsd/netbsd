#	$NetBSD$

.include <bsd.own.mk>

PROG=	xkbprint
SRCS=	xkbprint.c utils.c psgeom.c

CPPFLAGS+=	-DHAVE_ASPRINTF

LDADD+=	-lxkbfile -lXext -lX11
DPADD+=	${LIBXKBFILE} ${LIBXEXT} ${LIBX11}

.PATH:	${X11SRCDIR.${PROG}} ${X11SRCDIR.${PROG}}/man

# XXX
CPPFLAGS.psgeom.c+=	-Wno-error

.include "../../xorg-pkg-ver.mk"

.include <bsd.x11.mk>
.include <bsd.prog.mk>
