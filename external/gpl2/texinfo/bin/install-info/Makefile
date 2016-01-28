#	$NetBSD$

.include <bsd.init.mk>

PROG=		install-info

.include <bsd.prog.mk>

.PATH: ${IDIST}/util ${IDIST}/doc

infodir-meta:
.if (${MKUNPRIVED} != "no" && ${MKINFO} != "no")
	echo "./usr/share/info/dir type=file mode=0644 uname=${BINOWN} gname=${BINGRP}" \
	    | ${METALOG.add}
.endif
