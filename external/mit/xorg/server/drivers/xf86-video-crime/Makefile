#	$NetBSD$

DRIVER=		xf86-video-crime
DRIVER_NAME=	crime_drv

SRCS=		crime_driver.c crime_cursor.c crime_accel.c
MAN=		crime.4

CFLAGS+=	-march=mips3
CPPFLAGS+=	-I${X11SRCDIR.xorg-server}/fb

.include "../Makefile.xf86-driver"
