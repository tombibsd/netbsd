# $NetBSD$

SUBDIR=		examples momdoc
DIST_SUBDIR=    contrib/mom

.include <bsd.init.mk>

TMACOWN?=	${BINOWN}
TMACGRP?=	${BINGRP}
TMACMODE?=	${NOBINMODE}
# XXX: next depends on ../src/include/defs.h
TMACDIR?=	/usr/share/tmac

NORMALFILES=	mom.tmac om.tmac
MAN=		groff_mom.7

.include <bsd.own.mk>

.if ${MKSHARE} != "no"
FILESDIR=	${TMACDIR}
FILES+=		${NORMALFILES}
.endif

CLEANFILES+=	groff_mom.7

.include <bsd.prog.mk>
.include <bsd.subdir.mk>
