/*	$NetBSD$	*/

/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Kenneth Almquist.
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
#if 0
static char sccsid[] = "@(#)redir.c	8.2 (Berkeley) 5/4/95";
#else
__RCSID("$NetBSD$");
#endif
#endif /* not lint */

#include <sys/types.h>
#include <sys/param.h>	/* PIPE_BUF */
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Code for dealing with input/output redirection.
 */

#include "main.h"
#include "shell.h"
#include "nodes.h"
#include "jobs.h"
#include "options.h"
#include "expand.h"
#include "redir.h"
#include "output.h"
#include "memalloc.h"
#include "error.h"


#define EMPTY -2		/* marks an unused slot in redirtab */
#define CLOSED -1		/* fd was not open before redir */
#ifndef PIPE_BUF
# define PIPESIZE 4096		/* amount of buffering in a pipe */
#else
# define PIPESIZE PIPE_BUF
#endif


MKINIT
struct renamelist {
	struct renamelist *next;
	int orig;
	int into;
};

MKINIT
struct redirtab {
	struct redirtab *next;
	struct renamelist *renamed;
};


MKINIT struct redirtab *redirlist;

/*
 * We keep track of whether or not fd0 has been redirected.  This is for
 * background commands, where we want to redirect fd0 to /dev/null only
 * if it hasn't already been redirected.
 */
STATIC int fd0_redirected = 0;

/*
 * And also where to put internal use fds that should be out of the
 * way of user defined fds (normally)
 */
STATIC int big_sh_fd = 0;

STATIC const struct renamelist *is_renamed(const struct renamelist *, int);
STATIC void fd_rename(struct redirtab *, int, int);
STATIC void free_rl(struct redirtab *, int);
STATIC void openredirect(union node *, char[10], int);
STATIC int openhere(const union node *);
STATIC int copyfd(int, int, int);
STATIC void find_big_fd(void);

STATIC const struct renamelist *
is_renamed(const struct renamelist *rl, int fd)
{
	while (rl != NULL) {
		if (rl->orig == fd)
			return rl;
		rl = rl->next;
	}
	return NULL;
}

STATIC void
free_rl(struct redirtab *rt, int reset)
{
	struct renamelist *rl, *rn = rt->renamed;

	while ((rl = rn) != NULL) {
		rn = rl->next;
		if (rl->orig == 0)
			fd0_redirected--;
		if (reset) {
			if (rl->into < 0)
				close(rl->orig);
			else
				movefd(rl->into, rl->orig);
		}
		ckfree(rl);
	}
	rt->renamed = NULL;
}

STATIC void
fd_rename(struct redirtab *rt, int from, int to)
{
	struct renamelist *rl = ckmalloc(sizeof(struct renamelist));

	rl->next = rt->renamed;
	rt->renamed = rl;

	rl->orig = from;
	rl->into = to;
}

/*
 * Process a list of redirection commands.  If the REDIR_PUSH flag is set,
 * old file descriptors are stashed away so that the redirection can be
 * undone by calling popredir.  If the REDIR_BACKQ flag is set, then the
 * standard output, and the standard error if it becomes a duplicate of
 * stdout, is saved in memory.
 */

void
redirect(union node *redir, int flags)
{
	union node *n;
	struct redirtab *sv = NULL;
	int i;
	int fd;
	char memory[10];	/* file descriptors to write to memory */

	for (i = 10 ; --i >= 0 ; )
		memory[i] = 0;
	memory[1] = flags & REDIR_BACKQ;
	if (flags & REDIR_PUSH) {
		/* We don't have to worry about REDIR_VFORK here, as
		 * flags & REDIR_PUSH is never true if REDIR_VFORK is set.
		 */
		sv = ckmalloc(sizeof (struct redirtab));
		sv->renamed = NULL;
		sv->next = redirlist;
		redirlist = sv;
	}
	for (n = redir ; n ; n = n->nfile.next) {
		fd = n->nfile.fd;
		if ((n->nfile.type == NTOFD || n->nfile.type == NFROMFD) &&
		    n->ndup.dupfd == fd) {
			/* redirect from/to same file descriptor */
			/* make sure it stays open */
			if (fcntl(fd, F_SETFD, 0) < 0)
				error("fd %d: %s", fd, strerror(errno));
			continue;
		}

		if ((flags & REDIR_PUSH) && !is_renamed(sv->renamed, fd)) {
			INTOFF;
			if (big_sh_fd < 10)
				find_big_fd();
			if ((i = fcntl(fd, F_DUPFD, big_sh_fd)) == -1) {
				switch (errno) {
				case EBADF:
					i = CLOSED;
					break;
				case EMFILE:
				case EINVAL:
					find_big_fd();
					i = fcntl(fd, F_DUPFD, big_sh_fd);
					if (i >= 0)
						break;
					/* FALLTHRU */
				default:
					i = errno;
					INTON;    /* XXX not needed here ? */
					error("%d: %s", fd, strerror(i));
					/* NOTREACHED */
				}
			}
			if (i >= 0)
				(void)fcntl(i, F_SETFD, FD_CLOEXEC);
			fd_rename(sv, fd, i);
			INTON;
		}
		if (fd == 0)
			fd0_redirected++;
		openredirect(n, memory, flags);
	}
	if (memory[1])
		out1 = &memout;
	if (memory[2])
		out2 = &memout;
}


STATIC void
openredirect(union node *redir, char memory[10], int flags)
{
	struct stat sb;
	int fd = redir->nfile.fd;
	char *fname;
	int f;
	int eflags, cloexec;

	/*
	 * We suppress interrupts so that we won't leave open file
	 * descriptors around.  This may not be such a good idea because
	 * an open of a device or a fifo can block indefinitely.
	 */
	INTOFF;
	if (fd < 10)
		memory[fd] = 0;
	switch (redir->nfile.type) {
	case NFROM:
		fname = redir->nfile.expfname;
		if (flags & REDIR_VFORK)
			eflags = O_NONBLOCK;
		else
			eflags = 0;
		if ((f = open(fname, O_RDONLY|eflags)) < 0)
			goto eopen;
		if (eflags)
			(void)fcntl(f, F_SETFL, fcntl(f, F_GETFL, 0) & ~eflags);
		break;
	case NFROMTO:
		fname = redir->nfile.expfname;
		if ((f = open(fname, O_RDWR|O_CREAT|O_TRUNC, 0666)) < 0)
			goto ecreate;
		break;
	case NTO:
		if (Cflag) {
			fname = redir->nfile.expfname;
			if ((f = open(fname, O_WRONLY)) == -1) {
				if ((f = open(fname, O_WRONLY|O_CREAT|O_EXCL,
				    0666)) < 0)
					goto ecreate;
			} else if (fstat(f, &sb) == -1) {
				int serrno = errno;
				close(f);
				errno = serrno;
				goto ecreate;
			} else if (S_ISREG(sb.st_mode)) {
				close(f);
				errno = EEXIST;
				goto ecreate;
			}
			break;
		}
		/* FALLTHROUGH */
	case NCLOBBER:
		fname = redir->nfile.expfname;
		if ((f = open(fname, O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0)
			goto ecreate;
		break;
	case NAPPEND:
		fname = redir->nfile.expfname;
		if ((f = open(fname, O_WRONLY|O_CREAT|O_APPEND, 0666)) < 0)
			goto ecreate;
		break;
	case NTOFD:
	case NFROMFD:
		if (redir->ndup.dupfd >= 0) {	/* if not ">&-" */
			if (fd < 10 && redir->ndup.dupfd < 10 &&
			    memory[redir->ndup.dupfd])
				memory[fd] = 1;
			else if (copyfd(redir->ndup.dupfd, fd,
			    (flags&(REDIR_PUSH|REDIR_KEEP)) == REDIR_PUSH) < 0)
				error("Redirect (from %d to %d) failed: %s",
				    redir->ndup.dupfd, fd, strerror(errno));
		} else
			(void) close(fd);
		INTON;
		return;
	case NHERE:
	case NXHERE:
		f = openhere(redir);
		break;
	default:
		abort();
	}

	cloexec = fd > 2 && (flags & REDIR_KEEP) == 0;
	if (f != fd) {
		if (copyfd(f, fd, cloexec) < 0) {
			int e = errno;

			close(f);
			error("redirect reassignment (fd %d) failed: %s", fd,
			    strerror(e));
		}
		close(f);
	} else if (cloexec)
		(void)fcntl(f, F_SETFD, FD_CLOEXEC);

	INTON;
	return;
ecreate:
	exerrno = 1;
	error("cannot create %s: %s", fname, errmsg(errno, E_CREAT));
eopen:
	exerrno = 1;
	error("cannot open %s: %s", fname, errmsg(errno, E_OPEN));
}


/*
 * Handle here documents.  Normally we fork off a process to write the
 * data to a pipe.  If the document is short, we can stuff the data in
 * the pipe without forking.
 */

STATIC int
openhere(const union node *redir)
{
	int pip[2];
	int len = 0;

	if (pipe(pip) < 0)
		error("Pipe call failed");
	if (redir->type == NHERE) {
		len = strlen(redir->nhere.doc->narg.text);
		if (len <= PIPESIZE) {
			xwrite(pip[1], redir->nhere.doc->narg.text, len);
			goto out;
		}
	}
	if (forkshell(NULL, NULL, FORK_NOJOB) == 0) {
		close(pip[0]);
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		signal(SIGHUP, SIG_IGN);
#ifdef SIGTSTP
		signal(SIGTSTP, SIG_IGN);
#endif
		signal(SIGPIPE, SIG_DFL);
		if (redir->type == NHERE)
			xwrite(pip[1], redir->nhere.doc->narg.text, len);
		else
			expandhere(redir->nhere.doc, pip[1]);
		_exit(0);
	}
out:
	close(pip[1]);
	return pip[0];
}



/*
 * Undo the effects of the last redirection.
 */

void
popredir(void)
{
	struct redirtab *rp = redirlist;

	INTOFF;
	free_rl(rp, 1);
	redirlist = rp->next;
	ckfree(rp);
	INTON;
}

/*
 * Undo all redirections.  Called on error or interrupt.
 */

#ifdef mkinit

INCLUDE "redir.h"

RESET {
	while (redirlist)
		popredir();
}

SHELLPROC {
	clearredir(0);
}

#endif

/* Return true if fd 0 has already been redirected at least once.  */
int
fd0_redirected_p(void)
{
	return fd0_redirected != 0;
}

/*
 * Discard all saved file descriptors.
 */

void
clearredir(int vforked)
{
	struct redirtab *rp;
	struct renamelist *rl;

	for (rp = redirlist ; rp ; rp = rp->next) {
		if (!vforked)
			free_rl(rp, 0);
		else for (rl = rp->renamed; rl; rl = rl->next)
			if (rl->into >= 0)
				close(rl->into);
	}
}



/*
 * Copy a file descriptor to be == to.
 * cloexec indicates if we want close-on-exec or not.
 * Returns -1 if any error occurs.
 */

STATIC int
copyfd(int from, int to, int cloexec)
{
	int newfd;

	if (cloexec && to > 2)
		newfd = dup3(from, to, O_CLOEXEC);
	else
		newfd = dup2(from, to);

	return newfd;
}

/*
 * rename fd from to be fd to (closing from).
 * close-on-exec is never set on 'to' (unless
 * from==to and it was set on from) - ie: a no-op
 * returns to (or errors() if an error occurs).  
 *
 * This is mostly used for rearranging the
 * results from pipe().
 */
int
movefd(int from, int to)
{
	if (from == to)
		return to;

	(void) close(to);
	if (copyfd(from, to, 0) != to) {
		int e = errno;

		(void) close(from);
		error("Unable to make fd %d: %s", to, strerror(e));
	}
	(void) close(from);

	return to;
}

STATIC void
find_big_fd(void)
{
	int i, fd;

	for (i = (1 << 10); i >= 10; i >>= 1) {
		if ((fd = fcntl(0, F_DUPFD, i - 1)) >= 0) {
			close(fd);
			break;
		}
	}

	fd = (i / 5) * 4;
	if ((i - fd) > 100)
		fd = i - 100;
	else if (fd < 10)
		fd = 10;

	big_sh_fd = fd;
}

/*
 * If possible, move file descriptor fd out of the way
 * of expected user fd values.   Returns the new fd
 * (which may be the input fd if things do not go well.)
 * Always set close-on-exec on the result, and close
 * the input fd unless it is to be our result.
 */
int
to_upper_fd(int fd)
{
	int i;

	if (big_sh_fd < 10)
		find_big_fd();
	do {
		i = fcntl(fd, F_DUPFD_CLOEXEC, big_sh_fd);
		if (i >= 0) {
			if (fd != i)
				close(fd);
			return i;
		}
		if (errno != EMFILE)
			break;
		find_big_fd();
	} while (big_sh_fd > 10);

	/*
	 * If we wanted to move this fd to some random high number
	 * we certainly do not intend to pass it through exec, even
	 * if the reassignment failed.
	 */
	(void)fcntl(fd, F_SETFD, FD_CLOEXEC);
	return fd;
}
