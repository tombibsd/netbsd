#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../dev/pad

LIB=	rumpdev_pad
COMMENT=Pseudo Audio Device

SRCS=	pad.c
SRCS+=	pad_component.c

CPPFLAGS+=	-I${.CURDIR}/../../../librump/rumpvfs

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
