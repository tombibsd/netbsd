#	$NetBSD$

LIB=	lldbPluginObjectFileMachO

.include <bsd.init.mk>

.PATH: ${LLDB_SRCDIR}/source/Plugins/ObjectFile/Mach-O

SRCS+=	ObjectFileMachO.cpp

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
