#	$NetBSD$

.include <bsd.own.mk>

.if (defined(HOSTPROG) && \
     (${TOOLCHAIN_MISSING} == "no" || defined(EXTERNAL_TOOLCHAIN))) || \
    ${MKBINUTILS} != "no"
BFDSUBDIR=	lib

PROG=		mdsetimage
MAN=		mdsetimage.8

CPPFLAGS+=	-I${DIST}/bfd -I${DIST}/binutils -I${DIST}/include

.if !defined(HOSTPROG)
PROGDPLIBS+=	bfd	${TOP}/${BFDSUBDIR}/libbfd
PROGDPLIBS+=	iberty	${TOP}/${BFDSUBDIR}/libiberty
.else
DPADD+=		${BFDDIR}/libbfd.a
LDADD+=		-L${BFDDIR} -lbfd
DPADD+=		${IBERTYDIR}/libiberty.a
LDADD+=		-L${IBERTYDIR} -liberty
CPPFLAGS+=	-I${BFDDIR}
.endif

.include <bsd.prog.mk>

.ifndef HOSTPROG
.include	"${TOP}/${BFDSUBDIR}/libbfd/arch/${BINUTILS_MACHINE_ARCH}/defs.mk"

CPPFLAGS+=	-I${TOP}/${BFDSUBDIR}/libbfd/arch/${BINUTILS_MACHINE_ARCH} ${G_INCLUDES}
LDADD+=		-lintl
DPADD+=		${LIBINTL}
DPADD+=		${LIBZ}
.endif # HOSTPROG

LDADD+=		-lz
# No DPADD because we don't know the format and we didn't build it.

.endif # MKBINUTILS != no
