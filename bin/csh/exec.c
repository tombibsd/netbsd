/* $NetBSD$ */

/*-
 * Copyright (c) 1980, 1991, 1993
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
#if 0
static char sccsid[] = "@(#)exec.c	8.3 (Berkeley) 5/23/95";
#else
__RCSID("$NetBSD$");
#endif
#endif /* not lint */

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "csh.h"
#include "extern.h"

/*
 * System level search and execute of a command.  We look in each directory
 * for the specified command name.  If the name contains a '/' then we
 * execute only the full path name.  If there is no search path then we
 * execute only full path names.
 */
extern char **environ;

/*
 * As we search for the command we note the first non-trivial error
 * message for presentation to the user.  This allows us often
 * to show that a file has the wrong mode/no access when the file
 * is not in the last component of the search path, so we must
 * go on after first detecting the error.
 */
static const char *exerr;	/* Execution error message */
static Char *expath;		/* Path for exerr */

/*
 * Xhash is an array of HSHSIZ bits (HSHSIZ / 8 chars), which are used
 * to hash execs.  If it is allocated (havhash true), then to tell
 * whether ``name'' is (possibly) present in the i'th component
 * of the variable path, you look at the bit in xhash indexed by
 * hash(hashname("name"), i).  This is setup automatically
 * after .login is executed, and recomputed whenever ``path'' is
 * changed.
 * The two part hash function is designed to let texec() call the
 * more expensive hashname() only once and the simple hash() several
 * times (once for each path component checked).
 * Byte size is assumed to be 8.
 */
#define	HSHSIZ 8192	/* 1k bytes */
#define HSHMASK	(HSHSIZ - 1)
#define HSHMUL 243
static unsigned char xhash[HSHSIZ / 8];

#define hash(a, b) (((a) * HSHMUL + (b)) & HSHMASK)
/* these macros eval their arguments multiple times, so be careful */
#define bit(h, b) ((h)[(b) >> 3] & 1 << ((b) & 7))	/* bit test */
#define bis(h, b) ((h)[(b) >> 3] = \
    (unsigned char)((1 << ((b) & 7)) | (h)[(b) >> 3]))/* bit set */
static int hits, misses;

/* Dummy search path for just absolute search when no path */
static Char *justabs[] = {STRNULL, 0};

static void pexerr(void) __dead;
static void texec(Char *, Char **);
static int hashname(Char *);
static int tellmewhat(struct wordent *, Char *);
static int executable(Char *, Char *, int);
static int iscommand(Char *);

void
/*ARGSUSED*/
doexec(Char **v, struct command *t)
{
    struct varent *pathv;
    Char *blk[2], **av, *dp, **pv, *sav;
    int i, hashval, hashval1;
    sigset_t nsigset;
    int slash;

    hashval = 0;
    /*
     * Glob the command name. We will search $path even if this does something,
     * as in sh but not in csh.  One special case: if there is no PATH, then we
     * execute only commands which start with '/'.
     */
    blk[0] = t->t_dcom[0];
    blk[1] = 0;
    gflag = 0, tglob(blk);
    if (gflag) {
	pv = globall(blk);
	if (pv == 0) {
	    setname(vis_str(blk[0]));
	    stderror(ERR_NAME | ERR_NOMATCH);
	}
	gargv = 0;
    }
    else
	pv = saveblk(blk);

    trim(pv);

    exerr = 0;
    expath = Strsave(pv[0]);
    Vexpath = expath;

    pathv = adrof(STRpath);
    if (pathv == 0 && expath[0] != '/') {
	blkfree(pv);
	pexerr();
    }
    slash = any(short2str(expath), '/');

    /*
     * Glob the argument list, if necessary. Otherwise trim off the quote bits.
     */
    gflag = 0;
    av = &t->t_dcom[1];
    tglob(av);
    if (gflag) {
	av = globall(av);
	if (av == 0) {
	    blkfree(pv);
	    setname(vis_str(expath));
	    stderror(ERR_NAME | ERR_NOMATCH);
	}
	gargv = 0;
    }
    else
	av = saveblk(av);

    blkfree(t->t_dcom);
    t->t_dcom = blkspl(pv, av);
    xfree((ptr_t) pv);
    xfree((ptr_t) av);
    av = t->t_dcom;
    trim(av);

    if (*av == NULL || **av == '\0')
	pexerr();

    xechoit(av);		/* Echo command if -x */
    /*
     * Since all internal file descriptors are set to close on exec, we don't
     * need to close them explicitly here.  Just reorient ourselves for error
     * messages.
     */
    SHIN = 0;
    SHOUT = 1;
    SHERR = 2;
    OLDSTD = 0;
    /*
     * We must do this AFTER any possible forking (like `foo` in glob) so that
     * this shell can still do subprocesses.
     */
    sigemptyset(&nsigset);
    (void)sigprocmask(SIG_SETMASK, &nsigset, NULL);
    /*
     * If no path, no words in path, or a / in the filename then restrict the
     * command search.
     */
    if (pathv == 0 || pathv->vec[0] == 0 || slash)
	pv = justabs;
    else
	pv = pathv->vec;
    sav = Strspl(STRslash, *av); 	/* / command name for postpending */
    Vsav = sav;
    if (havhash)
	hashval = hashname(*av);
    i = 0;
    hits++;
    do {
	/*
	 * Try to save time by looking at the hash table for where this command
	 * could be.  If we are doing delayed hashing, then we put the names in
	 * one at a time, as the user enters them.  This is kinda like Korn
	 * Shell's "tracked aliases".
	 */
	if (!slash && pv[0][0] == '/' && havhash) {
	    hashval1 = hash(hashval, i);
	    if (!bit(xhash, hashval1))
		goto cont;
	}
	if (pv[0][0] == 0 || eq(pv[0], STRdot))	/* don't make ./xxx */
	    texec(*av, av);
	else {
	    dp = Strspl(*pv, sav);
	    Vdp = dp;
	    texec(dp, av);
	    Vdp = 0;
	    xfree((ptr_t)dp);
	}
	misses++;
cont:
	pv++;
	i++;
    } while (*pv);
    hits--;
    Vsav = 0;
    xfree((ptr_t)sav);
    pexerr();
    /* NOTREACHED */
}

static void
pexerr(void)
{
    /* Couldn't find the damn thing */
    if (expath) {
	setname(vis_str(expath));
	Vexpath = 0;
	xfree((ptr_t)expath);
	expath = 0;
    }
    else
	setname("");
    if (exerr)
	stderror(ERR_NAME | ERR_STRING, exerr);
    else
	stderror(ERR_NAME | ERR_COMMAND);
    /* NOTREACHED */
}

/*
 * Execute command f, arg list t.
 * Record error message if not found.
 * Also do shell scripts here.
 */
static void
texec(Char *sf, Char **st)
{
    struct varent *v;
    Char *lastsh[2], **vp, *st0, **ost;
    char *f, **t;
    int fd;
    unsigned char c = '\0';

    /* The order for the conversions is significant */
    t = short2blk(st);
    f = short2str(sf);
    Vt = t;
    errno = 0;			/* don't use a previous error */
    (void)execve(f, t, environ);
    Vt = 0;
    blkfree((Char **)t);
    switch (errno) {

    case ENOEXEC:
	/*
	 * From: casper@fwi.uva.nl (Casper H.S. Dik) If we could not execute
	 * it, don't feed it to the shell if it looks like a binary!
	 */
	if ((fd = open(f, O_RDONLY)) != -1) {
	    if (read(fd, (char *)&c, 1) == 1) {
		if (!Isprint(c) && (c != '\n' && c != '\t')) {
		    (void)close(fd);
		    /*
		     * We *know* what ENOEXEC means.
		     */
		    stderror(ERR_ARCH, f, strerror(errno));
		}
	    }
#ifdef _PATH_BSHELL
	    else
		c = '#';
#endif
	    (void)close(fd);
	}
	/*
	 * If there is an alias for shell, then put the words of the alias in
	 * front of the argument list replacing the command name. Note no
	 * interpretation of the words at this point.
	 */
	v = adrof1(STRshell, &aliases);
	if (v == 0) {
	    vp = lastsh;
	    vp[0] = adrof(STRshell) ? value(STRshell) : STR_SHELLPATH;
	    vp[1] = NULL;
#ifdef _PATH_BSHELL
	    if (fd != -1 && c != '#')
		vp[0] = STR_BSHELL;
#endif
	}
	else
	    vp = v->vec;
	st0 = st[0];
	st[0] = sf;
	ost = st;
	st = blkspl(vp, st);	/* Splice up the new arglst */
	ost[0] = st0;
	sf = *st;
	/* The order for the conversions is significant */
	t = short2blk(st);
	f = short2str(sf);
	xfree((ptr_t) st);
	Vt = t;
	(void)execve(f, t, environ);
	Vt = 0;
	blkfree((Char **) t);
	/* FALLTHROUGH */

    case ENOMEM:
	stderror(ERR_SYSTEM, f, strerror(errno));
	/* NOTREACHED */

    case ENOENT:
	break;

    default:
	if (exerr == 0) {
	    exerr = strerror(errno);
	    if (expath)
		xfree((ptr_t) expath);
	    expath = Strsave(sf);
	    Vexpath = expath;
	}
    }
}

/*ARGSUSED*/
void
execash(Char **t, struct command *kp)
{
    jmp_buf osetexit;
    sig_t osigint, osigquit, osigterm;
    int my_reenter, odidfds, oOLDSTD, oSHERR, oSHIN, oSHOUT;
    int saveDIAG, saveIN, saveOUT, saveSTD;

    if (chkstop == 0 && setintr)
	panystop(0);
    /*
     * Hmm, we don't really want to do that now because we might
     * fail, but what is the choice
     */
    rechist();

    osigint  = signal(SIGINT, parintr);
    osigquit = signal(SIGQUIT, parintr);
    osigterm = signal(SIGTERM, parterm);

    odidfds = didfds;
    oSHIN = SHIN;
    oSHOUT = SHOUT;
    oSHERR = SHERR;
    oOLDSTD = OLDSTD;

    saveIN = dcopy(SHIN, -1);
    saveOUT = dcopy(SHOUT, -1);
    saveDIAG = dcopy(SHERR, -1);
    saveSTD = dcopy(OLDSTD, -1);

    lshift(kp->t_dcom, 1);

    getexit(osetexit);

    if ((my_reenter = setexit()) == 0) {
	SHIN = dcopy(0, -1);
	SHOUT = dcopy(1, -1);
	SHERR = dcopy(2, -1);
	didfds = 0;
	doexec(t, kp);
    }

    (void)signal(SIGINT, osigint);
    (void)signal(SIGQUIT, osigquit);
    (void)signal(SIGTERM, osigterm);

    doneinp = 0;
    didfds = odidfds;
    (void)close(SHIN);
    (void)close(SHOUT);
    (void)close(SHERR);
    (void)close(OLDSTD);
    SHIN = dmove(saveIN, oSHIN);
    SHOUT = dmove(saveOUT, oSHOUT);
    SHERR = dmove(saveDIAG, oSHERR);
    OLDSTD = dmove(saveSTD, oOLDSTD);

    resexit(osetexit);
    if (my_reenter)
	stderror(ERR_SILENT);
}

void
xechoit(Char **t)
{
    if (adrof(STRecho)) {
	int odidfds = didfds;
	(void)fflush(csherr);
	odidfds = didfds;
	didfds = 0;
	blkpr(csherr, t);
	(void)fputc('\n', csherr);
	(void)fflush(csherr);
	didfds = odidfds;
    }
}

void
/*ARGSUSED*/
dohash(Char **v, struct command *t)
{
    struct dirent *dp;
    struct varent *pathv;
    DIR *dirp;
    Char **pv;
    size_t cnt;
    int hashval, i;

    i = 0;
    havhash = 1;
    pathv = adrof(STRpath);

    for (cnt = 0; cnt < sizeof xhash; cnt++)
	xhash[cnt] = 0;
    if (pathv == 0)
	return;
    for (pv = pathv->vec; *pv; pv++, i++) {
	if (pv[0][0] != '/')
	    continue;
	dirp = opendir(short2str(*pv));
	if (dirp == NULL)
	    continue;
	while ((dp = readdir(dirp)) != NULL) {
	    if (dp->d_ino == 0)
		continue;
	    if (dp->d_name[0] == '.' &&
		(dp->d_name[1] == '\0' ||
		 (dp->d_name[1] == '.' && dp->d_name[2] == '\0')))
		continue;
	    hashval = hash(hashname(str2short(dp->d_name)), i);
	    bis(xhash, hashval);
	    /* tw_add_comm_name (dp->d_name); */
	}
	(void) closedir(dirp);
    }
}

void
/*ARGSUSED*/
dounhash(Char **v, struct command *t)
{
    havhash = 0;
}

void
/*ARGSUSED*/
hashstat(Char **v, struct command *t)
{
    if (hits + misses)
	(void)fprintf(cshout, "%d hits, %d misses, %d%%\n",
	    hits, misses, 100 * hits / (hits + misses));
}

/*
 * Hash a command name.
 */
static int
hashname(Char *cp)
{
    long h = 0;

    while (*cp)
	h = hash(h, *cp++);
    return ((int) h);
}

static int
iscommand(Char *name)
{
    struct varent *v;
    Char **pv, *sav;
    int hashval, hashval1, i;
    int slash;

    hashval = 0;
    slash = any(short2str(name), '/');
    v = adrof(STRpath);
    
    if (v == 0 || v->vec[0] == 0 || slash)
	pv = justabs;
    else
	pv = v->vec;
    sav = Strspl(STRslash, name);	/* / command name for postpending */
    if (havhash)
	hashval = hashname(name);
    i = 0;
    do {
	if (!slash && pv[0][0] == '/' && havhash) {
	    hashval1 = hash(hashval, i);
	    if (!bit(xhash, hashval1))
		goto cont;
	}
	if (pv[0][0] == 0 || eq(pv[0], STRdot)) {	/* don't make ./xxx */
	    if (executable(NULL, name, 0)) {
		xfree((ptr_t) sav);
		return i + 1;
	    }
	}
	else {
	    if (executable(*pv, sav, 0)) {
		xfree((ptr_t) sav);
		return i + 1;
	    }
	}
cont:
	pv++;
	i++;
    } while (*pv);
    xfree((ptr_t) sav);
    return 0;
}

/* Also by:
 *  Andreas Luik <luik@isaak.isa.de>
 *  I S A  GmbH - Informationssysteme fuer computerintegrierte Automatisierung
 *  Azenberstr. 35
 *  D-7000 Stuttgart 1
 *  West-Germany
 * is the executable() routine below and changes to iscommand().
 * Thanks again!!
 */

/*
 * executable() examines the pathname obtained by concatenating dir and name
 * (dir may be NULL), and returns 1 either if it is executable by us, or
 * if dir_ok is set and the pathname refers to a directory.
 * This is a bit kludgy, but in the name of optimization...
 */
static int
executable(Char *dir, Char *name, int dir_ok)
{
    struct stat stbuf;
    Char path[MAXPATHLEN + 1], *dp, *sp;
    char *strname;

    if (dir && *dir) {
	for (dp = path, sp = dir; *sp; *dp++ = *sp++)
	    if (dp == &path[MAXPATHLEN + 1]) {
		*--dp = '\0';
		break;
	    }
	for (sp = name; *sp; *dp++ = *sp++)
	    if (dp == &path[MAXPATHLEN + 1]) {
		*--dp = '\0';
		break;
	    }
	*dp = '\0';
	strname = short2str(path);
    }
    else
	strname = short2str(name);
    return (stat(strname, &stbuf) != -1 && ((S_ISREG(stbuf.st_mode) &&
        /* save time by not calling access() in the hopeless case */
	(stbuf.st_mode & (S_IXOTH | S_IXGRP | S_IXUSR)) &&
	access(strname, X_OK) == 0) || (dir_ok && S_ISDIR(stbuf.st_mode))));
}

/* The dowhich() is by:
 *  Andreas Luik <luik@isaak.isa.de>
 *  I S A  GmbH - Informationssysteme fuer computerintegrierte Automatisierung
 *  Azenberstr. 35
 *  D-7000 Stuttgart 1
 *  West-Germany
 * Thanks!!
 */
/*ARGSUSED*/
void
dowhich(Char **v, struct command *c)
{
    struct wordent lexw[3];
    struct varent *vp;

    lexw[0].next = &lexw[1];
    lexw[1].next = &lexw[2];
    lexw[2].next = &lexw[0];

    lexw[0].prev = &lexw[2];
    lexw[1].prev = &lexw[0];
    lexw[2].prev = &lexw[1];

    lexw[0].word = STRNULL;
    lexw[2].word = STRret;

    while (*++v) {
	if ((vp = adrof1(*v, &aliases)) != NULL) {
	    (void)fprintf(cshout, "%s: \t aliased to ", vis_str(*v));
	    blkpr(cshout, vp->vec);
	    (void)fputc('\n', cshout);
	    set(STRstatus, Strsave(STR0));
	}
	else {
	    lexw[1].word = *v;
	    set(STRstatus, Strsave(tellmewhat(lexw, NULL) ? STR0 : STR1));
	}
    }
}

static int
tellmewhat(struct wordent *lexp, Char *str)
{
    struct biltins *bptr;
    struct wordent *sp;
    Char *cmd, *s0, *s1, *s2;
    int i;
    int aliased, found;
    Char qc;

    aliased = 0;
    sp = lexp->next;

    if (adrof1(sp->word, &aliases)) {
	alias(lexp);
	sp = lexp->next;
	aliased = 1;
    }

    s0 = sp->word;		/* to get the memory freeing right... */

    /* handle quoted alias hack */
    if ((*(sp->word) & (QUOTE | TRIM)) == QUOTE)
	(sp->word)++;

    /* do quoting, if it hasn't been done */
    s1 = s2 = sp->word;
    while (*s2)
	switch (*s2) {
	case '\'':
	case '"':
	    qc = *s2++;
	    while (*s2 && *s2 != qc)
		*s1++ = (Char)(*s2++ | QUOTE);
	    if (*s2)
		s2++;
	    break;
	case '\\':
	    if (*++s2)
		*s1++ = (Char)(*s2++ | QUOTE);
	    break;
	default:
	    *s1++ = *s2++;
	}
    *s1 = '\0';

    for (bptr = bfunc; bptr < &bfunc[nbfunc]; bptr++) {
	if (eq(sp->word, str2short(bptr->bname))) {
	    if (str == NULL) {
		if (aliased)
		    prlex(cshout, lexp);
		(void)fprintf(cshout, "%s: shell built-in command.\n", 
			       vis_str(sp->word));
	    }
	    else
		(void)Strcpy(str, sp->word);
	    sp->word = s0;	/* we save and then restore this */
	    return 1;
	}
    }

    sp->word = cmd = globone(sp->word, G_IGNORE);

    if ((i = iscommand(sp->word)) != 0) {
	Char **pv;
	struct varent *v;
	int    slash = any(short2str(sp->word), '/');

	v = adrof(STRpath);
	if (v == 0 || v->vec[0] == 0 || slash)
	    pv = justabs;
	else
	    pv = v->vec;

	while (--i)
	    pv++;
	if (pv[0][0] == 0 || eq(pv[0], STRdot)) {
	    if (!slash) {
		sp->word = Strspl(STRdotsl, sp->word);
		prlex(cshout, lexp);
		xfree((ptr_t) sp->word);
	    }
	    else
		prlex(cshout, lexp);
	}
	else {
	    s1 = Strspl(*pv, STRslash);
	    sp->word = Strspl(s1, sp->word);
	    xfree((ptr_t) s1);
	    if (str == NULL)
		prlex(cshout, lexp);
	    else
		(void)Strcpy(str, sp->word);
	    xfree((ptr_t) sp->word);
	}
	found = 1;
    }
    else {
 	if (str == NULL) {
	    if (aliased)
		prlex(cshout, lexp);
	    (void)fprintf(csherr,
			   "%s: Command not found.\n", vis_str(sp->word));
	}
	else
	    (void)Strcpy(str, sp->word);
	found = 0;
    }
    sp->word = s0;		/* we save and then restore this */
    xfree((ptr_t) cmd);
    return found;
}
