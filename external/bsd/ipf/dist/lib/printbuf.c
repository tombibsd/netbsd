/*	$NetBSD$	*/

/*
 * Copyright (C) 2012 by Darren Reed.
 *
 * See the IPFILTER.LICENCE file for details on licencing.
 *
 * Id: printbuf.c,v 1.1.1.2 2012/07/22 13:44:40 darrenr Exp $
 */

#include <ctype.h>

#include "ipf.h"


void
printbuf(buf, len, zend)
	char *buf;
	int len, zend;
{
	char *s;
	int c;
	int i;

	for (s = buf, i = len; i; i--) {
		c = *s++;
		if (isprint(c))
			putchar(c);
		else
			PRINTF("\\%03o", c);
		if ((c == '\0') && zend)
			break;
	}
}
