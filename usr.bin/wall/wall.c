/*	$NetBSD$	*/

/*
 * Copyright (c) 1988, 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__COPYRIGHT("@(#) Copyright (c) 1988, 1990, 1993\
 The Regents of the University of California.  All rights reserved.");
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)wall.c	8.2 (Berkeley) 11/16/93";
#endif
__RCSID("$NetBSD$");
#endif /* not lint */

/*
 * This program is not related to David Wall, whose Stanford Ph.D. thesis
 * is entitled "Mechanisms for Broadcast and Selective Broadcast".
 */

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/uio.h>

#include <ctype.h>
#include <err.h>
#include <grp.h>
#include <errno.h>
#include <paths.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <util.h>

#include "utmpentry.h"
#include "term_chk.h"

static void	addgroup(char *);
static void	makemsg(struct iovec *, const char *, int);
__dead static void	usage(void);

static struct wallgroup {
	gid_t	gid;
	char	*name;
	char	**mem;
	struct wallgroup *next;
} *grouplist;

int
main(int argc, char **argv)
{
	int ch;
	struct iovec iov;
	char *p, **mem;
	struct utmpentry *ep;
	gid_t egid;
	struct wallgroup *wg;
	struct passwd *pw;
	int nobanner;

	setprogname(argv[0]);
	egid = getegid();
	if (setegid(getgid()) == -1)
		err(EXIT_FAILURE, "setegid");
	pw = getpwnam("nobody");
	if (pw == NULL)
		errx(EXIT_FAILURE, "Can't find passwd entry for `nobody'");

	(void)check_sender(NULL, getuid(), egid);

	nobanner = 0;
	while ((ch = getopt(argc, argv, "g:n")) != -1)
		switch (ch) {
		case 'n':
			/* undoc option for shutdown: suppress banner */
			if (geteuid() == 0 || (pw && getuid() == pw->pw_uid))
				nobanner = 1;
			break;
		case 'g':
			addgroup(optarg);
			break;
		case '?':
		default:
			usage();
		}
	argc -= optind;
	argv += optind;
	if (argc > 1)
		usage();

	makemsg(&iov, *argv, nobanner);

	(void)getutentries(NULL, &ep);
	(void)setegid(egid);
	for (; ep; ep = ep->next) {
		if (grouplist) {
			int ingroup;

			ingroup = 0;
			pw = getpwnam(ep->name);
			if (!pw)
				continue;
			for (wg = grouplist; wg && !ingroup; wg = wg->next) {
				if (wg->gid == pw->pw_gid)
					ingroup = 1;
				for (mem = wg->mem; *mem && !ingroup; mem++)
					if (strcmp(ep->name, *mem) == 0)
						ingroup = 1;
			}
			if (ingroup == 0)
				continue;
		}

		/* skip [xgk]dm/xserver entries (":0", ":1", etc.) */
		if (ep->line[0] == ':' && isdigit((unsigned char)ep->line[1]))
			continue;

		if ((p = ttymsg(&iov, 1, ep->line, 60*5)) != NULL)
			warnx("%s", p);
	}
	return EXIT_SUCCESS;
}

static void
addgroup(char *name)
{
	size_t i;
	struct group *grp;
	struct wallgroup *g;

	grp = getgrnam(name);
	if ((grp = getgrnam(name)) == NULL)
		errx(EXIT_FAILURE, "unknown group `%s'", name);
	for (i = 0; grp->gr_mem[i]; i++)
		continue;

	g = malloc(sizeof(*g));
	if (g == NULL)
		err(EXIT_FAILURE, "malloc");
	g->gid = grp->gr_gid;
	g->name = name;
	g->mem = calloc(i + 1, sizeof(*g->mem));
	if (g->mem == NULL)
		err(EXIT_FAILURE, "calloc");
	for (i = 0; grp->gr_mem[i] != NULL; i++) {
		g->mem[i] = strdup(grp->gr_mem[i]);
		if (g->mem[i] == NULL)
			err(EXIT_FAILURE, "strdup");
	}
	g->mem[i] = NULL;
	g->next = grouplist;
	grouplist = g;
}

static void
makebanner(FILE *fp)
{
	const char *whom, *tty;
	char hostname[MAXHOSTNAMELEN + 1], lbuf[100];
	time_t now;
	struct tm *lt;
	struct passwd *pw;

	if (!(whom = getlogin()))
		whom = (pw = getpwuid(getuid())) ? pw->pw_name : "???";
	(void)gethostname(hostname, sizeof(hostname));
	hostname[sizeof(hostname) - 1] = '\0';
	(void)time(&now);
	lt = localtime(&now);

	/*
	 * all this stuff is to blank out a square for the message;
	 * we wrap message lines at column 79, not 80, because some
	 * terminals wrap after 79, some do not, and we can't tell.
	 * Which means that we may leave a non-blank character
	 * in column 80, but that can't be helped.
	 */
	(void)fprintf(fp, "\r%79s\r\n", " ");
	(void)snprintf(lbuf, sizeof lbuf,
	    "Broadcast Message from %s@%s", whom, hostname);
	(void)fprintf(fp, "%-79.79s\007\007\r\n", lbuf);
	tty = ttyname(STDERR_FILENO);
	if (tty == NULL)
		tty = "??";
	(void)snprintf(lbuf, sizeof lbuf, "        (%s) at %d:%02d %s...", tty,
	    lt->tm_hour, lt->tm_min, lt->tm_zone);
	(void)fprintf(fp, "%-79.79s\r\n", lbuf);
}

static void
makemsg(struct iovec *iov, const char *fname, int nobanner)
{
	int ch, cnt;
	struct stat sbuf;
	FILE *fp;
	int fd;
	char *p, tmpname[MAXPATHLEN], lbuf[100];
	size_t mbufsize;
	char *mbuf;

	(void)snprintf(tmpname, sizeof tmpname, "%s/wall.XXXXXX", _PATH_TMP);
	if ((fd = mkstemp(tmpname)) == -1)
		err(EXIT_FAILURE, "can't open temporary file");
	(void)unlink(tmpname);
	if (!(fp = fdopen(fd, "r+")))
		err(EXIT_FAILURE, "can't open temporary file");

	if (!nobanner)
		makebanner(fp);

	(void)fprintf(fp, "%79s\r\n", " ");

	if (fname && !(freopen(fname, "r", stdin)))
		err(EXIT_FAILURE, "can't read %s", fname);

	while (fgets(lbuf, sizeof(lbuf), stdin))
		for (cnt = 0, p = lbuf; (ch = *p) != '\0'; ++p, ++cnt) {
			if (cnt == 79 || ch == '\n') {
				for (; cnt < 79; ++cnt)
					putc(' ', fp);
				putc('\r', fp);
				putc('\n', fp);
				cnt = -1;
			}
			if (ch != '\n')
				putc(ch, fp);
		}
	(void)fprintf(fp, "%79s\r\n", " ");
	rewind(fp);

	if (fstat(fd, &sbuf) == -1)
		err(EXIT_FAILURE, "can't stat temporary file");
	if ((uint64_t)sbuf.st_size > SIZE_T_MAX)
		errx(EXIT_FAILURE, "file too big");
	mbufsize = (size_t)sbuf.st_size;
	if (!(mbuf = malloc(mbufsize)))
		err(EXIT_FAILURE, "malloc");
	if (fread(mbuf, 1, mbufsize, fp) != mbufsize)
		err(EXIT_FAILURE, "can't read temporary file");
	(void)fclose(fp);
	iov->iov_base = mbuf;
	iov->iov_len = mbufsize;
}

static void
usage(void)
{

	(void)fprintf(stderr, "Usage: %s [-g group] [file]\n", getprogname());
	exit(EXIT_FAILURE);
}
