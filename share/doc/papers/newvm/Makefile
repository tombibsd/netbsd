#	$NetBSD$
#
#	@(#)Makefile	1.4 (Berkeley) 6/8/93

DIR=	papers/newvm
SRCS=	0.t 1.t a.t
MACROS=	-ms

paper.ps: ${SRCS}
	${TOOL_TBL} ${SRCS} | ${TOOL_ROFF_PS} ${MACROS} > ${.TARGET}

.include <bsd.doc.mk>
