/*	$NetBSD$	*/

#ifndef _SSC_H_
#define _SSC_H_

#define SSC_CONSOLE_INIT		20
#define SSC_GETCHAR			21
#define SSC_PUTCHAR			31
#define SSC_OPEN			50
#define SSC_CLOSE			51
#define SSC_READ			52
#define SSC_WRITE			53
#define SSC_GET_COMPLETION		54
#define SSC_WAIT_COMPLETION		55
#define SSC_GET_RTC			65
#define SSC_EXIT			66
#define SSC_LOAD_SYMBOLS		69
#define	SSC_SAL_SET_VECTORS		120

uint64_t ssc(uint64_t, uint64_t, uint64_t, uint64_t, int);

#endif	/* _SSC_H_ */
