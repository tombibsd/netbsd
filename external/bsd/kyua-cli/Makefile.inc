# $NetBSD$

.include <bsd.own.mk>

TOPDIR=		${NETBSDSRCDIR}/external/bsd/kyua-cli
SRCDIR=		${TOPDIR}/dist

# Name of the private libraries (without their lib prefix) to depend on.
KYUA_LIBS?=

# Layout of installed files.
KYUA_BINDIR=		/usr/bin
KYUA_CONFDIR=		/etc/kyua
KYUA_DOCDIR=		/usr/share/doc/reference/ref1/kyua/kyua-cli
KYUA_EXAMPLESDIR=	/usr/share/examples/kyua-cli
KYUA_MISCDIR=		/usr/share/kyua-cli/misc
KYUA_STOREDIR=		/usr/share/kyua-cli/store
KYUA_TESTERSDIR=	/usr/libexec
KYUA_TESTSDIR=		${TESTSBASE}

KYUA_TARNAME!=	grep 'define PACKAGE_TARNAME' ${TOPDIR}/config.h \
		| cut -d '"' -f 2
cookie-tarname: cookie-tarname-2
	@cmp -s cookie-tarname cookie-tarname-2 \
	    || cp cookie-tarname-2 cookie-tarname
cookie-tarname-2: .PHONY
	@echo "${KYUA_TARNAME}" >cookie-tarname-2
CLEANFILES+=	cookie-tarname cookie-tarname-2
	
KYUA_VERSION!=	grep 'define VERSION' ${TOPDIR}/config.h | cut -d '"' -f 2
cookie-version: cookie-version-2
	@cmp -s cookie-version cookie-version-2 \
	    || cp cookie-version-2 cookie-version
cookie-version-2: .PHONY
	@echo "${KYUA_VERSION}" >cookie-version-2
CLEANFILES+=	cookie-version cookie-version-2

CPPFLAGS+=	-DHAVE_CONFIG_H

# Not all source files need these settings, but it's just easier to
# set them in all cases.
CPPFLAGS+=	-DKYUA_CONFDIR=\"${KYUA_CONFDIR}\"
CPPFLAGS+=	-DKYUA_DOCDIR=\"${KYUA_DOCDIR}\"
CPPFLAGS+=	-DKYUA_EXAMPLESDIR=\"${KYUA_EXAMPLESDIR}\"
CPPFLAGS+=	-DKYUA_MISCDIR=\"${KYUA_MISCDIR}\"
CPPFLAGS+=	-DKYUA_STOREDIR=\"${KYUA_STOREDIR}\"
CPPFLAGS+=	-DKYUA_TESTERSDIR=\"${KYUA_TESTERSDIR}\"
CPPFLAGS+=	-DKYUA_TESTSDIR=\"${KYUA_TESTSDIR}\"

CPPFLAGS+=	-I${TOPDIR}		# For config.h.
CPPFLAGS+=	-I${TOPDIR}/lib		# For utils/defs.hpp.
CPPFLAGS+=	-I${SRCDIR}

.for lib in ${KYUA_LIBS}
LIBOBJDIR_${lib}!=	cd ${TOPDIR}/lib/${lib}; ${PRINTOBJDIR}
LDADD+=			${LIBOBJDIR_${lib}}/lib${lib}.a
DPADD+=			${LIBOBJDIR_${lib}}/lib${lib}.a
.endfor

.if !empty(KYUA_LIBS:Mutils)
LDADD+=		-llutok -lsqlite3
DPADD+=		${LIBLUTOK} ${LIBSQLITE}
.endif

MANPAGE_DEPS = cookie-tarname cookie-version
BUILD_MANPAGE = \
	sed -e 's,__CONFDIR__,${KYUA_CONFDIR},g' \
	    -e 's,__DOCDIR__,${KYUA_DOCDIR},g' \
	    -e 's,__EGDIR__,${KYUA_EXAMPLESDIR},g' \
	    -e 's,__MISCDIR__,${KYUA_MISCDIR},g' \
	    -e 's,__PACKAGE__,${KYUA_TARNAME},g' \
	    -e 's,__STOREDIR__,${KYUA_STOREDIR},g' \
	    -e 's,__TESTERSDIR__,${KYUA_TESTERSDIR},g' \
	    -e 's,__TESTSDIR__,${KYUA_TESTSDIR},g' \
	    -e 's,__VERSION__,${KYUA_VERSION},g' \
	    <${SRCDIR}/doc/$${name}.in >$${name}.tmp; \
	mv $${name}.tmp $${name}
