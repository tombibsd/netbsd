#	$NetBSD$

IMAGESIZE=	7m
MAKEFS_FLAGS=	-o density=4096
IMAGEENDIAN=	be
MAKEDEVTARGETS=	all ipty
IMAGEDEPENDS+=	${ARCHDIR}/dot.profile ${ARCHDIR}/install.md \
		${KERNOBJDIR}/GENERIC/netbsd
LISTS+=		${DISTRIBDIR}/common/list.sysinst
