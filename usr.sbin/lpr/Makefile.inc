#	$NetBSD$

.include <bsd.own.mk>

USE_FORT?=yes	# network client/server *and* setgid

LIBCOMMON != cd ${.CURDIR}/../common_source && ${PRINTOBJDIR}
CPPFLAGS+=-I${.CURDIR}/../common_source
DPADD+=	${LIBCOMMON}/libcommon.a
LDADD+=	-L${LIBCOMMON} -lcommon

.if exists(${.CURDIR}/../../Makefile.inc)
.include "${.CURDIR}/../../Makefile.inc"
.endif
