#	$NetBSD$

.include <bsd.endian.mk>

SUBDIR=  ADI_BRH_flash_0x00140000

.if ${MACHINE_CPU} == "arm" && ${TARGET_ENDIANNESS} == "1234"
# Little endian platforms
SUBDIR+= GEMINI_dram_0x01600000
SUBDIR+= IQ80310_flash_0x00080000
SUBDIR+= IQ80321_flash_0xf0080000
SUBDIR+= SMDK2410_flash_0x00000000
SUBDIR+= SMDK2800_flash_0x00000000
SUBDIR+= SMDK2800_flash_0x00100000
SUBDIR+= TS7200_flash_0x60660000
.endif

.include <bsd.subdir.mk>
