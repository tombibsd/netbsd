/* ftello.c -- an implementation of ftell() that returns an off_t
   Copyright (C) 2003, Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.  */
#include <sys/cdefs.h>
__RCSID("$NetBSD$");

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <sys/types.h>

off_t
ftello (FILE *stream)
{
    long pos;
    pos = ftell (stream);

    return (off_t) pos;
}
