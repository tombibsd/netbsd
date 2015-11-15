#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../kern

LIB=	rumpdev_drvctl
COMMENT=/dev/drvctl driver

IOCONF=	DRVCTL.ioconf

SRCS=	kern_drvctl.c
SRCS+=	drvctl_component.c

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
