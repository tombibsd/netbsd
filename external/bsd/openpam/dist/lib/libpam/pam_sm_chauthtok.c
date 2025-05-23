/*	$NetBSD$	*/

/*-
 * Copyright (c) 2002-2003 Networks Associates Technology, Inc.
 * Copyright (c) 2004-2011 Dag-Erling Smørgrav
 * All rights reserved.
 *
 * This software was developed for the FreeBSD Project by ThinkSec AS and
 * Network Associates Laboratories, the Security Research Division of
 * Network Associates, Inc.  under DARPA/SPAWAR contract N66001-01-C-8035
 * ("CBOSS"), as part of the DARPA CHATS research program.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Id: pam_sm_chauthtok.c 648 2013-03-05 17:54:27Z des 
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <sys/cdefs.h>
__RCSID("$NetBSD$");

#include <sys/param.h>

#include <security/pam_appl.h>
#include <security/pam_modules.h>

/*
 * XSSO 4.2.2
 * XSSO 6 page 72
 *
 * Service module implementation for pam_chauthtok
 */

int
pam_sm_chauthtok(pam_handle_t *pamh,
	int flags,
	int argc,
	const char **argv)
{

	ENTER();
	RETURNC(PAM_SYSTEM_ERR);
}

/*
 * Error codes:
 *
 *	PAM_SERVICE_ERR
 *	PAM_SYSTEM_ERR
 *	PAM_BUF_ERR
 *	PAM_CONV_ERR
 *	PAM_PERM_DENIED
 *	PAM_IGNORE
 *	PAM_ABORT
 *
 *	PAM_PERM_DENIED
 *	PAM_AUTHTOK_ERR
 *	PAM_AUTHTOK_RECOVERY_ERR
 *	PAM_AUTHTOK_LOCK_BUSY
 *	PAM_AUTHTOK_DISABLE_AGING
 *	PAM_TRY_AGAIN
 */

/**
 * The =pam_sm_chauthtok function is the service module's implementation
 * of the =pam_chauthtok API function.
 *
 * When the application calls =pam_chauthtok, the service function is
 * called twice, first with the =PAM_PRELIM_CHECK flag set and then again
 * with the =PAM_UPDATE_AUTHTOK flag set.
 */
