/* Copyright (C) 1992,1995-1999,2000-2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */
#include <sys/cdefs.h>
__RCSID("$NetBSD$");


#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <errno.h>
#if !_LIBC
# if !defined errno && !defined HAVE_ERRNO_DECL
extern int errno;
# endif
# define __set_errno(ev) ((errno) = (ev))
#endif

#include <stdlib.h>
#include <string.h>
#if _LIBC || HAVE_UNISTD_H
# include <unistd.h>
#endif

#if !_LIBC
# define __environ	environ
# ifndef HAVE_ENVIRON_DECL
extern char **environ;
# endif
#endif

#if _LIBC
/* This lock protects against simultaneous modifications of `environ'.  */
# include <bits/libc-lock.h>
__libc_lock_define_initialized (static, envlock)
# define LOCK	__libc_lock_lock (envlock)
# define UNLOCK	__libc_lock_unlock (envlock)
#else
# define LOCK
# define UNLOCK
#endif

/* In the GNU C library we must keep the namespace clean.  */
#ifdef _LIBC
# define unsetenv __unsetenv
#endif


int
unsetenv (const char *name)
{
  size_t len;
  char **ep;

  if (name == NULL || *name == '\0' || strchr (name, '=') != NULL)
    {
      __set_errno (EINVAL);
      return -1;
    }

  len = strlen (name);

  LOCK;

  ep = __environ;
  while (*ep != NULL)
    if (!strncmp (*ep, name, len) && (*ep)[len] == '=')
      {
	/* Found it.  Remove this pointer by moving later ones back.  */
	char **dp = ep;

	do
	  dp[0] = dp[1];
	while (*dp++);
	/* Continue the loop in case NAME appears again.  */
      }
    else
      ++ep;

  UNLOCK;

  return 0;
}

#ifdef _LIBC
# undef unsetenv
weak_alias (__unsetenv, unsetenv)
#endif
