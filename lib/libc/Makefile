#	$NetBSD$
#	@(#)Makefile	8.2 (Berkeley) 2/3/94
#
# All library objects contain sccsid strings by default; they may be
# excluded as a space-saving measure.  To produce a library that does
# not contain these strings, delete -DLIBC_SCCS and -DSYSLIBC_SCCS
# from CPPFLAGS below.  To remove these strings from just the system call
# stubs, remove just -DSYSLIBC_SCCS from CPPFLAGS.
#
# The NLS (message catalog) functions are always in libc.  To choose that
# strerror(), perror(), strsignal(), psignal(), etc. actually call the NLS
# functions, put -DNLS on the CPPFLAGS line below.
#
# The YP functions are always in libc. To choose that getpwent() and friends
# actually call the YP functions, put -DYP on the CPPFLAGS line below.
#
# The Hesiod functions are always in libc. To choose that getpwent() and friends
# actually call the Hesiod functions, put -DHESIOD on the CPPFLAGS line below.

.include "Makefile.inc"

LIB=		c
CPPFLAGS+=	-I${.CURDIR}/include -I${.CURDIR}

LIBCDIR=	${.CURDIR}

.if exists (${ARCHDIR}/Makefile.inc)
.PATH:	${ARCHDIR}
.include "${ARCHDIR}/Makefile.inc"
.endif

.if exists (${ARCHDIR}/genassym.cf)
DPSRCS+=	assym.h
CLEANFILES+=	assym.h assym.h.tmp

assym.h: ${ARCHDIR}/genassym.cf
	${_MKTARGET_CREATE}
	${TOOL_GENASSYM} -- ${CC} ${CFLAGS:N-Wa,*} \
		${CPPFLAGS} ${CPPFLAGS.assym.h} ${PROF} \
		${GENASSYM_CPPFLAGS} < ${ARCHDIR}/genassym.cf > assym.h.tmp && \
	mv -f assym.h.tmp assym.h
.endif

# The following controls how to build compatibility code for old NetBSD
# binaries. If BUILD_LEGACY is yes, then we build a separate library; otherwise
# we include the code in libc.
BUILD_LEGACY?= no
.if "${BUILD_LEGACY}" == "yes"
SUBDIR=compat
.include <bsd.subdir.mk>
.else
COMPATDIR=${.CURDIR}/compat
.include "${.CURDIR}/compat/Makefile.inc"
# Marker for compat code that can't be easily isolated
CPPFLAGS+=	-D__BUILD_LEGACY
.endif

.include "${.CURDIR}/../../common/lib/libc/Makefile.inc"
.include "${.CURDIR}/atomic/Makefile.inc"
.include "${.CURDIR}/cdb/Makefile.inc"
.include "${.CURDIR}/db/Makefile.inc"
.include "${.CURDIR}/citrus/Makefile.inc"
.include "${.CURDIR}/compat-43/Makefile.inc"
.include "${.CURDIR}/compiler_rt/Makefile.inc"
.include "${.CURDIR}/dlfcn/Makefile.inc"
.include "${.CURDIR}/gdtoa/Makefile.inc"
.include "${.CURDIR}/gen/Makefile.inc"
.include "${.CURDIR}/gmon/Makefile.inc"
.include "${.CURDIR}/hash/Makefile.inc"
.include "${.CURDIR}/iconv/Makefile.inc"
.include "${.CURDIR}/inet/Makefile.inc"
.include "${.CURDIR}/isc/Makefile.inc"
.include "${.CURDIR}/locale/Makefile.inc"
.include "${.CURDIR}/md/Makefile.inc"
.include "${.CURDIR}/misc/Makefile.inc"
.include "${.CURDIR}/net/Makefile.inc"
.include "${.CURDIR}/nameser/Makefile.inc"
.include "${.CURDIR}/nls/Makefile.inc"
.if (${USE_LIBTRE} == "yes")
.include "${NETBSDSRCDIR}/external/bsd/tre/Makefile.inc"
.else
.include "${.CURDIR}/regex/Makefile.inc"
.endif
.include "${.CURDIR}/resolv/Makefile.inc"
.include "${.CURDIR}/rpc/Makefile.inc"
.include "${.CURDIR}/ssp/Makefile.inc"
.include "${.CURDIR}/stdio/Makefile.inc"
.include "${.CURDIR}/stdlib/Makefile.inc"
.include "${.CURDIR}/string/Makefile.inc"
.include "${.CURDIR}/termios/Makefile.inc"
.include "${.CURDIR}/thread-stub/Makefile.inc"
.include "${.CURDIR}/time/Makefile.inc"
.if ${RUMPRUN} != "yes"
.include "${.CURDIR}/tls/Makefile.inc"
.endif
.include "${.CURDIR}/sys/Makefile.inc"
.if ${HAVE_LIBGCC_EH} == "no"
.include "${NETBSDSRCDIR}/sys/lib/libunwind/Makefile.inc"
.endif
.include "${.CURDIR}/uuid/Makefile.inc"
.if (${MKYP} != "no")
.include "${.CURDIR}/yp/Makefile.inc"
.endif

# Remove from SRCS the .c files for any .S files added by the MD makefiles,
# also remove from SRCS the .c files for the .S and .c files in NO_SRCS.
# Add the .c file for .S files (in both variables) to LSRCS so that the
# 'normal' .c file for assembly files is used for the lint librray.
#
# Usage:
# Add .S files to NO_SRSC when another .S file provides the entry points.
# Add .c files to NO_SRSC when another .c file provides the entry points.
# (lint is run on all .c files in SRCS)

.for check_file in ${SRCS:M*.S} ${NO_SRCS}
unwanted_file := ${SRCS:M${check_file:.S=.c}}
.if "${unwanted_file}" != ""
SRCS := ${SRCS:N${unwanted_file}}
.if "${unwanted_file}" != "${check_file}"
LSRCS := ${LSRCS} ${unwanted_file}
.endif
.endif
.endfor

NLS=	C.msg Pig.msg ca.msg cs.msg de.msg es.msg fi.msg fr.msg nl.msg \
	no.msg pl.msg sk.msg sv.msg

.if ${MKREPRO:Uno} == "yes"
REGEX_SPECIALS=[][)(^$$.?*\\;]
MKREPRO_SED=   -e 's;${NETBSDSRCDIR:C/${REGEX_SPECIALS}/\\\\&/g};/usr/src;'
.endif

.if !defined(MLIBDIR) && ${RUMPRUN} != "yes"
realall: tags
tags: ${SRCS}
	${_MKTARGET_CREATE}
	-${TOOL_CTAGS} -f ${.TARGET}.tmp -w ${.ALLSRC:M*.c}
	-egrep "^ENTRY(.*)|^FUNC(.*)|^SYSCALL(.*)" /dev/null ${.ALLSRC:M*.S} | \
	    sed "s;\([^:]*\):\([^(]*\)(\([^, )]*\)\(.*\);\3 \1 /^\2(\3\4$$/;" \
           >> ${.TARGET}.tmp
.if ${MKREPRO:Uno} == "yes"
	sed ${MKREPRO_SED} <${.TARGET}.tmp | sort -o ${.TARGET}
.else
	sort -o ${.TARGET} ${.TARGET}.tmp
.endif
	rm -f ${.TARGET}.tmp

FILES=		tags
FILESNAME=	libc.tags
FILESDIR=	/var/db
.endif


# workaround for I18N stuffs: build singlebyte setlocale() for libc.a,
# multibyte for libc.so.  the quirk should be removed when we support
# dlopen() from within statically linked binaries.
CSHLIBFLAGS+=	-D_I18N_DYNAMIC

.include <bsd.lib.mk>

# force the dynamic linker to initialize libc first
SHLIB_SHFLAGS+=	-Wl,-z,initfirst
.if ${HAVE_LIBGCC} == "no"
SHLIB_SHFLAGS+= -Wl,-z,defs
.endif
