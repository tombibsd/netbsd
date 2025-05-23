#	$NetBSD$
#
# Makefile for sysinst

.if exists(${.CURDIR}/../../../Makefile.inc)
.include "${.CURDIR}/../../../Makefile.inc"
.endif

PROG=		sysinst
NOMAN=		# defined

WARNS=		4

SRCS+=	menu_defs.c msg_defs.c main.c install.c upgrade.c \
	txtwalk.c run.c factor.c net.c disks.c disks_lfs.c util.c geom.c \
	label.c target.c md.c sizemultname.c configmenu.c checkrc.c partman.c

SRCS+=	${MD_OPTIONS:MAOUT2ELF:S/AOUT2ELF/aout2elf.c/}
SRCS+=	${MENUS_MD:Mmenus.mbr:S/menus.mbr/mbr.c/}
SRCS+=	${CPPFLAGS:M-DWSKBD:S/-DWSKBD/wskbd.c/}
SRCS+=	${NODISKLABEL:D:Ubsddisklabel.c savenewlabel.c}

DPSRCS+= defs.h

SYSINSTLANG?=	en
LANGUAGES?=	de en es fr pl

MSG_MD?=	msg.md.${SYSINSTLANG}
MENUS_MD?=	menus.md.${SYSINSTLANG}

.include <bsd.own.mk>		# for mk.conf
#.include "${NETBSDSRCDIR}/distrib/common/Makefile.distrib"

DISTRIBVER!=	${HOST_SH} ${NETBSDSRCDIR}/sys/conf/osrelease.sh
DISTRIBVERDEP=	${NETBSDSRCDIR}/sys/sys/param.h \
		${NETBSDSRCDIR}/sys/conf/osrelease.sh

_MKSHTARGET_CREATE?=	${_MKSHMSG_CREATE} ${.CURDIR:T}/${.TARGET}

DPADD=		${LIBCURSES} ${LIBTERMLIB} ${LIBUTIL}
LDADD=		-lcurses -ltermlib -lutil

UNIF_AWK=	${.CURDIR}/../../unif.awk
MSG_XLAT_SH=	${.CURDIR}/../../msg_xlat.sh

CATALOGDIR=	/usr/share/sysinst/catalog
CPPFLAGS+=	-I. -I${.CURDIR}/../.. -I${.CURDIR} \
		-DREL=\"${DISTRIBVER}\" -DMACH=\"${MACHINE}\" \
		-DMACH_${MACHINE} -DARCH_${MACHINE_ARCH} \
		${NODISKLABEL:D-DNO_DISKLABEL}

.if !defined(DEBUG)
CPPFLAGS+=	-DCATALOG_DIR=\"${CATALOGDIR}\"
.endif

.if (defined(DEBUG))
.if ${ACTIVE_CC} == "gcc"
# make gcc run additional passes for better debug info (only needed
# when compiling with -O0)
O0TRACKING=	-fvar-tracking-assignments -fvar-tracking
.endif
CPPFLAGS+=-DDEBUG
DBG +=-g -O0 ${O0TRACKING}
.endif

.if empty(DBG:M-g)
COPTS += -Os
.endif

.if (!defined(SMALLPROG) || defined(SMALLPROG_INET6)) && (${USE_INET6} != "no")
CPPFLAGS+=-DINET6
.endif

.if defined(HAVE_GCC) || defined(HAVE_PCC)
COPTS.label.c+=	-Wno-pointer-sign
.endif
COPTS.main.c += -Wno-format-nonliteral
COPTS.disks.c += -Wno-format-nonliteral
COPTS.util.c += -Wno-format-nonliteral
COPTS.label.c += -Wno-format-nonliteral
COPTS.target.c += -Wno-format-nonliteral
COPTS.mbr.c += -Wno-format-nonliteral
COPTS.msg_defs.c += -Wno-format-nonliteral
COPTS.bsddisklabel.c += -Wno-format-nonliteral
COPTS.md.c += -Wno-format-nonliteral
COPTS.partman.c += -Wno-format-nonliteral -Wno-stack-protector

# Host to ftp from.  Default:
#	"ftp.NetBSD.org"
#
.if defined(SYSINST_FTP_HOST)
CPPFLAGS+=	-DSYSINST_FTP_HOST=\"${SYSINST_FTP_HOST}\"
.endif

# Top-level ftp directory.  Default:
#	"pub/NetBSD/NetBSD-" + VER + "/" + MACH
#
.if defined(SYSINST_FTP_DIR)
CPPFLAGS+=	-DSYSINST_FTP_DIR=\"${SYSINST_FTP_DIR}\"
.endif

# Top-level CDROM directory.  Default:
#	"/" + MACH
#
.if defined(SYSINST_CDROM_DIR)
CPPFLAGS+=	-DSYSINST_CDROM_DIR=\"${SYSINST_CDROM_DIR}\"
.endif


DPSRCS+=	msg_defs.h menu_defs.h

CLEANFILES=	menu_defs.c menu_defs.h menus.def \
		msg_defs.c msg_defs.h msg.def msgtouch \
		sysinstmsgs.*

.PATH: ${.CURDIR}/../..

menu_defs.h: menu_defs.c
	[ -f ${.TARGET} ] || { \
		${_MKSHTARGET_CREATE}; \
		${TOOL_MENUC} menus.def; \
	}
menu_defs.c: menus.def
	${_MKTARGET_CREATE}
	[ ! -f menu_defs.h ] || mv -f menu_defs.h menu_defs.oh
	${TOOL_MENUC} menus.def
	! cmp -s menu_defs.oh menu_defs.h || mv -f menu_defs.oh menu_defs.h
	@rm -f menu_defs.oh

msg_defs.h: msg_defs.c
	[ -f ${.TARGET} ] || { \
		${_MKSHTARGET_CREATE}; \
		${TOOL_MSGC} msg.def; \
	}
msg_defs.c: msg.def
	${_MKTARGET_CREATE}
	[ ! -f msg_defs.h ] || mv -f msg_defs.h msg_defs.oh
	${TOOL_MSGC} msg.def
	! cmp -s msg_defs.oh msg_defs.h || mv -f msg_defs.oh msg_defs.h
	@rm -f msg_defs.oh

# Needed to get proper dependency checks on osrelease
msgtouch: ${DISTRIBVERDEP}
	touch ${.TARGET}

msg.def: msg.mi.${SYSINSTLANG} ${MSG_MD} msgtouch
	${_MKTARGET_CREATE}
	${TOOL_SED} "s/@@VERSION@@/${DISTRIBVER}/" ${.ALLSRC} | \
	${TOOL_SED} "s/@@MACHINE@@/${MACHINE}/" | \
	${TOOL_AWK} -f ${UNIF_AWK} -v defines="${MD_OPTIONS}" > ${.TARGET}

menus.def: menus.mi ${MENUS_MD} msgtouch
	${_MKTARGET_CREATE}
	${TOOL_SED} "s/@@VERSION@@/${DISTRIBVER}/" ${.ALLSRC} | \
	${TOOL_SED} "s/@@MACHINE@@/${MACHINE}/" | \
	${TOOL_AWK} -f ${UNIF_AWK} -v defines="${MD_OPTIONS}" > ${.TARGET}

sysinstmsgs.fmtcnt: msg.mi.${SYSINSTLANG} ${MSG_MD} msgtouch msg_defs.h
	${_MKTARGET_CREATE}
	${TOOL_SED} "s/@@VERSION@@/${DISTRIBVER}/" \
		${.ALLSRC:M*.${SYSINSTLANG}} | \
	${TOOL_SED} "s/@@MACHINE@@/${MACHINE}/" | \
	${TOOL_AWK} -f ${UNIF_AWK} -v defines="${MD_OPTIONS}" | \
	${HOST_SH} ${MSG_XLAT_SH} -c > ${.TARGET}

.for LANG in ${LANGUAGES:N${SYSINSTLANG}}
sysinstmsgs.${LANG}: msg.mi.${LANG} ${MSG_MD:S/.${SYSINSTLANG}$/.${LANG}/} msgtouch msg_defs.h sysinstmsgs.fmtcnt
	${_MKTARGET_CREATE}
	${TOOL_SED} "s/@@VERSION@@/${DISTRIBVER}/" ${.ALLSRC:M*.${LANG}} | \
	${TOOL_SED} "s/@@MACHINE@@/${MACHINE}/" | \
	${TOOL_AWK} -f ${UNIF_AWK} -v defines="${MD_OPTIONS}" | \
	${HOST_SH} ${MSG_XLAT_SH} -f sysinstmsgs.fmtcnt > ${.TARGET}

DPSRCS+=	sysinstmsgs.${LANG}
# FILES+=		sysinstmsgs.${LANG}
# FILESDIR_sysinstmsgs.${LANG}=	${CATALOGDIR}
.endfor

.include <bsd.prog.mk>
