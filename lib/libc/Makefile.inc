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

USE_FORT?=	yes

USE_SHLIBDIR=	yes

.include <bsd.own.mk>

# build libc suitable for rumprun software stack, i.e. are system calls
# rump kernel function calls instead of kernel traps?
RUMPRUN?=	no

WARNS=5
CPPFLAGS+=	-D_LIBC -DLIBC_SCCS -DSYSLIBC_SCCS -D_REENTRANT
CPPFLAGS+=	-D_DIAGNOSTIC
.if defined(MLIBDIR)
CPPFLAGS+=	-DMLIBDIR=\"${MLIBDIR}\"
.endif

.if (${USE_HESIOD} != "no")
CPPFLAGS+=	-DHESIOD
.endif

.if (${USE_INET6} != "no")
CPPFLAGS+=	-DINET6
.endif

CPPFLAGS+=	-DNLS

.if (${USE_YP} != "no")
CPPFLAGS+=	-DYP
.endif

# Set lint to exit on warnings
#LINTFLAGS+=	-w
# ignore 'empty translation unit' warnings.
LINTFLAGS+=	-X 272

.include "libcincludes.mk"

ARCHDIR=	${.CURDIR}/arch/${ARCHSUBDIR}
AFLAGS+=	-I${ARCHDIR}
CLEANFILES+=	tags

# Don't try to lint the C library against itself when creating llib-lc.ln
LLIBS=

INCSDIR=	/usr/include
