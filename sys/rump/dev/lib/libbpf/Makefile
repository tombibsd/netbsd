#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../net

LIB=	rumpdev_bpf
COMMENT=Berkeley Packet Filter

SRCS=	bpf.c bpf_filter.c
SRCS+=	bpf_component.c

CPPFLAGS+=	-I${.CURDIR}/../../../librump/rumpvfs

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
