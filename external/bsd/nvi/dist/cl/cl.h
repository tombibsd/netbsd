/*	$NetBSD$ */
/*-
 * Copyright (c) 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) 1993, 1994, 1995, 1996
 *	Keith Bostic.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 *
 *	Id: cl.h,v 10.32 2001/08/28 11:33:40 skimo Exp  (Berkeley) Date: 2001/08/28 11:33:40 
 */

/* Avoid clash on OSF1 */
#undef DB

#ifdef USE_SLANG_CURSES
#include <slcurses.h>
#else
#ifdef HAVE_NCURSESW_NCURSES_H /* { */
#include <ncursesw/ncurses.h>
#else /* } { */
#ifdef HAVE_NCURSES_H /* { */
#include <ncurses.h>
#else /* } { */
#include <curses.h>
#include <term.h>
#include <termcap.h>
#endif /* } */
#endif
#endif

typedef struct _cl_private {
	char	 ibuf[256];	/* Input keys. */

	size_t	 skip;		/* Remaining keys. */

	CONVWIN cw;		/* Conversion buffer. */

	int	 eof_count;	/* EOF count. */

	struct termios orig;	/* Original terminal values. */
	struct termios ex_enter;/* Terminal values to enter ex. */
	struct termios vi_enter;/* Terminal values to enter vi. */

	SCREEN	*screen;	/* Curses screen. */

	char	*el;		/* Clear to EOL terminal string. */
	char	*cup;		/* Cursor movement terminal string. */
	char	*cuu1;		/* Cursor up terminal string. */
	char	*rmso, *smso;	/* Inverse video terminal strings. */
	char	*smcup, *rmcup;	/* Terminal start/stop strings. */

	char	*oname;		/* Original screen window name. */

	SCR	*focus;		/* Screen that has the "focus". */

	int	 killersig;	/* Killer signal. */
#define	INDX_HUP	0
#define	INDX_INT	1
#define	INDX_TERM	2
#define	INDX_WINCH	3
#define	INDX_MAX	4	/* Original signal information. */
	struct sigaction oact[INDX_MAX];

	enum {			/* Tty group write mode. */
	    TGW_UNKNOWN=0, TGW_SET, TGW_UNSET } tgw;

	enum {			/* Terminal initialization strings. */
	    TE_SENT=0, TI_SENT } ti_te;

#define	CL_IN_EX	0x0001	/* Currently running ex. */
#define	CL_LAYOUT	0x0002	/* Screen layout changed. */
#define	CL_RENAME	0x0004	/* X11 xterm icon/window renamed. */
#define	CL_RENAME_OK	0x0008	/* User wants the windows renamed. */
#define	CL_SCR_EX_INIT	0x0010	/* Ex screen initialized. */
#define	CL_SCR_VI_INIT	0x0020	/* Vi screen initialized. */
#define	CL_SIGHUP	0x0040	/* SIGHUP arrived. */
#define	CL_SIGINT	0x0080	/* SIGINT arrived. */
#define	CL_SIGTERM	0x0100	/* SIGTERM arrived. */
#define	CL_SIGWINCH	0x0200	/* SIGWINCH arrived. */
#define	CL_STDIN_TTY	0x0400	/* Talking to a terminal. */
#define	CL_SETUPTERM	0x0800	/* Terminal initialized. */
#define	CL_CHANGE_TERM	0x1000	/* Terminal changed. */
	u_int32_t flags;
} CL_PRIVATE;

#define	CLP(sp)		((CL_PRIVATE *)((sp)->gp->cl_private))
#define	GCLP(gp)	((CL_PRIVATE *)gp->cl_private)
#define	CLSP(sp)	((WINDOW *)((sp)->cl_private))

/* Return possibilities from the keyboard read routine. */
typedef enum { INP_OK=0, INP_EOF, INP_ERR, INP_INTR, INP_TIMEOUT } input_t;

/* The screen position relative to a specific window. */
/*
#define	RCNO(sp, cno)	(sp)->coff + (cno)
#define	RLNO(sp, lno)	(sp)->roff + (lno)
*/
#define	RCNO(sp, cno)	(cno)
#define	RLNO(sp, lno)	(lno)

/*
 * XXX
 * Some implementations of curses.h don't define these for us.  Used for
 * compatibility only.
 */
#ifndef TRUE
#define	TRUE	1
#endif
#ifndef FALSE
#define	FALSE	0
#endif

#include "cl_extern.h"

#ifdef USE_PERL_SETENV
#include "perl_api_extern.h"
#define cl_setenv(sp,name,val)	perl_setenv(sp,name,val)
#define cl_unsetenv(sp,name)	perl_setenv(sp,name,NULL)
#else
#define cl_setenv(sp,name,val)	setenv(name,val,1)
#define cl_unsetenv(sp,name)	unsetenv(name)
#endif
