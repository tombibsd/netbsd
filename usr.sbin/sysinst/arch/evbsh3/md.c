/*	$NetBSD$	*/

/*
 * Copyright 1997 Piermont Information Systems Inc.
 * All rights reserved.
 *
 * Based on code written by Philip A. Nelson for Piermont Information
 * Systems Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of Piermont Information Systems Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PIERMONT INFORMATION SYSTEMS INC. ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PIERMONT INFORMATION SYSTEMS INC. BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <sys/types.h>
#include <sys/disklabel.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <fcntl.h>
#include <util.h>

#include "defs.h"
#include "md.h"
#include "msg_defs.h"
#include "menu_defs.h"

int
md_get_info(void)
{
	struct disklabel disklabel;
	int fd;
	char dev_name[100];

	snprintf(dev_name, 100, "/dev/r%s%c", pm->diskdev, 'a' + getrawpartition());

	fd = open(dev_name, O_RDONLY, 0);
	if (fd < 0) {
		endwin();
		fprintf (stderr, "Can't open %s\n", dev_name);
		exit(1);
	}
	if (ioctl(fd, DIOCGDINFO, &disklabel) == -1) {
		endwin();
		fprintf (stderr, "Can't read disklabel on %s.\n", dev_name);
		close(fd);
		exit(1);
	}
	close(fd);

	pm->dlcyl = disklabel.d_ncylinders;
	pm->dlhead = disklabel.d_ntracks;
	pm->dlsec = disklabel.d_nsectors;
	pm->sectorsize = disklabel.d_secsize;
	pm->dlcylsize = disklabel.d_secpercyl;

	/*
	 * Compute whole disk size. Take max of (pm->dlcyl*pm->dlhead*pm->dlsec)
	 * and secperunit,  just in case the disk is already labelled.
	 * (If our new label's RAW_PART size ends up smaller than the
	 * in-core RAW_PART size  value, updating the label will fail.)
	 */
	pm->dlsize = pm->dlcyl * pm->dlhead * pm->dlsec;
	if (disklabel.d_secperunit > pm->dlsize)
		pm->dlsize = disklabel.d_secperunit;

	return 1;
}

/*
 * hook called before writing new disklabel.
 */
int
md_pre_disklabel(void)
{

	return 0;
}

/*
 * hook called after writing disklabel to new target disk.
 */
int
md_post_disklabel(void)
{
	return 0;
}

/*
 * MD hook called after upgrade() or install() has finished setting
 * up the target disk but immediately before the user is given the
 * ``disks are now set up'' message, so that if power fails, they can
 * continue installation by booting the target disk and doing an
 * `upgrade'.
 */
int
md_post_newfs(void)
{

	return 0;
}

int
md_make_bsd_partitions(void)
{

	return make_bsd_partitions();
}

/*
 * any additional partition validataion
 */
int
md_check_partitions(void)
{

	return 1;
}

/* Upgrade support */
int
md_update(void)
{

	endwin();
	md_post_newfs();
	wrefresh(curscr);
	wmove(stdscr, 0, 0);
	wclear(stdscr);
	wrefresh(stdscr);
	return 1;
}

void
md_cleanup_install(void)
{

	enable_rc_conf();
}

int
md_pre_update(void)
{

	return 1;
}

void
md_init(void)
{
}

void
md_init_set_status(int flags)
{

	(void)flags;
}

int
md_post_extract(void)
{
	return 0;
}

int
md_pre_mount()
{
	return 0;
}
