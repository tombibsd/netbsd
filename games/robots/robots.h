/*	$NetBSD$	*/

/*
 * Copyright (c) 1980, 1993
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
 *	@(#)robots.h	8.1 (Berkeley) 5/31/93
 */

#include <sys/cdefs.h>

#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * miscellaneous constants
 */

#define Y_FIELDSIZE	23
#define X_FIELDSIZE	60
#define Y_SIZE		24
#define X_SIZE		80
#define MAXLEVELS	4
#define MAXROBOTS	(MAXLEVELS * 10)
#define ROB_SCORE	10
#undef  S_BONUS
#define S_BONUS		(60 * ROB_SCORE)
#define Y_SCORE		21
#define X_SCORE		(X_FIELDSIZE + 9)
#define Y_PROMPT	(Y_FIELDSIZE - 1)
#define X_PROMPT	(X_FIELDSIZE + 2)
#define MAXSCORES	(Y_SIZE - 2)
#define MAXNAME		16
#define MS_NAME		"Ten"

/*
 * characters on screen
 */

#define ROBOT	'+'
#define HEAP	'*'
#define PLAYER	'@'

/*
 * type definitions
 */

typedef struct {
	int y, x;
} COORD;

typedef struct {
	uint32_t s_uid;
	uint32_t s_score;
	uint32_t s_auto;
	uint32_t s_level;
	char s_name[MAXNAME];
} SCORE;

/*
 * global variables
 */

extern bool Dead, Full_clear, Jump, Newscore, Real_time, Running,
	Teleport, Waiting, Was_bonus, Auto_bot;

#ifdef FANCY
extern bool Pattern_roll, Stand_still;
#endif

extern char Cnt_move, Field[Y_FIELDSIZE][X_FIELDSIZE], Run_ch;
extern const char *Next_move, *Move_list;

extern int Count, Level, Num_robots, Num_scrap, Num_scores,
	Start_level, Wait_bonus, Num_games;

extern uint32_t Score;

extern COORD Max, Min, My_pos, Robots[], Scrap[];

extern jmp_buf End_move;

/*
 * functions
 */

void add_score(int);
char automove(void);
void flush_in(void);
void get_move(void);
void init_field(void);
bool jumping(void);
void make_level(void);
void move_robots(int);
void play_level(void);
int query(const char *);
void quit(int) __dead;
void reset_count(void);
COORD *rnd_pos(void);
void score(int);
void show_score(void);
int sign(int);
void telmsg(int);
