/*	$NetBSD$	*/

/*
 * Default boot parameters
 *  - must match struct landisk_boot_params in bootblock.h
 */

	.long	0			/* flags */
	.long	5			/* timeout in seconds */
	.long   0			/* console device 0 => BIOSCONS */
	.long   9600			/* serial baud rate */
