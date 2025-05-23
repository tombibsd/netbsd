/*	$NetBSD$	*/

#include "ipf.h"

const char *familyname(int family)
{
	if (family == AF_INET)
		return "inet";
#ifdef AF_INET6
	if (family == AF_INET6)
		return "inet6";
#endif
	return "unknown";
}
