#	$NetBSD$

.include "../Makefile.inc"

.PATH:	${S}/netsmb

KMOD=	nsmb
SRCS=	iconv.c smb_conn.c smb_crypt.c smb_dev.c smb_iod.c smb_rq.c \
	smb_smb.c smb_subr.c smb_trantcp.c smb_usr.c subr_mchain.c

.include <bsd.kmodule.mk>

