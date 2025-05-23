/*	$NetBSD$	*/

/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)talk.h	8.1 (Berkeley) 6/6/93
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <protocols/talkd.h>
#include <curses.h>
#include <string.h>

extern	int sockt;
extern	int curses_initialized;
extern	int invitation_waiting;

extern	const char *current_state;
extern	int current_line;

typedef struct xwin {
	WINDOW	*x_win;
	int	x_nlines;
	int	x_ncols;
	int	x_line;
	int	x_col;
	char	kill;
	char	cerase;
	char	werase;
} xwin_t;

extern	xwin_t my_win;
extern	xwin_t his_win;
extern	WINDOW *line_win;

void	announce_invite(void);
int	check_local(void);
void	ctl_transact(struct in_addr, CTL_MSG, int, CTL_RESPONSE *);
void	display(xwin_t *, char *, int);
void	disp_msg(int);
void	end_msgs(void);
void	get_addrs(const char *, const char *);
void	get_names(int, char **);
void	init_display(void);
void	invite_remote(void);
int	look_for_invite(CTL_RESPONSE *);
int	max(int, int);
void	message(const char *);
void	open_ctl(void);
void	open_sockt(void);
void	print_addr(struct sockaddr_in);
void	p_error(const char *) __dead;
void	quit(void) __dead;
int	readwin(WINDOW *, int, int);
void	re_invite(int) __dead;
void	send_delete(void);
void	set_edit_chars(void);
void	sig_sent(int) __dead;
void	start_msgs(void);
void	talk(void) __dead;
void	xscroll(xwin_t *, int);
