#	$NetBSD$
#

LIB=		c_vfp

USE_SHLIBDIR=	yes

.include <bsd.own.mk>

CPUFLAGS+=	-mfpu=vfp -marm

SRCS=		vfpsf.S vfpdf.S
AFLAGS.vfpsf.S+=	-mfpu=vfp
AFLAGS.vfpdf.S+=	-mfpu=vfp

.include <bsd.lib.mk>
