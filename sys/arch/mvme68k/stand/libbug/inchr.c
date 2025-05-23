/*	$NetBSD$	*/

/*
 * bug routines -- assumes that the necessary sections of memory
 * are preserved.
 */
#include <sys/types.h>
#include <machine/prom.h>

#include <lib/libsa/stand.h>
#include "libbug.h"

/* returns 0 if no characters ready to read */
int
getchar(void)
{
	int ret;

	MVMEPROM_NOARG();
	MVMEPROM_CALL(MVMEPROM_INCHR);
	MVMEPROM_RETURN_BYTE(ret);
}
