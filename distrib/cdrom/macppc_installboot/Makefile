#	$NetBSD$

NOMAN=	# defined

.include <bsd.init.mk>
HOSTPROG=	macppc_installboot
SRCS=		installboot.c cd9660.c cd9660_util.c

.PATH.c: ${NETBSDSRCDIR}/sys/fs/cd9660

HOST_CPPFLAGS+=	-I. -I${.CURDIR}
#HOST_CPPFLAGS+=	-DDEBUG

.if !make(obj) && !make(clean) && !make(cleandir)
.BEGIN:
	-rm -rf fs
	${HOST_INSTALL_DIR} fs
	${HOST_INSTALL_DIR} fs/cd9660
	${HOST_LN} -s ${NETBSDSRCDIR}/sys/fs/unicode.h fs
	${HOST_LN} -s ${NETBSDSRCDIR}/sys/fs/cd9660/iso.h fs/cd9660
.endif

cleandir distclean: cleaninc

cleaninc:
	-rm -rf fs

.include <bsd.own.mk>

.ifndef NOCOMPATLIB
.-include	"${TOOLDIR}/share/compat/defs.mk"
.endif

.include <bsd.hostprog.mk>
