#	$NetBSD$

.include <bsd.own.mk>
.include "${NETBSDSRCDIR}/distrib/common/Makefile.distrib"

IMAGE=		ramdisk.fs
IMAGESIZE=	2860k
MAKEFS_FLAGS=	-f 15

WARNS=		1
DBG=		-Os -mmemcpy -fno-unwind-tables

CRUNCHBIN=	ramdiskbin
CRUNCHGEN_FLAGS= -d "${DBG}"
LISTS=		${.CURDIR}/list ${DISTRIBDIR}/common/list.sysinst.en
MTREECONF=	${DISTRIBDIR}/common/mtree.common
IMAGEENDIAN=	le
MAKEDEVTARGETS=	minimal
IMAGEDEPENDS=	${CRUNCHBIN} \
		dot.profile \
		${NETBSDSRCDIR}/etc/group ${NETBSDSRCDIR}/etc/master.passwd \
		${NETBSDSRCDIR}/etc/netconfig ${DISTRIBDIR}/common/protocols \
		${DISTRIBDIR}/common/services
SMALLPROG_INET6=1

# Use stubs to eliminate some large stuff from libc
HACKSRC=	${DISTRIBDIR}/utils/libhack
.include	"${HACKSRC}/Makefile.inc"
${CRUNCHBIN}:	libhack.o

.include "${DISTRIBDIR}/common/Makefile.crunch"
.include "${DISTRIBDIR}/common/Makefile.makedev"
.include "${DISTRIBDIR}/common/Makefile.image"

release:

.include <bsd.prog.mk>
