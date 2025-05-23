%{
/* $NetBSD$ */
/* $OpenBSD: tokenizer.l,v 1.6 2008/08/21 21:00:14 espie Exp $ */
/*
 * Copyright (c) 2004 Marc Espie <espie@cvs.openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#if HAVE_NBTOOL_CONFIG_H
#include "nbtool_config.h"
#endif
#include "parser.h"
__RCSID("$NetBSD$");
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <limits.h>

extern int mimic_gnu;
extern int32_t yylval;
extern int yylex(void);
extern int yywrap(void);

int32_t number(void);
int32_t parse_radix(void);

%}

%option nounput
%option noinput

delim 	[ \t\n]
ws	{delim}+
hex	0[xX][0-9a-fA-F]+
oct	0[0-7]*
dec	[1-9][0-9]*
radix	0[rR][0-9]+:[0-9a-zA-Z]+

%%
{ws}			{/* just skip it */}
{hex}|{oct}|{dec}	{ yylval = number(); return(NUMBER); }
{radix}			{ if (mimic_gnu) {
				yylval = parse_radix(); return(NUMBER);
			  } else {
			  	return(ERROR);
			  }
			}
"<="			{ return(LE); }
">="			{ return(GE); }
"<<"			{ return(LSHIFT); }
">>"			{ return(RSHIFT); }
"=="			{ return(EQ); }
"!="			{ return(NE); }
"&&"			{ return(LAND); }
"||"			{ return(LOR); }
.			{ return yytext[0]; }
%%

int32_t
number(void)
{
	long l;

	errno = 0;
	l = strtol(yytext, NULL, 0);
	if (((l == LONG_MAX || l == LONG_MIN) && errno == ERANGE) ||
	    l > INT32_MAX || l < INT32_MIN) {
		fprintf(stderr, "m4: numeric overflow in expr: %s\n", yytext);
	}
	return l;
}

int32_t
parse_radix(void)
{
	long base;
	char *next;
	long l;

	l = 0;
	base = strtol(yytext+2, &next, 0);
	if (base > 36 || next == NULL) {
		fprintf(stderr, "m4: error in number %s\n", yytext);
	} else {
		next++;
		while (*next != 0) {
			if (*next >= '0' && *next <= '9')
				l = base * l + *next - '0';
			else if (*next >= 'a' && *next <= 'z')
				l = base * l + *next - 'a' + 10;
			else if (*next >= 'A' && *next <= 'Z')
				l = base * l + *next - 'A' + 10;
			next++;
		}
	}
	return l;
}

