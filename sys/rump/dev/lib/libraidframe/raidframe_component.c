/*	$NetBSD$	*/

/*
 * Copyright (c) 2009 Antti Kantee.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include <sys/param.h>
#include <sys/conf.h>
#include <sys/device.h>
#include <sys/stat.h>

#include <rump-sys/kern.h>
#include <rump-sys/dev.h>
#include <rump-sys/vfs.h>

#include "ioconf.h"

RUMP_COMPONENT(RUMP_COMPONENT_DEV)
{
	extern const struct bdevsw raid_bdevsw;
	extern const struct cdevsw raid_cdevsw;
	devmajor_t bmaj, cmaj;
	int error;

	bmaj = cmaj = -1;
	if ((error = devsw_attach("raid", &raid_bdevsw, &bmaj,
	    &raid_cdevsw, &cmaj)) != 0)
		panic("raid devsw attach failed: %d", error);

	/*
	 * Now that we have the major numbers, detach.  It will get
	 * re-attached later during raid's module initialization.
	 */
	devsw_detach(&raid_bdevsw, &raid_cdevsw);

	if ((error = rump_vfs_makedevnodes(S_IFBLK, "/dev/raid0", 'a',
	    bmaj, 0, 7)) != 0)
		panic("cannot create cooked raid dev nodes: %d", error);
	if ((error = rump_vfs_makedevnodes(S_IFCHR, "/dev/rraid0", 'a',
	    cmaj, 0, 7)) != 0)
		panic("cannot create raw raid dev nodes: %d", error);

	rump_pdev_add(raidattach, 4);
}
