#	$NetBSD$

.include <bsd.own.mk>
.include <bsd.init.mk>

CPPFLAGS+=	-DELFSIZE=32 -DELF32_COMPAT
LIB=	ldd_elf32_compat

# XXX Force one member
SRCS=	dummy.c

LIBISPRIVATE=	yes
.PATH: ${.CURDIR}/..

.ifdef COMPAT_MLIBDIR
MLIBDIR=	${COMPAT_MLIBDIR}
CPPFLAGS+=	-DRTLD_ARCH_SUBDIR=\"${MLIBDIR}\"
.endif

.ifdef MLIBDIR
.include "../Makefile.elf"
.endif

.include <bsd.lib.mk>
