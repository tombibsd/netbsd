/*	$NetBSD$	*/

/*-
 * Copyright (c)2008 YAMAMOTO Takashi,
 * All rights reserved.
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
 */

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD$");
#endif /* not lint */

#include <sys/types.h>

#include <machine/sysarch.h>

#include <err.h>
#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* arbitrary port to test */
#define	IO_TIMER1	0x40
#define	TIMER_CNTR0	0
#define	PORT	(IO_TIMER1+TIMER_CNTR0)

static uint8_t
inb(uint16_t port)
{
	uint8_t data;

	__asm __volatile("inb %1, %0" : "=a"(data) : "id"(port));
	return data;
}

sigjmp_buf env;
int gotsig;

static void
sighandler(int sig)
{

	siglongjmp(env, sig);
}

static void
try(const char *msg, bool success)
{
	int sig;

	sig = sigsetjmp(env, 1);
	if (sig == 0) {
		inb(PORT);
		if (!success) {
			errx(EXIT_FAILURE, "%s: unexpected success of inb",
			    msg);
		}
		return;
	}
	if (success) {
		errx(EXIT_FAILURE, "%s: got signal %d\n", msg, sig);
	}
}

int
main(int argc, char *argv[])
{
	int ret;

	signal(SIGSEGV, sighandler);

	try("1", false);
	sleep(1);

	ret = i386_iopl(3);
	if (ret == -1) {
		err(EXIT_FAILURE, "iopl 1");
	}

	sleep(1);
	try("2", true);
	sleep(1);

	ret = i386_iopl(0);
	if (ret == -1) {
		err(EXIT_FAILURE, "iopl 2");
	}

	sleep(1);
	try("3", false);
}
