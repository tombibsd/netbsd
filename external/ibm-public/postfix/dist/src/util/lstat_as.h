/*	$NetBSD$	*/

#ifndef _LSTAT_AS_H_INCLUDED_
#define _LSTAT_AS_H_INCLUDED_

/*++
/* NAME
/*	lstat_as 3h
/* SUMMARY
/*	lstat file as user
/* SYNOPSIS
/*	#include <sys/stat.h>
/*	#include <lstat_as.h>
/* DESCRIPTION
/* .nf

 /* External interface. */

extern int lstat_as(const char *, struct stat *, uid_t, gid_t);

/* LICENSE
/* .ad
/* .fi
/*	The Secure Mailer license must be distributed with this software.
/* AUTHOR(S)
/*	Wietse Venema
/*	IBM T.J. Watson Research
/*	P.O. Box 704
/*	Yorktown Heights, NY 10598, USA
/*--*/

#endif
