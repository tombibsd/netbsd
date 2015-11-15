#	$NetBSD$
#
# Public Domain.
#

.PATH:	${.CURDIR}/../../../../net

LIB=	rumpnet_bpfjit
COMMENT=JIT compiler for Berkeley Packet Filter

SRCS=	bpfjit.c                                                                

CPPFLAGS+=	-I ${NETBSDSRCDIR}/sys/external/bsd/sljit/dist/sljit_src    

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
