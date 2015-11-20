#	$NetBSD$

DRIVER=		xf86-video-modesetting
DRIVER_NAME=	modesetting_drv

SRCS=		driver.c drmmode_display.c
MAN=		modesetting.4

CPPFLAGS+=	-I${DESTDIR}${X11INCDIR}/libdrm
CPPFLAGS+=	-DRANDR_12_INTERFACE

.include "../Makefile.xf86-driver"
