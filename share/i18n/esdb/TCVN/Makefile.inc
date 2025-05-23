# $NetBSD$

.PATH: ${.CURDIR}/TCVN

CODESETS_TCVN!=	${TOOL_SED} \
			'/^\#/d;/^ *$$/d;/^[ 	]*/s///;s/[ 	].*//' \
			${.CURDIR}/TCVN/esdb.dir.TCVN.src
ESDB_TCVN:=	${CODESETS_TCVN:C/$/.esdb/:S/:/@/}

SRC_esdb.dir+=		esdb.dir.TCVN.src
SRC_esdb.alias+=	esdb.alias.TCVN.src
FILES+=			${ESDB_TCVN}
CLEANFILES+=		${ESDB_TCVN}
.for i in ${ESDB_TCVN}
FILESDIR_$i:=		${BINDIR}/TCVN
.endfor
