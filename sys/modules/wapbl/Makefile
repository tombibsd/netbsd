#	$NetBSD$

.include "../Makefile.inc"

.PATH:	${S}/kern

KMOD=	wapbl

CPPFLAGS+=      -DWAPBL

#CWARNFLAGS.clang=	-Wno-conversion

SRCS=	vfs_wapbl.c

.include <bsd.kmodule.mk>
