#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../dev/raidframe

LIB=	rumpdev_raidframe
COMMENT=RAIDframe (software RAID)

IOCONF=	RAIDFRAME.ioconf
SRCS=   rf_acctrace.c rf_alloclist.c rf_aselect.c rf_callback.c		\
	rf_chaindecluster.c rf_copyback.c rf_cvscan.c rf_dagdegrd.c	\
	rf_dagdegwr.c rf_dagffrd.c rf_dagffwr.c rf_dagfuncs.c		\
	rf_dagutils.c rf_debugMem.c rf_debugprint.c rf_decluster.c	\
	rf_declusterPQ.c rf_diskqueue.c rf_disks.c rf_driver.c		\
	rf_engine.c rf_evenodd.c rf_evenodd_dagfuncs.c rf_evenodd_dags.c\
	rf_fifo.c rf_interdecluster.c rf_invertq.c rf_layout.c		\
	rf_map.c rf_mcpair.c rf_netbsdkintf.c rf_nwayxor.c rf_options.c	\
	rf_paritylog.c rf_paritylogDiskMgr.c rf_paritylogging.c		\
	rf_parityloggingdags.c rf_parityscan.c rf_pq.c rf_pqdeg.c	\
	rf_pqdegdags.c rf_psstatus.c rf_raid0.c rf_raid1.c rf_raid4.c	\
	rf_raid5.c rf_raid5_rotatedspare.c rf_reconbuffer.c		\
	rf_reconmap.c rf_reconstruct.c rf_reconutil.c rf_revent.c	\
	rf_shutdown.c rf_sstf.c rf_states.c rf_stripelocks.c		\
	rf_strutils.c rf_utils.c rf_paritymap.c

SRCS+=	raidframe_component.c

.include <bsd.init.mk>

.if !empty(RUMP_NBCOMPAT:M50)
SRCS+=	rf_compat50.c
.endif

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
