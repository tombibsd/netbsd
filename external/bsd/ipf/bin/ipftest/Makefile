#	$NetBSD$

.include <bsd.own.mk>

PROG=		ipftest
SRCS=		ipftest.c fil.c ip_frag.c ip_state.c ip_nat.c ip_nat6.c \
		ip_proxy.c ip_auth.c ip_htable.c ip_lookup.c \
		ip_dstlist.c ip_pool.c ip_sync.c \
		ip_fil.c ip_log.c ippool_y.c ippool_l.c ipf_y.c \
		ipf_l.c ipnat_y.c ipnat_l.c radix_ipf.c bpf_filter.c
MAN=		ipftest.1

CPPFLAGS+=	-DIPFILTER_LOG -DIPFILTER_LOOKUP \
		-DIPFILTER_SYNC -DIPFILTER_CKSUM \
		-I. -D_KERNTYPES

COPTS.radix.c=	-Wno-error
COPTS+=		-fno-strict-aliasing

.PATH:		${NETBSDSRCDIR}/sys/external/bsd/ipf/netinet

DPSRCS+=	ipnat_l.h ipnat_y.h ippool_l.h ippool_y.h ipf_l.h ipf_y.h

CLEANFILES+=	ipf_y.c ipf_y.h
CLEANFILES+=	ipf_l.c ipf_l.h
CLEANFILES+=	ipf.tab.c ipf.tab.h
CLEANFILES+=	ipnat_y.c ipnat_y.h
CLEANFILES+=	ipnat_l.c ipnat_l.h
CLEANFILES+=	ipnat.tab.c ipnat.tab.h
CLEANFILES+=	ippool_y.c ippool_y.h
CLEANFILES+=	ippool_l.c ippool_l.h
CLEANFILES+=	ippool.tab.c ippool.tab.h

ipnat_y.c: ipnat_y.y
	${_MKTARGET_CREATE}
	${YACC} -b ipnat -d ${.ALLSRC}
	${TOOL_SED} -e 's/yy/ipnat_yy/g' \
	    -e 's/y.tab.c/ipnat_y.c/' \
	    -e s/\"ipnat_y.y\"/\"..\\/tools\\/ipnat_y.y\"/ \
	    ipnat.tab.c > ${.TARGET}
	${TOOL_SED} -e 's/yy/ipnat_yy/g' \
	    -e 's/y.tab.h/ipnat_y.h/' \
	    ipnat.tab.h > ${.TARGET:.c=.h}

ipnat_y.h: ipnat_y.c

ipnat_l.c: lexer.c
	${_MKTARGET_CREATE}
	${TOOL_SED} -e 's/yy/ipnat_yy/g' \
	    -e 's/y.tab.h/ipnat_y.h/' \
	    -e 's/lexer.h/ipnat_l.h/' \
	    ${.ALLSRC} > ${.TARGET}

ipnat_l.h: lexer.h
	${_MKTARGET_CREATE}
	${TOOL_SED} -e 's/yy/ipnat_yy/g' \
	    ${.ALLSRC} > ${.TARGET}

ippool_y.c: ippool_y.y
	${_MKTARGET_CREATE}
	${YACC} -b ippool -d ${.ALLSRC}
	${TOOL_SED} -e 's/yy/ippool_yy/g' \
	    -e 's/"ippool_y.y"/"..\/tools\/ippool_y.y"/' \
	    ippool.tab.c > ${.TARGET}
	${TOOL_SED} -e 's/yy/ippool_yy/g' \
	    ippool.tab.h > ${.TARGET:.c=.h}

ippool_y.h: ippool_y.c

ippool_l.c: lexer.c
	${_MKTARGET_CREATE}
	${TOOL_SED} -e 's/yy/ippool_yy/g' \
	    -e 's/y.tab.h/ippool_y.h/' \
	    -e 's/lexer.h/ippool_l.h/' \
	    ${.ALLSRC} > ${.TARGET}

ippool_l.h: lexer.h
	${_MKTARGET_CREATE}
	${TOOL_SED} -e 's/yy/ippool_yy/g' \
	    ${.ALLSRC} > ${.TARGET}

ipf_y.c: ipf_y.y
	${_MKTARGET_CREATE}
	${YACC} -b ipf -d ${.ALLSRC}
	${TOOL_SED} -e 's/yy/ipf_yy/g' \
	    -e 's/"ipf_y.y"/"..\/tools\/ipf_y.y"/' \
		ipf.tab.c > ${.TARGET}
	${TOOL_SED} -e 's/yy/ipf_yy/g' \
		ipf.tab.h > ${.TARGET:.c=.h}

ipf_y.h: ipf_y.c

ipf_l.c: lexer.c
	${_MKTARGET_CREATE}
	${TOOL_SED} -e 's/yy/ipf_yy/g' \
	     -e 's/y.tab.h/ipf_y.h/' \
	     -e 's/lexer.h/ipf_l.h/' \
	    ${.ALLSRC} > ${.TARGET}

ipf_l.h: lexer.h
	${_MKTARGET_CREATE}
	${TOOL_SED} -e 's/yy/ipf_yy/g' \
	    ${.ALLSRC} > ${.TARGET}

.include <bsd.prog.mk>
