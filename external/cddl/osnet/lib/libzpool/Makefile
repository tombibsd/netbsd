#	$NetBSD$

.include "${.CURDIR}/../../dist/uts/common/Makefile.files"
.include "../../Makefile.zfs"

.PATH: ${.CURDIR}/../../dist/common/zfs
.PATH: ${.CURDIR}/../../dist/common/unicode
.PATH: ${.CURDIR}/../../dist/lib/libzpool/common
.PATH: ${.CURDIR}/../../dist/uts/common/fs/zfs
.PATH: ${.CURDIR}/../../dist/uts/common/os

LIBDPLIBS+= \
	avl	${.CURDIR}/../libavl \
	nvpair	${.CURDIR}/../libnvpair \
	pthread	${NETBSDSRCDIR}/lib/libpthread \
	umem	${.CURDIR}/../libumem \
	z	${NETBSDSRCDIR}/lib/libz \

LIB=		zpool
CPPFLAGS+=	-std=c99
#CPPFLAGS+=-Wall -Wno-unknown-pragmas

# Local stuff
SRCS=		kernel.c kernel2.c atomic.c

# Sun stuff
SRCS+=		${ZFS_COMMON_OBJS:C/.o$/.c/}
SRCS+=		${ZFS_SHARED_OBJS:C/.o$/.c/}
SRCS+=		taskq.c util.c list.c u8_textprep.c

print:
	echo ${LIST_SRCS} ${LIST_OBJS}


.include <bsd.lib.mk>
