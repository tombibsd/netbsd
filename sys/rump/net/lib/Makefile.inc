#	$NetBSD$
#

RUMPTOP=	${.CURDIR}/../../..
CPPFLAGS+=	-I${RUMPTOP}/librump/rumpkern -I${RUMPTOP}/librump/rumpnet

.include "${RUMPTOP}/Makefile.rump"
