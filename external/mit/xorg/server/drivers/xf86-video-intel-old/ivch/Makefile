#	$NetBSD$

DRIVER=		xf86-video-intel-old
DRIVER_NAME=	ivch_drv


SRCS=		ivch_module.c ivch.c

CPPFLAGS+=      -I${DESTDIR}${X11INCDIR}/X11
CPPFLAGS+=      -I${DESTDIR}${X11INCDIR}/X11/dri

.include "../../Makefile.xf86-driver"
.PATH: ${X11SRCDIR.${DRIVER}}/src/ivch
