#	$NetBSD$

DRIVER=		xf86-video-intel-old
DRIVER_NAME=	sil164_drv


SRCS=		sil164_module.c sil164.c

CPPFLAGS+=      -I${DESTDIR}${X11INCDIR}/X11
CPPFLAGS+=      -I${DESTDIR}${X11INCDIR}/X11/dri

.include "../../Makefile.xf86-driver"
.PATH: ${X11SRCDIR.${DRIVER}}/src/sil164
