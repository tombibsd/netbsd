# $NetBSD$

.include <bsd.own.mk>


.if (${HOSTPROG:U} == "")
VERSION!=	cd ${.PARSEDIR}/dist && ${MAKE} -V VERSION
CPPFLAGS+=	-DVERSION=\"${VERSION}\" -I${DISTDIR}/../include
.endif

DISTDIR:=	${.PARSEDIR}/dist

.PATH:	${DISTDIR}

.for _LIB in mandoc
MDOCMLOBJDIR.${_LIB} !=	cd ${.PARSEDIR}/lib/lib${_LIB} && ${PRINTOBJDIR}
MDOCMLLIB.${_LIB}=	${MDOCMLOBJDIR.${_LIB}}/lib${_LIB}.a
.endfor

WARNS?=	4
