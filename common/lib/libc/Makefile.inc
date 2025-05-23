# $NetBSD$

.include <bsd.own.mk>

COMMON_DIR:=${.PARSEDIR}
COMMON_CODEDIRS=atomic gen gmon inet md net stdlib string sys
COMMON_CODEDIRS+=hash/sha1 hash/sha2 hash/rmd160 hash/murmurhash

.if defined(COMMON_MACHINE_ARCH) && !empty(COMMON_MACHINE_ARCH) && \
    exists(${COMMON_DIR}/arch/${COMMON_MACHINE_ARCH})
COMMON_ARCHSUBDIR=	${COMMON_MACHINE_ARCH}
.elif defined(COMMON_MACHINE_CPU) && !empty(COMMON_MACHINE_CPU) && \
    exists(${COMMON_DIR}/arch/${COMMON_MACHINE_CPU})
COMMON_ARCHSUBDIR=	${COMMON_MACHINE_CPU}
.elif defined(LIBKERN_ARCH) && !empty(LIBKERN_ARCH) && \
    exists(${KERNDIR}/arch/${LIBKERN_ARCH})
COMMON_ARCHSUBDIR=	${LIBKERN_ARCH}
.elif exists(${COMMON_DIR}/arch/${MACHINE_ARCH})
COMMON_ARCHSUBDIR=	${MACHINE_ARCH}
.elif exists(${COMMON_DIR}/arch/${MACHINE_CPU})
COMMON_ARCHSUBDIR=	${MACHINE_CPU}
.endif

COMMON_ARCHDIR=${COMMON_DIR}/arch/${COMMON_ARCHSUBDIR}

.for i in ${COMMON_CODEDIRS}
.if exists(${COMMON_DIR}/$i)
.PATH.c: ${COMMON_DIR}/$i
.endif
.if exists(${COMMON_ARCHDIR}/${i}/Makefile.inc)
.include "${COMMON_ARCHDIR}/${i}/Makefile.inc"
.endif
.if !empty(CPPFLAGS:M-DLIBKERN_OPTIMISE_SPACE) && \
    exists(${COMMON_ARCHDIR}/$i/small)
.PATH.S: ${COMMON_ARCHDIR}/$i/small
.endif

.if exists(${COMMON_ARCHDIR}/$i)
.PATH.c: ${COMMON_ARCHDIR}/$i
.PATH.S: ${COMMON_ARCHDIR}/$i
.endif
.endfor

CPPFLAGS+=-I${COMMON_DIR}/quad -I${COMMON_DIR}/string
.if defined(COMMON_ARCHSUBDIR)
CPPFLAGS+=-I${COMMON_ARCHDIR}/string
.endif
