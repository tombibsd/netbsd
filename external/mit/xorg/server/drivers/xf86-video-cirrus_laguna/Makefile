#	$NetBSD$

# XXX move this under ../xf86-video-cirrus

DRIVER=		xf86-video-cirrus
DRIVER_NAME=	cirrus_laguna

SRCS=		lg_driver.c lg_hwcurs.c lg_i2c.c lg_xaa.c

CPPFLAGS.lg_driver.c=	-Wno-error # XXX

.include "../Makefile.xf86-driver"
