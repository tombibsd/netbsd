/*	$NetBSD$ */
/*-
 * Copyright (c) 1992, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) 1992, 1993, 1994, 1995, 1996
 *	Keith Bostic.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#include "config.h"

#include <sys/cdefs.h>
#if 0
#ifndef lint
static const char copyright[] =
"%Z% Copyright (c) 1992, 1993, 1994\n\
	The Regents of the University of California.  All rights reserved.\n\
%Z% Copyright (c) 1992, 1993, 1994, 1995, 1996\n\
	Keith Bostic.  All rights reserved.\n";
#endif /* not lint */
#else
__RCSID("$NetBSD$");
#endif

#include <sys/cdefs.h>
#if 0
#ifndef lint
static const char sccsid[] = "Id: main.c,v 10.63 2001/11/01 15:24:43 skimo Exp  (Berkeley) Date: 2001/11/01 15:24:43 ";
#endif /* not lint */
#else
__RCSID("$NetBSD$");
#endif

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <bitstring.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "../vi/vi.h"
#include "pathnames.h"

static void	 v_estr __P((const char *, int, const char *));
static int	 v_obsolete __P((char *, char *[]));

/*
 * editor --
 *	Main editor routine.
 *
 * PUBLIC: int editor __P((WIN *, int, char *[]));
 */
int
editor(WIN *wp, int argc, char **argv)
{
	extern int optind;
	extern char *optarg;
	const char *p;
	EVENT ev;
	FREF *frp;
	SCR *sp;
	GS *gp;
	size_t len;
	u_int flags;
	int ch, flagchk, lflag, secure, startup, readonly, rval, silent;
#ifdef GTAGS
	int gtags = 0;
#endif
	char *tag_f, *wsizearg, path[256];
	const CHAR_T *w;
	size_t wlen;

	gp = wp->gp;

	/* Initialize the busy routine, if not defined by the screen. */
	if (gp->scr_busy == NULL)
		gp->scr_busy = vs_busy;
	/* Initialize the message routine, if not defined by the screen. */
	if (wp->scr_msg == NULL)
		wp->scr_msg = vs_msg;

	/* Set initial screen type and mode based on the program name. */
	readonly = 0;
	if (!strcmp(gp->progname, "ex") || !strcmp(gp->progname, "nex"))
		LF_INIT(SC_EX);
	else {
		/* Nview, view are readonly. */
		if (!strcmp(gp->progname, "nview") ||
		    !strcmp(gp->progname, "view"))
			readonly = 1;
		
		/* Vi is the default. */
		LF_INIT(SC_VI);
	}

	/* Convert old-style arguments into new-style ones. */
	if (v_obsolete(gp->progname, argv))
		return (1);

	/* Parse the arguments. */
	flagchk = '\0';
	tag_f = wsizearg = NULL;
	lflag = secure = silent = 0;
	startup = 1;

	/* Set the file snapshot flag. */
	F_SET(gp, G_SNAPSHOT);

	while ((ch = getopt(argc, argv, "c:"
#ifdef DEBUG
	    "D:"
#endif
	    "eF"
#ifdef GTAGS
	    "G"
#endif
	    "lRrSsT:t:vw:")) != EOF)
		switch (ch) {
		case 'c':		/* Run the command. */
			/*
			 * XXX
			 * We should support multiple -c options.
			 */
			if (gp->c_option != NULL) {
				v_estr(gp->progname, 0,
				    "only one -c command may be specified.");
				return (1);
			}
			gp->c_option = optarg;
			break;
#ifdef DEBUG
		case 'D':
			switch (optarg[0]) {
			case 's':
				startup = 0;
				break;
			case 'w':
				attach(gp);
				break;
			default:
				v_estr(gp->progname, 0,
				    "usage: -D requires s or w argument.");
				return (1);
			}
			break;
#endif
		case 'e':		/* Ex mode. */
			LF_CLR(SC_VI);
			LF_SET(SC_EX);
			break;
		case 'F':		/* No snapshot. */
			v_estr(gp->progname, 0, 
			    "-F option no longer supported.");
			break;
		case 'l':		/* Set lisp, showmatch options. */
			lflag = 1;
			break;
#ifdef GTAGS
		case 'G':               /* gtags mode. */
			gtags = 1;
			break;
#endif
		case 'R':		/* Readonly. */
			readonly = 1;
			break;
		case 'r':		/* Recover. */
			if (flagchk == 't') {
				v_estr(gp->progname, 0,
				    "only one of -r and -t may be specified.");
				return (1);
			}
			flagchk = 'r';
			break;
		case 'S':
			secure = 1;
			break;
		case 's':
			silent = 1;
			break;
#ifdef TRACE
		case 'T':		/* Trace. */
			(void)vtrace_init(optarg);
			break;
#endif
		case 't':		/* Tag. */
			if (flagchk == 'r') {
				v_estr(gp->progname, 0,
				    "only one of -r and -t may be specified.");
				return (1);
			}
			if (flagchk == 't') {
				v_estr(gp->progname, 0,
				    "only one tag file may be specified.");
				return (1);
			}
			flagchk = 't';
			tag_f = optarg;
			break;
		case 'v':		/* Vi mode. */
			LF_CLR(SC_EX);
			LF_SET(SC_VI);
			break;
		case 'w':
			wsizearg = optarg;
			break;
		case '?':
		default:
			(void)gp->scr_usage();
			return (1);
		}
	argc -= optind;
	argv += optind;

	/*
	 * -s option is only meaningful to ex.
	 *
	 * If not reading from a terminal, it's like -s was specified.
	 */
	if (silent && !LF_ISSET(SC_EX)) {
		v_estr(gp->progname, 0, "-s option is only applicable to ex.");
		goto err;
	}
	if (LF_ISSET(SC_EX) && F_ISSET(gp, G_SCRIPTED))
		silent = 1;

	/*
	 * Build and initialize the first/current screen.  This is a bit
	 * tricky.  If an error is returned, we may or may not have a
	 * screen structure.  If we have a screen structure, put it on a
	 * display queue so that the error messages get displayed.
	 *
	 * !!!
	 * Everything we do until we go interactive is done in ex mode.
	 */
	if (screen_init(gp, NULL, &sp)) {
		if (sp != NULL) {
			TAILQ_INSERT_HEAD(&wp->scrq, sp, q);
			sp->wp = wp;
		}
		goto err;
	}
	F_SET(sp, SC_EX);
	TAILQ_INSERT_HEAD(&wp->scrq, sp, q);
	sp->wp = wp;

	if (v_key_init(sp))		/* Special key initialization. */
		goto err;

	{ int oargs[5], *oargp = oargs;
	if (lflag) {			/* Command-line options. */
		*oargp++ = O_LISP;
		*oargp++ = O_SHOWMATCH;
	}
	if (readonly)
		*oargp++ = O_READONLY;
#ifdef GTAGS
	if (gtags)
		*oargp++ = O_GTAGSMODE;
#endif
	if (secure)
		*oargp++ = O_SECURE;
	*oargp = -1;			/* Options initialization. */
	if (opts_init(sp, oargs))
		goto err;
	}
	if (wsizearg != NULL) {
		ARGS *av[2], a, b;
		(void)snprintf(path, sizeof(path), "window=%s", wsizearg);
		a.bp = (CHAR_T *)path;
		a.len = strlen(path);
		b.bp = NULL;
		b.len = 0;
		av[0] = &a;
		av[1] = &b;
		(void)opts_set(sp, av, NULL);
	}
	if (silent) {			/* Ex batch mode option values. */
		O_CLR(sp, O_AUTOPRINT);
		O_CLR(sp, O_PROMPT);
		O_CLR(sp, O_VERBOSE);
		O_CLR(sp, O_WARN);
		F_SET(sp, SC_EX_SILENT);
	}

	sp->rows = O_VAL(sp, O_LINES);	/* Make ex formatting work. */
	sp->cols = O_VAL(sp, O_COLUMNS);

	if (!silent && startup) {	/* Read EXINIT, exrc files. */
		if (ex_exrc(sp))
			goto err;
		if (F_ISSET(sp, SC_EXIT | SC_EXIT_FORCE)) {
			if (screen_end(sp))
				goto err;
			goto done;
		}
	}

	/*
	 * List recovery files if -r specified without file arguments.
	 * Note, options must be initialized and startup information
	 * read before doing this.
	 */
	if (flagchk == 'r' && argv[0] == NULL) {
		if (rcv_list(sp))
			goto err;
		if (screen_end(sp))
			goto err;
		goto done;
	}

	/*
	 * !!!
	 * Initialize the default ^D, ^U scrolling value here, after the
	 * user has had every opportunity to set the window option.
	 *
	 * It's historic practice that changing the value of the window
	 * option did not alter the default scrolling value, only giving
	 * a count to ^D/^U did that.
	 */
	sp->defscroll = (O_VAL(sp, O_WINDOW) + 1) / 2;

	/*
	 * If we don't have a command-line option, switch into the right
	 * editor now, so that we position default files correctly, and
	 * so that any tags file file-already-locked messages are in the
	 * vi screen, not the ex screen.
	 *
	 * XXX
	 * If we have a command-line option, the error message can end
	 * up in the wrong place, but I think that the combination is
	 * unlikely.
	 */
	if (gp->c_option == NULL) {
		F_CLR(sp, SC_EX | SC_VI);
		F_SET(sp, LF_ISSET(SC_EX | SC_VI));
	}

	/* Open a tag file if specified. */
	if (tag_f != NULL) {
		CHAR2INT(sp, tag_f, strlen(tag_f) + 1, w, wlen);
		if (ex_tag_first(sp, w))
			goto err;
	}

	/*
	 * Append any remaining arguments as file names.  Files are recovery
	 * files if -r specified.  If the tag option or ex startup commands
	 * loaded a file, then any file arguments are going to come after it.
	 */
	if (*argv != NULL) {
		if (sp->frp != NULL) {
			/* Cheat -- we know we have an extra argv slot. */
			MALLOC_NOMSG(sp,
			    *--argv, char *, strlen(sp->frp->name) + 1);
			if (*argv == NULL) {
				v_estr(gp->progname, errno, NULL);
				goto err;
			}
			(void)strcpy(*argv, sp->frp->name);
		}
		sp->argv = sp->cargv = argv;
		F_SET(sp, SC_ARGNOFREE);
		if (flagchk == 'r')
			F_SET(sp, SC_ARGRECOVER);
	}

	/*
	 * If the ex startup commands and or/the tag option haven't already
	 * created a file, create one.  If no command-line files were given,
	 * use a temporary file.
	 */
	if (sp->frp == NULL) {
		if (sp->argv == NULL) {
			if ((frp = file_add(sp, NULL)) == NULL)
				goto err;
		} else  {
			if ((frp = file_add(sp, sp->argv[0])) == NULL)
				goto err;
			if (F_ISSET(sp, SC_ARGRECOVER))
				F_SET(frp, FR_RECOVER);
		}

		if (file_init(sp, frp, NULL, 0))
			goto err;
		if (EXCMD_RUNNING(wp)) {
			(void)ex_cmd(sp);
			if (F_ISSET(sp, SC_EXIT | SC_EXIT_FORCE)) {
				if (screen_end(sp))
					goto err;
				goto done;
			}
		}
	}

	/*
	 * Check to see if we need to wait for ex.  If SC_SCR_EX is set, ex
	 * was forced to initialize the screen during startup.  We'd like to
	 * wait for a single character from the user, but we can't because
	 * we're not in raw mode.  We can't switch to raw mode because the
	 * vi initialization will switch to xterm's alternate screen, causing
	 * us to lose the messages we're pausing to make sure the user read.
	 * So, wait for a complete line.  
	 */
	if (F_ISSET(sp, SC_SCR_EX)) {
		p = msg_cmsg(sp, CMSG_CONT_R, &len);
		(void)write(STDOUT_FILENO, p, len);
		for (;;) {
			if (v_event_get(sp, &ev, 0, 0))
				goto err;
			if (ev.e_event == E_INTERRUPT ||
			    (ev.e_event == E_CHARACTER &&
			     (ev.e_value == K_CR || ev.e_value == K_NL)))
				break;
			(void)gp->scr_bell(sp);
		}
	}

	/* Switch into the right editor, regardless. */
	F_CLR(sp, SC_EX | SC_VI);
	F_SET(sp, LF_ISSET(SC_EX | SC_VI) | SC_STATUS_CNT);

	/*
	 * Main edit loop.  Vi handles split screens itself, we only return
	 * here when switching editor modes or restarting the screen.
	 */
	while (sp != NULL)
		if (F_ISSET(sp, SC_EX) ? ex(&sp) : vi(&sp))
			goto err;

done:	rval = 0;
	if (0)
err:		rval = 1;

	return (rval);
}

/*
 * v_obsolete --
 *	Convert historic arguments into something getopt(3) will like.
 */
static int
v_obsolete(char *name, char **argv)
{
	size_t len;
	char *p;

	/*
	 * Translate old style arguments into something getopt will like.
	 * Make sure it's not text space memory, because ex modifies the
	 * strings.
	 *	Change "+" into "-c$".
	 *	Change "+<anything else>" into "-c<anything else>".
	 *	Change "-" into "-s"
	 *	The c, T, t and w options take arguments so they can't be
	 *	    special arguments.
	 *
	 * Stop if we find "--" as an argument, the user may want to edit
	 * a file named "+foo".
	 */
	while (*++argv && strcmp(argv[0], "--"))
		if (argv[0][0] == '+') {
			if (argv[0][1] == '\0') {
				MALLOC_NOMSG(NULL, argv[0], char *, 4);
				if (argv[0] == NULL)
					goto nomem;
				(void)strcpy(argv[0], "-c$");
			} else  {
				p = argv[0];
				len = strlen(argv[0]);
				MALLOC_NOMSG(NULL, argv[0], char *, len + 2);
				if (argv[0] == NULL)
					goto nomem;
				argv[0][0] = '-';
				argv[0][1] = 'c';
				(void)strcpy(argv[0] + 2, p + 1);
			}
		} else if (argv[0][0] == '-') {
			if (argv[0][1] == '\0') {
				MALLOC_NOMSG(NULL, argv[0], char *, 3);
				if (argv[0] == NULL) {
nomem:					v_estr(name, errno, NULL);
					return (1);
				}
				(void)strcpy(argv[0], "-s");
			} else
				if ((argv[0][1] == 'c' || argv[0][1] == 'T' ||
				    argv[0][1] == 't' || argv[0][1] == 'w') &&
				    argv[0][2] == '\0')
					++argv;
		}
	return (0);
}

#ifdef DEBUG
static void
attach(GS *gp)
{
	int fd;
	char ch;

	if ((fd = open(_PATH_TTY, O_RDONLY, 0)) < 0) {
		v_estr(gp->progname, errno, _PATH_TTY);
		return;
	}

	(void)printf("process %lu waiting, enter <CR> to continue: ",
	    (u_long)getpid());
	(void)fflush(stdout);

	do {
		if (read(fd, &ch, 1) != 1) {
			(void)close(fd);
			return;
		}
	} while (ch != '\n' && ch != '\r');
	(void)close(fd);
}
#endif

static void
v_estr(const char *name, int eno, const char *msg)
{
	(void)fprintf(stderr, "%s", name);
	if (msg != NULL)
		(void)fprintf(stderr, ": %s", msg);
	if (eno)
		(void)fprintf(stderr, ": %s", strerror(errno));
	(void)fprintf(stderr, "\n");
}
