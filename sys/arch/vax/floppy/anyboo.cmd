!	$NetBSD$
!
! BOOTSTRAP ANY DEVICE TO A SINGLE USER SHELL
!
SET DEF HEX
SET DEF LONG
SET REL:0
HALT
UNJAM
INIT
LOAD BOOT
D R8  0		! MEMORY NOT COUNTED
D R11 3		! 3=RB_ASKNAME+RB_SINGLE
D R10 0		! DEVICE CHOICE ISNT APPLICABLE
D AP FFFFFFFF	! WE ARE NOT NETBOOTED
START 22
