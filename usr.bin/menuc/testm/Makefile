#	$NetBSD$

NOMAN=	# defined

.include <bsd.own.mk>

MENUCDIR!=	cd $(.CURDIR)/..; ${PRINTOBJDIR}
MENUC=	${MENUCDIR}/menuc
${MENUC}:
	cd ${.CURDIR}/..; ${MAKE}

PROG=	testm
SRCS=	msg_defs.c menu_defs.c main.c
CFLAGS+=-g
CPPFLAGS+=-I.
LDADD+=	-lcurses -lterminfo
DPADD+=	${LIBCURSES}

CLEANFILES+= menu_defs.c menu_defs.h msg_defs.c msg_defs.h

menu_defs.c menu_defs.h: menus.mc ${MENUC} ${.CURDIR}/../menu_sys.def
	MENUDEF=${.CURDIR}/.. ${MENUC} ${.CURDIR}/menus.mc

msg_defs.c msg_defs.h: menus.msg
	msgc ${.CURDIR}/menus.msg

.include <bsd.prog.mk>
