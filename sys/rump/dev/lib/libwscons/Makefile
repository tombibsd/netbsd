#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../dev/wscons

LIB=	rumpdev_wscons
COMMENT=Workstation console support

IOCONF=	WSCONS.ioconf

SRCS=   wsevent.c wskbd.c wskbdutil.c wsmouse.c

SRCS+=	wscons_component.c

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
