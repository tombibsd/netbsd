/* $Id$ (Berkeley) $Date$ */

/*
 * Declare the basic types, if they aren't already declared.  Named and
 * some system's db.h files protect them with __BIT_TYPES_DEFINED__.
 */
#ifndef __BIT_TYPES_DEFINED__
#define __BIT_TYPES_DEFINED__





#endif






/*
 * XXX
 * Handle function prototypes.  This steps on name space that vi doesn't
 * control, but all of the other solutions are worse.
 */
#undef	__P
#if defined(__STDC__) || defined(__cplusplus)
#define	__P(protos)	protos		/* ANSI C prototypes */
#else
#define	__P(protos)	()		/* K&R C preprocessor */
#endif

/*
 * XXX
 * Some versions of System V changed the number of arguments to gettimeofday
 * without changing the name.
 */
#ifdef HAVE_BROKEN_GETTIMEOFDAY
#define	gettimeofday(tv, tz)	gettimeofday(tv)
#endif

/* 
 * XXX
 * If we don't have mmap, we fake it with read and write, but we'll
 * still need the header information.
 */
#ifndef HAVE_SYS_MMAN_H
#define	MAP_SHARED	1		/* share changes */
#define	MAP_PRIVATE	2		/* changes are private */
#define	PROT_READ	0x1		/* pages can be read */
#define	PROT_WRITE	0x2		/* pages can be written */
#define	PROT_EXEC	0x4		/* pages can be executed */
#endif

/*
 * XXX
 * POSIX 1003.1 names for file descriptors.
 */
#ifndef STDERR_FILENO
#define STDIN_FILENO	0		/* ANSI C #defines */
#define STDOUT_FILENO	1
#define STDERR_FILENO	2
#endif

/*
 * XXX
 * POSIX 1003.1 names for seek settings.
 */
#ifndef SEEK_END
#define	SEEK_SET	0		/* POSIX 1003.1 seek values */
#define	SEEK_CUR	1
#define	SEEK_END	2
#endif

/*
 * Hack _POSIX_VDISABLE to \377 since Ultrix doesn't honor _POSIX_VDISABLE
 * (treats it as ^@).  The symptom is that the ^@ keystroke immediately
 * drops core.
 */
#ifdef HAVE_BROKEN_VDISABLE
#undef	_POSIX_VDISABLE
#define	_POSIX_VDISABLE	((unsigned char)'\377')
#endif

/*
 * XXX
 * POSIX 1003.1 tty disabling character.
 */
#ifndef _POSIX_VDISABLE
#define	_POSIX_VDISABLE	0		/* Some systems used 0. */
#endif

/*
 * XXX
 * 4.4BSD extension to only set the software termios bits.
 */
#ifndef	TCSASOFT			/* 4.4BSD extension. */
#define	TCSASOFT	0
#endif

/*
 * XXX
 * POSIX 1003.1 maximum path length.
 */
#ifndef MAXPATHLEN
#ifdef PATH_MAX
#define	MAXPATHLEN	PATH_MAX
#else
#define	MAXPATHLEN	1024
#endif
#endif

/*
 * XXX
 * MIN, MAX, historically in <sys/param.h>
 */
#ifndef	MAX
#define	MAX(_a,_b)	((_a)<(_b)?(_b):(_a))
#endif
#ifndef	MIN
#define	MIN(_a,_b)	((_a)<(_b)?(_a):(_b))
#endif

/*
 * XXX
 * "DB" isn't always portable, and we want the private information.
 */
#define DB      L__DB
#undef	pgno_t			/* IRIX has its own version. */
#define	pgno_t	L__db_pgno_t

/*
 * XXX
 * 4.4BSD extension to provide lock values in the open(2) call.
 */
#ifndef O_EXLOCK
#define	O_EXLOCK	0
#endif

#ifndef O_SHLOCK
#define	O_SHLOCK	0
#endif

/*
 * XXX
 * POSIX 1003.1 bad file format errno.
 */
#ifndef EFTYPE
#define	EFTYPE		EINVAL
#endif

/*
 * XXX
 * POSIX 1003.2 RE length limit.
 */
#ifndef	_POSIX2_RE_DUP_MAX
#define	_POSIX2_RE_DUP_MAX	255
#endif

/*
 * XXX
 * 4.4BSD extension to determine if a program dropped core from the exit
 * status.
 */
#ifndef	WCOREDUMP
#define	WCOREDUMP(a)	0
#endif

/*
 * XXX
 * Endian-ness of the machine.
 */
#if !defined(LITTLE_ENDIAN)
#define	LITTLE_ENDIAN	1234
#endif
#if !defined(BIG_ENDIAN)
#define	BIG_ENDIAN	4321
#endif
#if !defined(BYTE_ORDER)
#if WORDS_BIGENDIAN == 1
#define	BYTE_ORDER	BIG_ENDIAN
#else
#define	BYTE_ORDER	LITTLE_ENDIAN
#endif
#endif

#ifndef HAVE_MEMCPY
#define memcpy memmove
#endif

#ifdef NEED_FPRINTF_PROTO
extern  int     fprintf( FILE *, const char *, ... );
#endif

#ifdef HAVE_PTHREAD
#define VI_DB_THREAD DB_THREAD
#else
#define VI_DB_THREAD 0
#endif
