#	$NetBSD$
#

LIB=	rumpnet_virtif
COMMENT=Network interface which uses hypercalls for I/O

VIRTIF=	virt

.include "${.CURDIR}/Makefile.virtif"

RUMPCOMP_USER_SRCS=	virtif_user.c

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
