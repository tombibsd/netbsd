# $NetBSD$

NOMAN=		# defined

.include <bsd.own.mk>

TESTSDIR=	${TESTSBASE}/usr.bin/bzip2

TESTS_SH=	t_bzip2

DIST=           ${NETBSDSRCDIR}/external/bsd/bzip2/dist
.PATH:		${DIST}

FILESDIR=	${TESTSDIR}
.for f in sample1.bz2 sample1.ref sample2.bz2 sample2.ref \
          sample3.bz2 sample3.ref
FILES+=		${f}
FILESNAME_${f}=	d_${f}
.endfor

.include <bsd.test.mk>
