/*	$NetBSD$	*/

/*
 * Copyright (C) 2004 Sistina Software, Inc. All rights reserved.  
 * Copyright (C) 2004 Red Hat, Inc. All rights reserved.
 *
 * This file is part of LVM2.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU Lesser General Public License v.2.1.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _LVM_INTL_H
#define _LVM_INTL_H

#ifdef INTL_PACKAGE
#  include <libintl.h>
#  define _(String) dgettext(INTL_PACKAGE, (String))
#else
#  define _(String) (String)
#endif

#endif
