#	$NetBSD$

.include "${.CURDIR}/../Makefile.inc"

DIST=		${IDIST}/src
.PATH:		${DIST} ${IDIST}/doc ${IDIST}/man ${.CURDIR}/../po

PROG=		grep
SRCS=		grep.c dfa.c kwset.c search.c grepmat.c

CWARNFLAGS.clang+=	-Wno-format-security -Wno-tautological-compare

.ifndef HOSTPROG
DPADD+=		${LIBGREPUTILS} ${LIBINTL}
LDADD+=		${LIBGREPUTILS} -lintl
.else
.PATH:	${IDIST}/lib
SRCS+=	closeout.c error.c exclude.c hard-locale.c isdir.c \
	obstack.c quotearg.c regex.c savedir.c stpcpy.c \
	xmalloc.c xstrtol.c xstrtoumax.c
.endif

INFOFLAGS=	-I${IDIST}/doc -I${.CURDIR}/../include
TEXINFO=	grep.texi

LINKS=		${BINDIR}/grep ${BINDIR}/egrep \
		${BINDIR}/grep ${BINDIR}/fgrep
MLINKS=		grep.1 egrep.1 \
		grep.1 fgrep.1

.if !defined(HOSTPROG) && ${MKNLS} != "no"
.for F in cs.gmo de.gmo el.gmo eo.gmo es.gmo et.gmo fr.gmo gl.gmo hr.gmo \
	id.gmo it.gmo ja.gmo ko.gmo nl.gmo no.gmo pl.gmo pt_BR.gmo ru.gmo \
	sl.gmo sv.gmo
FILES+=		${F}
FILESDIR_${F}=	/usr/share/locale/${F:.gmo=}/LC_MESSAGES
FILESNAME_${F}=	grep.mo
.endfor
.endif

.include <bsd.info.mk>
.include <bsd.prog.mk>
