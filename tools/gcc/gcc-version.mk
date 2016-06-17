#	$NetBSD$

# common location for tools and native build

.if ${HAVE_GCC} == 48
NETBSD_GCC_VERSION=nb3 20151015
.else
NETBSD_GCC_VERSION=nb1 20160606
.endif
