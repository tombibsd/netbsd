#---------------------------------------------------------------------------
#
#	i4b - Deutsche Telekom charging rates config file
#	-------------------------------------------------
#
#	$Id$
#
# $NetBSD$
#
#	last edit-date: [Mon Dec 13 21:42:13 1999]
#
#	-hm	updated to reflect last Telekom changes as of 1.1.99
#
#---------------------------------------------------------------------------
#
#	reference: http://www.telekom.de/untern/tarife/inland/index.htm
#
#---------------------------------------------------------------------------
#
#	Syntax:
#	-------
#	rate-code: ra0 = City, ra1 = Deutschland
# 	Day-No:    0-6, where 0 = Sunday, 1 = Monday, etc.
#	Rate:      start_hour.minutes-end_hour.minutes:period in seconds
#
#------------------------------------------------------------------------------
#
#=================== T-ISDN City =============================================================
#
#rate-code Day rate1           rate2           rate3           rate4           rate5          
#--------- --- --------------- --------------- --------------- --------------- ---------------
ra0        0   00.00-05.00:240 05.00-21.00:150 21.00-24.00:240
ra0        1   00.00-05.00:240 05.00-09.00:150 09.00-18.00:90  18.00-21.00:150 21.00-24.00:240
ra0        2   00.00-05.00:240 05.00-09.00:150 09.00-18.00:90  18.00-21.00:150 21.00-24.00:240
ra0        3   00.00-05.00:240 05.00-09.00:150 09.00-18.00:90  18.00-21.00:150 21.00-24.00:240
ra0        4   00.00-05.00:240 05.00-09.00:150 09.00-18.00:90  18.00-21.00:150 21.00-24.00:240
ra0        5   00.00-05.00:240 05.00-09.00:150 09.00-18.00:90  18.00-21.00:150 21.00-24.00:240
ra0        6   00.00-05.00:240 05.00-21.00:150 21.00-24.00:240
#
#=================== T-ISDN Deutschland ======================================================
#
#rate-code Day rate1           rate2           rate3           rate4           rate5          
#--------- --- --------------- --------------- --------------- --------------- ---------------
ra1        0   00.00-24.00:60
ra1        1   00.00-02.00:60  02.00-05.00:120 05.00-09.00:60  09.00-18.00:30  18.00-24.00:60
ra1        2   00.00-02.00:60  02.00-05.00:120 05.00-09.00:60  09.00-18.00:30  18.00-24.00:60
ra1        3   00.00-02.00:60  02.00-05.00:120 05.00-09.00:60  09.00-18.00:30  18.00-24.00:60
ra1        4   00.00-02.00:60  02.00-05.00:120 05.00-09.00:60  09.00-18.00:30  18.00-24.00:60
ra1        5   00.00-02.00:60  02.00-05.00:120 05.00-09.00:60  09.00-18.00:30  18.00-24.00:60
ra1        6   00.00-24.00:60
#
#================== EOF ======================================================================
