#	$NetBSD$
#	@(#)Makefile	8.2 (Berkeley) 4/17/94

USE_FORT?= yes	# data-driven bugs?

PROG=    compress
SRCS=    compress.c zopen.c
LINKS=   ${BINDIR}/compress ${BINDIR}/uncompress
#LINKS+=  ${BINDIR}/compress ${BINDIR}/zcat
MLINKS=  compress.1 uncompress.1
#MLINKS+= compress.1 zcat.1

.include <bsd.prog.mk>
