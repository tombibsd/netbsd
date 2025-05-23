# $NetBSD$

.include "../Makefile.inc"

BINDIR=		/bin

.if (${MKDYNAMICROOT} == "no")
LDSTATIC?=	-static
.endif

DPADD=	${LIBARCHIVE_FE} ${LIBARCHIVE} ${LIBBZ2} ${LIBLZMA} ${LIBZ}
LDADD=	-L${LIBARCHIVE_FE_DIR} -larchive_fe -larchive -lbz2 -llzma -lz
CPPFLAGS+=	-I${LIBARCHIVEDIR}/libarchive_fe
