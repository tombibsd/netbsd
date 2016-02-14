#	$NetBSD$

.include <bsd.own.mk>

.if ${MKBINUTILS} != "no"
SUBDIR+=	${EXTERNAL_BINUTILS_SUBDIR}
.endif

.if ${MKGCC} != "no"
SUBDIR+=	${EXTERNAL_GCC_SUBDIR}
.endif

.if ${MKGDB} != "no"
SUBDIR+=        ${EXTERNAL_GDB_SUBDIR}
.endif

.include <bsd.subdir.mk>
