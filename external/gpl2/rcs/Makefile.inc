# $NetBSD$

.include <bsd.own.mk>

BINDIR=/usr/bin
DIST=${NETBSDSRCDIR}/external/gpl2/rcs
IDIST=${DIST}/dist

PROGDPLIBS+=	rcs ${DIST}/lib
CPPFLAGS+=	-I${DIST}/include

.PATH: ${IDIST}/src ${IDIST}/man
