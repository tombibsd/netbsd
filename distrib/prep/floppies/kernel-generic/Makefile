#	$NetBSD$

.include <bsd.own.mk>
.include "${NETBSDSRCDIR}/distrib/common/Makefile.distrib"

.include <bsd.kernobj.mk>

KERN=		${KERNOBJDIR}/GENERIC/netbsd
KERNEL=		netbsd-GENERIC.gz

CLEANFILES+=	${KERNEL}

${KERNEL}: ${KERN}
	-rm -f ${.TARGET}
	${TOOL_GZIP_N} -9c ${.ALLSRC} > ${.TARGET}

realall: ${KERNEL}

release: check_RELEASEDIR .WAIT ${KERNEL}
	${RELEASE_INSTALL} ${KERNEL} ${RELEASEDIR}/${RELEASEMACHINEDIR}/binary/kernel

.include <bsd.prog.mk>
