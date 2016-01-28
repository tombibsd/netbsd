#	$NetBSD$

PROG=	gettext

CLEANFILES+=gettext.1

.include "${.CURDIR}/../Makefile.inc.prog"

gettext.1: ${DIST}/gettext/gettext-runtime/man/gettext.1.in
	cp $> $@
