#	@(#)Makefile	8.2 (Berkeley) 3/27/94
#	$NetBSD$

.include <bsd.prog.mk>

MELIB=	${BINDIR}/me
#TMLIB=	${BINDIR}/tmac
MESRCS=	acm.me chars.me deltext.me eqn.me float.me footnote.me \
	index.me letterhead.me local.me null.me refer.me sh.me \
	tbl.me thesis.me
#TMSRCS=	tmac.e

.if ${MKGROFF} == "no"
MAN=	me.7
.endif

CLEANFILES+= ${MESRCS:=.tmp}

.for M in ${MESRCS}

proginstall:: ${DESTDIR}${MELIB}/${M}
.if ${MKUPDATE} == "no"
.PHONY: ${DESTDIR}${MELIB}/${M}
.endif
.if !defined(BUILD)
${DESTDIR}${MELIB}/${M}: .MADE
.endif

${DESTDIR}${MELIB}/${M}: ${M}
	@rm -f ${.TARGET} ${M}.tmp
	${TOOL_SED} -f ${.CURDIR}/strip.sed < ${.ALLSRC} > ${M}.tmp
	${INSTALL_FILE:N-c} -o ${BINOWN} -g ${BINGRP} -m ${NONBINMODE} \
		${M}.tmp ${.TARGET}
	@rm -f ${M}.tmp
.endfor

#.for M in ${TMSRCS}

#proginstall:: ${DESTDIR}${TMLIB}/${M}
#.if ${MKUPDATE} == "no"
#.PHONY: ${DESTDIR}${TMLIB}/${M}
#.endif
#.if !defined(BUILD)
#${DESTDIR}${TMLIB}/${M}: .MADE
#.endif

#${DESTDIR}${TMLIB}/${M}: ${M}
#	@rm -f ${.TARGET} ${M}.tmp
#	${TOOL_SED} -f ${.CURDIR}/strip.sed < ${.ALLSRC} > ${M}.tmp
#	${INSTALL_FILE:N-c} -o ${BINOWN} -g ${BINGRP} -m ${NONBINMODE} \
#		${M}.tmp ${.TARGET}
#.endfor

proginstall:: ${DESTDIR}${MELIB}/revisions
.if ${MKUPDATE} == "no"
.PHONY: ${DESTDIR}${MELIB}/revisions
.endif
.if !defined(BUILD)
${DESTDIR}${MELIB}/revisions: .MADE
.endif

${DESTDIR}${MELIB}/revisions: revisions
	${INSTALL_FILE} -o ${BINOWN} -g ${BINGRP} -m ${NONBINMODE} \
	    ${.ALLSRC} ${.TARGET}

