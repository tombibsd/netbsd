#---------------------------------------------------------------------------
#
#	i4b - Tarifs France Telecom
#	---------------------------
#
#	$Id$
#
# $NetBSD$
#
#	last edit-date: [Mon Dec 13 21:42:23 1999]
#
#	-hm	got from Aurelien Bargy <dj@dial.oleane.com>
#
#---------------------------------------------------------------------------
#
# a jour au 1.10.97
# Aurelien Bargy - 18.03.97
#
#	I wrote a rates  file for France.   It is rather approximative
# because  some time ago France Telecom  decided to abandon the charging
# units system. The problem  is, on ISDN  lines charging units are still
# used for the  aocd system, even as  it gives a completely  false price
# for a communication, so  "abandoning" the msg_charging_unit system for
# French users of i4b is not a good idea :))
#
#---------------------------------------------------------------------------
#
#	Syntax:
#	-------
#	rate-code: ra0 = local ; ra1 = zone 1 ; ra2 = zone 2...
#       plus quelques tarifs internationaux
# 	Day-No:    0-6, where 0 = Sunday, 1 = Monday, etc.
#	Rate:      start_hour.minutes-end_hour.minutes:period in seconds
#
#------------------------------------------------------------------------
#
#========================== Communications locales ======================
#rate-code Day rate1           rate2           rate3
#--------- --- --------------- --------------- ---------------
ra0        0   00.00-24.00:317
ra0        1   00.00-08.00:317 08.00-19.00:158 19.00-24.00:317
ra0        2   00.00-08.00:317 08.00-19.00:158 19.00-24.00:317
ra0        3   00.00-08.00:317 08.00-19.00:158 19.00-24.00:317
ra0        4   00.00-08.00:317 08.00-19.00:158 19.00-24.00:317
ra0        5   00.00-08.00:317 08.00-19.00:158 19.00-24.00:317
ra0        6   00.00-08.00:317 08.00-12.00:158 12.00-24.00:317
#
#
#
#================== Zone 1 : < 25 km ==================================
#
#rate-code Day rate1           rate2           rate3          
#--------- --- --------------- --------------- ---------------
ra1        0   00.00-24.00:222
ra1        1   00.00-08.00:222 08.00-19.00:111 19.00-24.00:222
ra1        2   00.00-08.00:222 08.00-19.00:111 19.00-24.00:222
ra1        3   00.00-08.00:222 08.00-19.00:111 19.00-24.00:222
ra1        4   00.00-08.00:222 08.00-19.00:111 19.00-24.00:222
ra1        5   00.00-08.00:222 08.00-19.00:111 19.00-24.00:222
ra1        6   00.00-08.00:222 08.00-12.00:111 12.00-24.00:222
#
#
#
#================== Zone 2 : 25-30 km =================================
#
#rate-code Day rate1           rate2           rate3          
#--------- --- --------------- --------------- -------------- 
ra2        0   00.00-24.00:148
ra2        1   00.00-08.00:148 08.00-19.00:74 19.00-24.00:148
ra2        2   00.00-08.00:148 08.00-19.00:74 19.00-24.00:148
ra2        3   00.00-08.00:148 08.00-19.00:74 19.00-24.00:148
ra2        4   00.00-08.00:148 08.00-19.00:74 19.00-24.00:148
ra2        5   00.00-08.00:148 08.00-19.00:74 19.00-24.00:148
ra2        6   00.00-08.00:148 08.00-12.00:74 12.00-24.00:148
#
#=================== Zone 3 : 30-52 km ================================
#
#rate-code day rate1           rate2           rate3          
#--------- --- --------------- --------------- ---------------
ra3        0   00.00-24.00:94
ra3        1   00.00-08.00:94 08.00-19.00:47 19.00-24.00:94
ra3        2   00.00-08.00:94 08.00-19.00:47 19.00-24.00:94
ra3        3   00.00-08.00:94 08.00-19.00:47 19.00-24.00:94
ra3        4   00.00-08.00:94 08.00-19.00:47 19.00-24.00:94
ra3        5   00.00-08.00:94 08.00-19.00:47 19.00-24.00:94
ra3        6   00.00-08.00:94 08.00-12.00:47 12.00-24.00:94
#
#
#=================== Zone 4 : > 52 km =================================
#
#rate-code day rate1           rate2           rate3          
#--------- --- --------------- --------------- ---------------
ra4        0   00.00-24.00:78
ra4        1   00.00-08.00:78 08.00-19.00:39 19.00-24.00:78
ra4        2   00.00-08.00:78 08.00-19.00:39 19.00-24.00:78
ra4        3   00.00-08.00:78 08.00-19.00:39 19.00-24.00:78
ra4        4   00.00-08.00:78 08.00-19.00:39 19.00-24.00:78
ra4        5   00.00-08.00:78 08.00-19.00:39 19.00-24.00:78
ra4        6   00.00-08.00:78 08.00-12.00:39 12.00-24.00:78
#
#
#=================== INTERNATIONAL : Allemagne ========================
# (pour appeler Hellmuth)
#
#rate-code day rate1           rate2           rate3          
#--------- --- --------------- --------------- ---------------
ra5        0   00.00-24.00:27
ra5        1   00.00-08.00:27 08.00-19.00:21 19.00-24.00:27
ra5        2   00.00-08.00:27 08.00-19.00:21 19.00-24.00:27
ra5        3   00.00-08.00:27 08.00-19.00:21 19.00-24.00:27
ra5        4   00.00-08.00:27 08.00-19.00:21 19.00-24.00:27
ra5        5   00.00-08.00:27 08.00-19.00:21 19.00-24.00:27
ra5        6   00.00-08.00:27 08.00-12.00:21 12.00-24.00:27
#
#
#=================== INTERNATIONAL : Pays-Bas ==========================
#
#rate-code day rate1           rate2           rate3          
#--------- --- --------------- --------------- ---------------
ra6        0   00.00-24.00:25
ra6        1   00.00-08.00:25 08.00-19.00:20 19.00-24.00:25
ra6        2   00.00-08.00:25 08.00-19.00:20 19.00-24.00:25
ra6        3   00.00-08.00:25 08.00-19.00:20 19.00-24.00:25
ra6        4   00.00-08.00:25 08.00-19.00:20 19.00-24.00:25
ra6        5   00.00-08.00:25 08.00-19.00:20 19.00-24.00:25
ra6        6   00.00-08.00:25 08.00-12.00:20 12.00-24.00:25
#
#
#=================== INTERNATIONAL : Portugal ===========================
#
#rate-code day rate1           rate2           rate3          
#--------- --- --------------- --------------- ---------------
ra7        0   00.00-24.00:22
ra7        1   00.00-08.00:22 08.00-19.00:18 19.00-24.00:22
ra7        2   00.00-08.00:22 08.00-19.00:18 19.00-24.00:22
ra7        3   00.00-08.00:22 08.00-19.00:18 19.00-24.00:22
ra7        4   00.00-08.00:22 08.00-19.00:18 19.00-24.00:22
ra7        5   00.00-08.00:22 08.00-19.00:18 19.00-24.00:22
ra7        6   00.00-08.00:22 08.00-12.00:18 12.00-24.00:22
#
#
#=================== INTERNATIONAL : USA/Canada ========================
#
#rate-code day rate1           rate2           rate3          
#--------- --- --------------- --------------- ---------------
ra8        0   00.00-24.00:25
ra8        1   00.00-13.00:25 13.00-19.00:20 19.00-24.00:25
ra8        2   00.00-13.00:25 13.00-19.00:20 19.00-24.00:25
ra8        3   00.00-13.00:25 13.00-19.00:20 19.00-24.00:25
ra8        4   00.00-13.00:25 13.00-19.00:20 19.00-24.00:25
ra8        5   00.00-13.00:25 13.00-19.00:20 19.00-24.00:25
ra8        6   00.00-24.00:25
#
#================== EOF ==============================================
