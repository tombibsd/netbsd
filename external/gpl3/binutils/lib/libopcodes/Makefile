#	$NetBSD$

NOLINKLIB=	# defined
NOLINT=		# defined
NOMAN=		# defined
NOPROFILE=	# defined
NOCLANGERROR=	# defined

.include <bsd.init.mk>

LIB=		opcodes

BFD_MACHINE_ARCH?=	${MACHINE_ARCH:C/armv[4-7]/arm/}

.if exists(${.CURDIR}/arch/${BFD_MACHINE_ARCH}/defs.mk)
.include "${.CURDIR}/arch/${BFD_MACHINE_ARCH}/defs.mk"

SHLIB_MAJOR=	7
SHLIB_MINOR=	0

GCPPFLAGS=	${G_archdefs} ${G_DEFS} ${G_INCLUDES} ${G_TDEFAULTS}
CPPFLAGS+=	-I${.CURDIR}/arch/${BFD_MACHINE_ARCH} \
		-I${.CURDIR}/../libbfd/arch/${BFD_MACHINE_ARCH} \
		-I${DIST}/include -I. \
		-I${DIST}/bfd ${GCPPFLAGS:M-D*} \
		${GCPPFLAGS:M-I*:N-I.*}

GSRCS=		${G_BFD_MACHINES}
SRCS=		${GSRCS:.lo=.c} ${G_libopcodes_la_SOURCES}

.PATH: ${DIST}/opcodes

.include <bsd.lib.mk>

.else
.include <bsd.prog.mk> # do nothing
.endif
