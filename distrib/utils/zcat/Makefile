# $NetBSD$
# Small zcat (i.e. for install media)
#
# Note: gzio.c is compiled here so that crunchgen will assume
# the same symbol space for zcat.c and gzio.c which is required
# so that the fake deflate functions in zcat.c will satisfy the
# references to those functions in gzio.c (yes, it's a hack).

SRCDIR=		${.CURDIR}/../../../common/dist/zlib

WARNS?=		4
PROG=		zcat
NOMAN=		# defined

SRCS=		zcat.c gzio.c

CPPFLAGS+=	-I${SRCDIR} -DNO_GZCOMPRESS
DPADD+=		${LIBZ}
LDADD+=		-lz

.include <bsd.prog.mk>

.PATH:		${SRCDIR}

test: zcat
	echo 'hello, hello!' | gzip | ./zcat
