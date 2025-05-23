/*	$NetBSD$	*/

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 *
 * $FreeBSD: src/sys/cddl/dev/dtrace/dtrace_modevent.c,v 1.1.4.1 2009/08/03 08:13:06 kensmith Exp $
 *
 */

/* ARGSUSED */
static int
dtrace_modcmd(modcmd_t cmd, void *data)
{
	int bmajor = -1, cmajor = -1;
	int error;

	switch (cmd) {
	case MODULE_CMD_INIT:
		dtrace_load(NULL);
		error = devsw_attach("dtrace", NULL, &bmajor,
		    &dtrace_cdevsw, &cmajor);
		if (error != 0)
			if (dtrace_unload() != 0)
				panic("failed to unload dtrace");
		return error;

	case MODULE_CMD_FINI:
		error = devsw_detach(NULL, &dtrace_cdevsw);
		if (error != 0)
			return error;

		error = dtrace_unload();
		if (error != 0) {
			if (devsw_attach("dtrace", NULL, &bmajor,
					 &dtrace_cdevsw, &cmajor) != 0)
				panic("failed to reattach dtrace_devsw");
		}
		return error;

	case MODULE_CMD_AUTOUNLOAD:
		return EBUSY;
	default:
		return ENOTTY;
	}
}
