/*	$NetBSD$	*/

/* currently only framebuffer uses wired map */
#define MIPS3_NWIRED_ENTRY	2
#define MIPS3_WIRED_SIZE	MIPS3_PG_SIZE_MASK_TO_SIZE(MIPS3_PG_SIZE_16M)

#include <mips/wired_map.h>
