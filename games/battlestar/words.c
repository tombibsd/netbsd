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
 */

#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "@(#)words.c	8.2 (Berkeley) 4/28/95";
#else
__RCSID("$NetBSD$");
#endif
#endif				/* not lint */

#include "extern.h"

struct wlist wlist[] = {
	{ "knife",	KNIFE,		OBJECT, NULL },
	{ "sword",	SWORD,		NOUNS, NULL },
	{ "scabbard",	SWORD,		OBJECT, NULL },
	{ "fine",	SWORD,		OBJECT, NULL },
	{ "two-handed",	TWO_HANDED,	OBJECT, NULL },
	{ "cleaver",	CLEAVER,	OBJECT, NULL },
	{ "broadsword",	BROAD,		OBJECT, NULL },
	{ "mail",	MAIL,		OBJECT, NULL },
	{ "coat",	MAIL,		OBJECT, NULL },
	{ "helmet",	HELM,		OBJECT, NULL },
	{ "shield",	SHIELD,		OBJECT, NULL },
	{ "maid",	MAID,		OBJECT, NULL },
	{ "maid's",	MAID,		OBJECT, NULL },
	{ "body",	BODY,		NOUNS, NULL },
	{ "viper",	VIPER,		OBJECT, NULL },
	{ "lamp",	LAMPON,		OBJECT, NULL },
	{ "lantern",	LAMPON,		OBJECT, NULL },
	{ "shoes",	SHOES,		OBJECT, NULL },
	{ "pajamas",	PAJAMAS,	OBJECT, NULL },
	{ "robe",	ROBE,		OBJECT, NULL },
	{ "amulet",	AMULET,		NOUNS, NULL },
	{ "medallion",	MEDALION,	NOUNS, NULL },
	{ "talisman",	TALISMAN,	NOUNS, NULL },
	{ "woodsman",	DEADWOOD,	OBJECT, NULL },
	{ "woodsman's",	DEADWOOD,	OBJECT, NULL },
	{ "mallet",	MALLET,		OBJECT, NULL },
	{ "laser",	LASER,		OBJECT, NULL },
	{ "pistol",	LASER,		OBJECT, NULL },
	{ "blaster",	LASER,		OBJECT, NULL },
	{ "gun",	LASER,		OBJECT, NULL },
	{ "goddess",	NORMGOD,	NOUNS, NULL },
	{ "grenade",	GRENADE,	OBJECT, NULL },
	{ "chain",	CHAIN,		OBJECT, NULL },
	{ "rope",	ROPE,		OBJECT, NULL },
	{ "levis",	LEVIS,		OBJECT, NULL },
	{ "pants",	LEVIS,		OBJECT, NULL },
	{ "mace",	MACE,		OBJECT, NULL },
	{ "shovel",	SHOVEL,		OBJECT, NULL },
	{ "halberd",	HALBERD,	OBJECT, NULL },
	{ "compass",	COMPASS,	OBJECT, NULL },
	{ "elf",	ELF,		OBJECT, NULL },
	{ "coins",	COINS,		OBJECT, NULL },
	{ "matches",	MATCHES,	OBJECT, NULL },
	{ "match",	MATCHES,	OBJECT, NULL },
	{ "book",	MATCHES,	OBJECT, NULL },
	{ "man",	MAN,		NOUNS, NULL },
	{ "papaya",	PAPAYAS,	OBJECT, NULL },
	{ "papayas",	PAPAYAS,	OBJECT, NULL },
	{ "pineapple",	PINEAPPLE,	OBJECT, NULL },
	{ "kiwi",	KIWI,		OBJECT, NULL },
	{ "coconut",	COCONUTS,	OBJECT, NULL },
	{ "coconuts",	COCONUTS,	OBJECT, NULL },
	{ "mango",	MANGO,		OBJECT, NULL },
	{ "ring",	RING,		OBJECT, NULL },
	{ "potion",	POTION,		OBJECT, NULL },
	{ "bracelet",	BRACELET,	OBJECT, NULL },
	{ "timer",	TIMER,		NOUNS, NULL },
	{ "bomb",	BOMB,		OBJECT, NULL },
	{ "warhead",	BOMB,		OBJECT, NULL },
	{ "girl",	NATIVE,		NOUNS, NULL },
	{ "native",	NATIVE,		NOUNS, NULL },
	{ "horse",	HORSE,		OBJECT, NULL },
	{ "stallion",	HORSE,		OBJECT, NULL },
	{ "car",	CAR,		OBJECT, NULL },
	{ "volare",	CAR,		OBJECT, NULL },
	{ "pot",	POT,		OBJECT, NULL },
	{ "jewels",	POT,		OBJECT, NULL },
	{ "bar",	BAR,		OBJECT, NULL },
	{ "diamond",	BLOCK,		OBJECT, NULL },
	{ "block",	BLOCK,		OBJECT, NULL },
	{ "up",		UP,		VERB, NULL },
	{ "u",		UP,		VERB, NULL },
	{ "down",	DOWN,		VERB, NULL },
	{ "d",		DOWN,		VERB, NULL },
	{ "ahead",	AHEAD,		VERB, NULL },
	{ "a",		AHEAD,		VERB, NULL },
	{ "back",	BACK,		VERB, NULL },
	{ "b",		BACK,		VERB, NULL },
	{ "right",	RIGHT,		VERB, NULL },
	{ "r",		RIGHT,		VERB, NULL },
	{ "left",	LEFT,		VERB, NULL },
	{ "l",		LEFT,		VERB, NULL },
	{ "take",	TAKE,		VERB, NULL },
	{ "get",	TAKE,		VERB, NULL },
	{ "use",	USE,		VERB, NULL },
	{ "look",	LOOK,		VERB, NULL },
	{ "lo",		LOOK,		VERB, NULL },
	{ "quit",	QUIT,		VERB, NULL },
	{ "q",		QUIT,		VERB, NULL },
	{ "su",		SU,		VERB, NULL },
	{ "drop",	DROP,		VERB, NULL },
	{ "draw",	DRAW,		VERB, NULL },
	{ "pull",	DRAW,		VERB, NULL },
	{ "carry",	DRAW,		VERB, NULL },
	{ "wear",	WEARIT,		VERB, NULL },
	{ "sheathe",	WEARIT,		VERB, NULL },
	{ "put",	PUT,		VERB, NULL },
	{ "buckle",	PUT,		VERB, NULL },
	{ "strap",	PUT,		VERB, NULL },
	{ "tie",	PUT,		VERB, NULL },
	{ "inven",	INVEN,		VERB, NULL },
	{ "i",		INVEN,		VERB, NULL },
	{ "everything",	EVERYTHING,	OBJECT, NULL },
	{ "all",	EVERYTHING,	OBJECT, NULL },
	{ "and",	AND,		CONJ, NULL },
	{ ",",		AND,		CONJ, NULL },
	{ "kill",	KILL,		VERB, NULL },
	{ "fight",	KILL,		VERB, NULL },
	{ "ravage",	RAVAGE,		VERB, NULL },
	{ "rape",	RAVAGE,		VERB, NULL },
	{ "undress",	UNDRESS,	VERB, NULL },
	{ "throw",	THROW,		VERB, NULL },
	{ "launch",	LAUNCH,		VERB, NULL },
	{ "land",	LANDIT,		VERB, NULL },
	{ "light",	LIGHT,		VERB, NULL },
	{ "strike",	LIGHT,		VERB, NULL },
	{ "follow",	FOLLOW,		VERB, NULL },
	{ "chase",	FOLLOW,		VERB, NULL },
	{ "kiss",	KISS,		VERB, NULL },
	{ "love",	LOVE,		VERB, NULL },
	{ "fuck",	LOVE,		VERB, NULL },
	{ "give",	GIVE,		VERB, NULL },
	{ "smite",	SMITE,		VERB, NULL },
	{ "attack",	SMITE,		VERB, NULL },
	{ "swing",	SMITE,		VERB, NULL },
	{ "stab",	SMITE,		VERB, NULL },
	{ "slice",	SMITE,		VERB, NULL },
	{ "cut",	SMITE,		VERB, NULL },
	{ "hack",	SMITE,		VERB, NULL },
	{ "shoot",	SHOOT,		VERB, NULL },
	{ "blast",	SHOOT,		VERB, NULL },
	{ "open",	OPEN,		VERB, NULL },
	{ "unlock",	OPEN,		VERB, NULL },
	{ "on",		ON,		PREPS, NULL },
	{ "off",	OFF,		PREPS, NULL },
	{ "time",	TIME,		VERB, NULL },
	{ "sleep",	SLEEP,		VERB, NULL },
	{ "dig",	DIG,		VERB, NULL },
	{ "eat",	EAT,		VERB, NULL },
	{ "swim",	SWIM,		VERB, NULL },
	{ "drink",	DRINK,		VERB, NULL },
	{ "door",	DOOR,		NOUNS, NULL },
	{ "verbose",	VERBOSE,	VERB, NULL },
	{ "brief",	BRIEF,		VERB, NULL },
	{ "save",	SAVE,		VERB, NULL },
	{ "ride",	RIDE,		VERB, NULL },
	{ "mount",	RIDE,		VERB, NULL },
	{ "drive",	DRIVE,		VERB, NULL },
	{ "start",	DRIVE,		VERB, NULL },
	{ "score",	SCORE,		VERB, NULL },
	{ "points",	SCORE,		VERB, NULL },
	{ "bury",	BURY,		VERB, NULL },
	{ "jump",	JUMP,		VERB, NULL },
	{ "kick",	KICK,		VERB, NULL },
	{ "kerosene",	0,		ADJS, NULL },
	{ "plumed",	0,		ADJS, NULL },
	{ "ancient",	0,		ADJS, NULL },
	{ "golden",	0,		ADJS, NULL },
	{ "gold",	0,		ADJS, NULL },
	{ "ostrich",	0,		ADJS, NULL },
	{ "rusty",	0,		ADJS, NULL },
	{ "old",	0,		ADJS, NULL },
	{ "dented",	0,		ADJS, NULL },
	{ "blue",	0,		ADJS, NULL },
	{ "purple",	0,		ADJS, NULL },
	{ "kingly",	0,		ADJS, NULL },
	{ "the",	0,		ADJS, NULL },
	{ "climb",	AUXVERB,	VERB, NULL },
	{ "move",	AUXVERB,	VERB, NULL },
	{ "make",	AUXVERB,	VERB, NULL },
	{ "to",		0,		ADJS, NULL },
	{ NULL,		0,		0, NULL }
};
