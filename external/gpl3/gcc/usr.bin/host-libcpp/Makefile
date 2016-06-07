#	$NetBSD$

libcpp/libcpp.a:
	[ ! -d libcpp ] && mkdir libcpp || true
	(cd libcpp; \
		CC=${HOST_CC:Q} \
		CFLAGS=${HOST_CFLAGS:Q} \
		MAKE=${TOOL_GMAKE:Q} \
		CONFIG_SHELL=${HOST_SH:Q} \
		${HOST_SH} ${DIST}/libcpp/configure \
		&& CC=${HOST_CC:Q} CFLAGS=${HOST_CFLAGS:Q} ${TOOL_GMAKE})

cleandir:
	-rm -rf libcpp

depend dependall all: libcpp/libcpp.a

.include <bsd.prog.mk>
