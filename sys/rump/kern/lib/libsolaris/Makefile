#	$NetBSD$
#

S!=             cd ${.PARSEDIR}/../../../../;pwd

.include "${.CURDIR}/../../../../modules/solaris/Makefile.solmod"

LIB=	rumpkern_solaris
COMMENT=Solaris compatibility layer (for ZFS)

CPPFLAGS+=	-DASSERT=KASSERT

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
