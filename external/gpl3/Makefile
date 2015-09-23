#	$NetBSD$

.include <bsd.own.mk>

.if ${MKBINUTILS} != "no"
SUBDIR+=	binutils
.endif

.if ${MKGCC} != "no"
SUBDIR+=	${EXTERNAL_GCC_SUBDIR}
.endif

.if ${MKGDB} != "no"
SUBDIR+=        gdb
.endif

.include <bsd.subdir.mk>
