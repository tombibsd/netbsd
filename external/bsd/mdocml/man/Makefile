# $NetBSD$

MAN=	mandoc_man.7 mandoc_char.7 mandoc_eqn.7 mandoc_mdoc.7 \
	mandoc_roff.7 mandoc_tbl.7

.include <bsd.init.mk>

mandoc_eqn.7: ${DISTDIR}/eqn.7
	${_MKTARGET_CREATE}
	rm -f ${.TARGET}
	${TOOL_CAT} ${DISTDIR}/eqn.7 > ${.TARGET}

mandoc_man.7: ${DISTDIR}/man.7
	${_MKTARGET_CREATE}
	rm -f ${.TARGET}
	${TOOL_CAT} ${DISTDIR}/man.7 > ${.TARGET}

mandoc_mdoc.7: ${DISTDIR}/mdoc.7
	${_MKTARGET_CREATE}
	rm -f ${.TARGET}
	${TOOL_CAT} ${DISTDIR}/mdoc.7 > ${.TARGET}

mandoc_roff.7: ${DISTDIR}/roff.7
	${_MKTARGET_CREATE}
	rm -f ${.TARGET}
	${TOOL_CAT} ${DISTDIR}/roff.7 > ${.TARGET}

mandoc_tbl.7: ${DISTDIR}/tbl.7
	${_MKTARGET_CREATE}
	rm -f ${.TARGET}
	${TOOL_CAT} ${DISTDIR}/tbl.7 > ${.TARGET}

CLEANFILES+=	mandoc_eqn.7 mandoc_man.7 mandoc_mdoc.7 \
		mandoc_roff.7 mandoc_tbl.7

.if (${MKHTML} != "no") && (${MKMAN} != "no")
FILES=		${DISTDIR}/example.style.css
FILESDIR=	/usr/share/man
FILESNAME=	style.css
.endif

.include <bsd.man.mk>
