#	$NetBSD$

.PATH: ${.CURDIR}/../../dist/lib/libuutil/common
.PATH: ${.CURDIR}/../../dist/common/avl

CPPFLAGS+=-Wall -Wno-unknown-pragmas

LIB=	uutil
SRCS=	avl.c uu_alloc.c uu_avl.c uu_dprintf.c uu_ident.c uu_list.c \
	uu_misc.c uu_open.c uu_pname.c uu_strtoint.c

CFLAGS+= -DNATIVE_BUILD

.include "../../Makefile.zfs"
.include <bsd.lib.mk>
