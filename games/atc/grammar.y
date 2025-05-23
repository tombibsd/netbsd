/*	$NetBSD$	*/

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ed James.
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
 */

/*
 * Copyright (c) 1987 by Ed James, UC Berkeley.  All rights reserved.
 *
 * Copy permission is hereby granted provided that this notice is
 * retained on all partial or complete copies.
 *
 * For more info on this and all of my stuff, mail edjames@berkeley.edu.
 */

%token <ival>	HeightOp
%token <ival>	WidthOp
%token <ival>	UpdateOp
%token <ival>	NewplaneOp
%token <cval>	DirOp
%token <ival>	ConstOp
%token <ival>	LineOp
%token <ival>	AirportOp
%token <ival>	BeaconOp
%token <ival>	ExitOp
%union {
	int	ival;
	char	cval;
}

%{
#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "@(#)grammar.y	8.1 (Berkeley) 5/31/93";
#else
__RCSID("$NetBSD$");
#endif
#endif /* not lint */

#include <stdio.h>

#include "def.h"
#include "struct.h"
#include "extern.h"
#include "tunable.h"

int line = 1;

static int errors = 0;

static int yyerror(const char *);
%}

%%
file:
	bunch_of_defs { if (checkdefs() < 0) return (errors); } bunch_of_lines
		{ 
		if (sp->num_exits + sp->num_airports < 2)
			yyerror("Need at least 2 airports and/or exits.");
		return (errors);
		}
	;

bunch_of_defs:
	def bunch_of_defs
	| def
	;

def:
	udef
	| ndef
	| wdef
	| hdef
	;

udef:
	UpdateOp '=' ConstOp ';'
		{
		if (sp->update_secs != 0)
			return (yyerror("Redefinition of 'update'."));
		else if ($3 < 1)
			return (yyerror("'update' is too small."));
		else
			sp->update_secs = $3;
		}
	;

ndef:
	NewplaneOp '=' ConstOp ';'
		{
		if (sp->newplane_time != 0)
			return (yyerror("Redefinition of 'newplane'."));
		else if ($3 < 1)
			return (yyerror("'newplane' is too small."));
		else
			sp->newplane_time = $3;
		}
	;

hdef:
	HeightOp '=' ConstOp ';'
		{
		if (sp->height != 0)
			return (yyerror("Redefinition of 'height'."));
		else if ($3 < 3)
			return (yyerror("'height' is too small."));
		else
			sp->height = $3; 
		}
	;

wdef:
	WidthOp '=' ConstOp ';'
		{
		if (sp->width != 0)
			return (yyerror("Redefinition of 'width'."));
		else if ($3 < 3)
			return (yyerror("'width' is too small."));
		else
			sp->width = $3; 
		}
	;

bunch_of_lines:
	line bunch_of_lines
		{}
	| line
		{}
	;

line:
	BeaconOp ':' Bpoint_list ';'
		{}
	| ExitOp ':' Epoint_list ';'
		{}
	| LineOp ':' Lline_list ';'
		{}
	| AirportOp ':' Apoint_list ';'
		{}
	;

Bpoint_list:
	Bpoint Bpoint_list
		{}
	| Bpoint
		{}
	;

Bpoint:
	'(' ConstOp ConstOp ')'
		{
		if (sp->num_beacons % REALLOC == 0) {
			if (sp->beacon == NULL)
				sp->beacon = malloc((sp->num_beacons
					+ REALLOC) * sizeof (BEACON));
			else
				sp->beacon = realloc(sp->beacon,
					(sp->num_beacons + REALLOC) * 
					sizeof (BEACON));
			if (sp->beacon == NULL)
				return (yyerror("No memory available."));
		}
		sp->beacon[sp->num_beacons].x = $2;
		sp->beacon[sp->num_beacons].y = $3;
		check_point($2, $3);
		sp->num_beacons++;
		}
	;

Epoint_list:
	Epoint Epoint_list
		{}
	| Epoint
		{}
	;

Epoint:
	'(' ConstOp ConstOp DirOp ')'
		{
		int	dir;

		if (sp->num_exits % REALLOC == 0) {
			if (sp->exit == NULL)
				sp->exit = malloc((sp->num_exits + 
					REALLOC) * sizeof (EXIT));
			else
				sp->exit = realloc(sp->exit,
					(sp->num_exits + REALLOC) * 
					sizeof (EXIT));
			if (sp->exit == NULL)
				return (yyerror("No memory available."));
		}
		dir = dir_no($4);
		sp->exit[sp->num_exits].x = $2;
		sp->exit[sp->num_exits].y = $3;
		sp->exit[sp->num_exits].dir = dir;
		check_edge($2, $3);
		check_edir($2, $3, dir);
		sp->num_exits++;
		}
	;

Apoint_list:
	Apoint Apoint_list
		{}
	| Apoint
		{}
	;

Apoint:
	'(' ConstOp ConstOp DirOp ')'
		{
		int	dir;

		if (sp->num_airports % REALLOC == 0) {
			if (sp->airport == NULL)
				sp->airport = malloc((sp->num_airports
					+ REALLOC) * sizeof(AIRPORT));
			else
				sp->airport = realloc(sp->airport,
					(sp->num_airports + REALLOC) * 
					sizeof(AIRPORT));
			if (sp->airport == NULL)
				return (yyerror("No memory available."));
		}
		dir = dir_no($4);
		sp->airport[sp->num_airports].x = $2;
		sp->airport[sp->num_airports].y = $3;
		sp->airport[sp->num_airports].dir = dir;
		check_point($2, $3);
		sp->num_airports++;
		}
	;

Lline_list:
	Lline Lline_list
		{}
	| Lline
		{}
	;

Lline:
	'[' '(' ConstOp ConstOp ')' '(' ConstOp ConstOp ')' ']'
		{
		if (sp->num_lines % REALLOC == 0) {
			if (sp->line == NULL)
				sp->line = malloc((sp->num_lines + 
					REALLOC) * sizeof (LINE));
			else
				sp->line = realloc(sp->line,
					(sp->num_lines + REALLOC) *
					sizeof (LINE));
			if (sp->line == NULL)
				return (yyerror("No memory available."));
		}
		sp->line[sp->num_lines].p1.x = $3;
		sp->line[sp->num_lines].p1.y = $4;
		sp->line[sp->num_lines].p2.x = $7;
		sp->line[sp->num_lines].p2.y = $8;
		check_line($3, $4, $7, $8);
		sp->num_lines++;
		}
	;
%%

static void
check_edge(int x, int y)
{
	if (!(x == 0) && !(x == sp->width - 1) && 
	    !(y == 0) && !(y == sp->height - 1))
		yyerror("edge value not on edge.");
}

static void
check_point(int x, int y)
{
	if (x < 1 || x >= sp->width - 1)
		yyerror("X value out of range.");
	if (y < 1 || y >= sp->height - 1)
		yyerror("Y value out of range.");
}

static void
check_linepoint(int x, int y)
{
	if (x < 0 || x >= sp->width)
		yyerror("X value out of range.");
	if (y < 0 || y >= sp->height)
		yyerror("Y value out of range.");
}

static void
check_line(int px1, int py1, int px2, int py2)
{
	int	d1, d2;

	check_linepoint(px1, py1);
	check_linepoint(px2, py2);

	d1 = ABS(px2 - px1);
	d2 = ABS(py2 - py1);

	if (!(d1 == d2) && !(d1 == 0) && !(d2 == 0))
		yyerror("Bad line endpoints.");
}

static int
yyerror(const char *s)
{
	fprintf(stderr, "\"%s\": line %d: %s\n", filename, line, s);
	errors++;

	return (errors);
}

static void
check_edir(int x, int y, int dir)
{
	int	bad = 0;

	if (x == sp->width - 1)
		x = 2;
	else if (x != 0)
		x = 1;
	if (y == sp->height - 1)
		y = 2;
	else if (y != 0)
		y = 1;
	
	switch (x * 10 + y) {
	case 00: if (dir != 3) bad++; break;
	case 01: if (dir < 1 || dir > 3) bad++; break;
	case 02: if (dir != 1) bad++; break;
	case 10: if (dir < 3 || dir > 5) bad++; break;
	case 11: break;
	case 12: if (dir > 1 && dir < 7) bad++; break;
	case 20: if (dir != 5) bad++; break;
	case 21: if (dir < 5) bad++; break;
	case 22: if (dir != 7) bad++; break;
	default:
		yyerror("Unknown value in checkdir!  Get help!");
		break;
	}
	if (bad)
		yyerror("Bad direction for entrance at exit.");
}

static int
checkdefs(void)
{
	int	error = 0;

	if (sp->width == 0) {
		yyerror("'width' undefined.");
		error++;
	}
	if (sp->height == 0) {
		yyerror("'height' undefined.");
		error++;
	}
	if (sp->update_secs == 0) {
		yyerror("'update' undefined.");
		error++;
	}
	if (sp->newplane_time == 0) {
		yyerror("'newplane' undefined.");
		error++;
	}
	if (error)
		return (-1);
	else
		return (0);
}
