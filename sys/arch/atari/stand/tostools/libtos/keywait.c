/*	$NetBSD$	*/

/*
 * Copyright (c) 1995 Waldi Ravens.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by Waldi Ravens.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include "libtos.h"

int
key_wait(char *text)
{
	struct termios	term_attr;
	tcflag_t	lfl_orig;
	int		any_key;
	char		c;

	any_key = (text == NULL);
	if (any_key)
		text = "Press any key...";

	fprintf(stderr, text);
	fflush(stderr);

	tcgetattr(STDERR_FILENO, &term_attr);
	lfl_orig = term_attr.c_lflag;
	if (any_key)
		term_attr.c_lflag &= ~ECHO;
	term_attr.c_lflag &= ~ICANON;
	tcsetattr(STDERR_FILENO, TCSAFLUSH, &term_attr);
	read(STDERR_FILENO, &c, 1);
	term_attr.c_lflag = lfl_orig;
	tcsetattr(STDERR_FILENO, TCSAFLUSH, &term_attr);

	fprintf(stderr, (any_key ? "\r" : "\r\n"));
	fflush(stderr);
	return(c);
}
