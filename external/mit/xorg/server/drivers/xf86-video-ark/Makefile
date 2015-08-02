#	$NetBSD$

DRIVER=		xf86-video-ark
DRIVER_NAME=	ark_drv

SRCS=		ark_accel.c ark_driver.c

CPPFLAGS+=	-DPACKAGE_VERSION=\"${PACKAGE_VERSION}\"
CPPFLAGS+=	-DHAVE_XAA_H

.include "../Makefile.xf86-driver"
