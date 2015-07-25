#	$NetBSD$

.include <bsd.own.mk>

PROG=	bdftopcf

LDADD+=	-lXfont -lfontenc -lfreetype -lm -lz -lbz2
DPADD+=	${LIBXFONT} ${LIBFONTENC} ${LIBFREETYPE} ${LIBM} ${LIBZ} ${LIBBZ2}

.PATH:	${X11SRCDIR.${PROG}}
.PATH:	${X11SRCDIR.${PROG}}/man

.include "../../xorg-pkg-ver.mk"

.include <bsd.x11.mk>
.include <bsd.prog.mk>
