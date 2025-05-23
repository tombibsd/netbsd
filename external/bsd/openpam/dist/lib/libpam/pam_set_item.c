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
 * Id: pam_set_item.c 648 2013-03-05 17:54:27Z des 
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <sys/cdefs.h>
__RCSID("$NetBSD$");

#include <sys/param.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <string.h>

#include <security/pam_appl.h>

#include "openpam_impl.h"

/*
 * XSSO 4.2.1
 * XSSO 6 page 60
 *
 * Set authentication information
 */

int
pam_set_item(pam_handle_t *pamh,
	int item_type,
	const void *item)
{
	void **slot, *tmp;
	size_t nsize, osize;

	ENTERI(item_type);
	if (pamh == NULL)
		RETURNC(PAM_SYSTEM_ERR);
	slot = &pamh->item[item_type];
	osize = nsize = 0;
	switch (item_type) {
	case PAM_SERVICE:
		/* set once only, by pam_start() */
		if (*slot != NULL)
			RETURNC(PAM_SYSTEM_ERR);
		/*FALLTHROUGH*/
	case PAM_USER:
	case PAM_AUTHTOK:
	case PAM_OLDAUTHTOK:
	case PAM_TTY:
	case PAM_RHOST:
	case PAM_RUSER:
	case PAM_USER_PROMPT:
	case PAM_AUTHTOK_PROMPT:
	case PAM_OLDAUTHTOK_PROMPT:
	case PAM_HOST:
	case PAM_NUSER:
		if (*slot != NULL)
			osize = strlen(*slot) + 1;
		if (item != NULL)
			nsize = strlen(item) + 1;
		break;
	case PAM_REPOSITORY:
		osize = nsize = sizeof(struct pam_repository);
		break;
	case PAM_CONV:
		osize = nsize = sizeof(struct pam_conv);
		break;
	case PAM_SOCKADDR:
		osize = nsize = sizeof(struct sockaddr_storage);
		break;
	default:
		RETURNC(PAM_SYMBOL_ERR);
	}
	if (*slot != NULL) {
		memset(*slot, 0xd0, osize);
		FREE(*slot);
	}
	if (item != NULL) {
		if ((tmp = malloc(nsize)) == NULL)
			RETURNC(PAM_BUF_ERR);
		memcpy(tmp, item, nsize);
	} else {
		tmp = NULL;
	}
	*slot = tmp;
	RETURNC(PAM_SUCCESS);
}

/*
 * Error codes:
 *
 *	PAM_SYMBOL_ERR
 *	PAM_SYSTEM_ERR
 *	PAM_BUF_ERR
 */

/**
 * The =pam_set_item function sets the item specified by the =item_type
 * argument to a copy of the object pointed to by the =item argument.
 * The item is stored in the PAM context specified by the =pamh argument.
 * See =pam_get_item for a list of recognized item types.
 */
