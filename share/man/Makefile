#	$NetBSD$
#	from: @(#)Makefile	8.2 (Berkeley) 4/16/94

.include <bsd.own.mk>

.if ${MKSHARE} != "no"
SUBDIR=	man0 man1 man2 man3 man3lua man4 man5 man6 man7 man8 man9 man9lua

.if ${MKRUMP} != "no"
SUBDIR+=${.CURDIR}/../../sys/rump/share/man
.endif

WHATISDBDIR?=	/usr/share/man

.if ${MKMAKEMANDB} == "no"
makedb:
	${TOOL_MAKEWHATIS} ${DESTDIR}${WHATISDBDIR}
.if ${MKUNPRIVED} != "no"
	echo ".${WHATISDBDIR}/whatis.db type=file mode=0444 uname=${BINOWN} gname=${BINGRP}" \
	    | ${METALOG.add}
.endif
.else
makedb:
.endif

.else
makedb:
.endif

.include <bsd.subdir.mk>
