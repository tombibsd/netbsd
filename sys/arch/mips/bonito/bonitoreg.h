/*	$NetBSD$	*/

/*
 * Bonito Register Map
 * Copyright (c) 1999 Algorithmics Ltd
 *
 * Algorithmics gives permission for anyone to use and modify this file
 * without any obligation or license condition except that you retain
 * this copyright message in any source redistribution in whole or part.
 *
 * Updated copies of this and other files can be found at
 * ftp://ftp.algor.co.uk/pub/bonito/
 *
 * Users of the Bonito controller are warmly recommended to contribute
 * any useful changes back to Algorithmics (mail to
 * bonito@algor.co.uk).
 */

/* Revision 1.48 autogenerated on 08/17/99 15:20:01 */

#ifndef _BONITO_H_

#define BONITO(x)	(BONITO_REG_BASE + (x))


#ifdef _LP64
#define	REGVAL(x)	*((volatile u_int32_t *)MIPS_PHYS_TO_XKPHYS_UNCACHED(x))
#define	REGVAL8(x)      *((volatile u_int8_t *)MIPS_PHYS_TO_XKPHYS_UNCACHED(x))
#else
#define	REGVAL(x)	*((volatile u_int32_t *) MIPS_PHYS_TO_KSEG1(x))
#define	REGVAL8(x)      *((volatile u_int8_t *)MIPS_PHYS_TO_KSEG1(x))
#endif

#define BONITO_BOOT_BASE		0x1fc00000
#define BONITO_BOOT_SIZE		0x00100000
#define BONITO_BOOT_TOP 		(BONITO_BOOT_BASE+BONITO_BOOT_SIZE-1)
#define BONITO_FLASH_BASE		0x1c000000
#define BONITO_FLASH_SIZE		0x03000000
#define BONITO_FLASH_TOP		(BONITO_FLASH_BASE+BONITO_FLASH_SIZE-1)
#define BONITO_SOCKET_BASE		0x1f800000
#define BONITO_SOCKET_SIZE		0x00400000
#define BONITO_SOCKET_TOP		(BONITO_SOCKET_BASE+BONITO_SOCKET_SIZE-1)
#define BONITO_REG_BASE 		0x1fe00000
#define BONITO_REG_SIZE 		0x00040000
#define BONITO_REG_TOP			(BONITO_REG_BASE+BONITO_REG_SIZE-1)
#define BONITO_DEV_BASE 		0x1ff00000
#define BONITO_DEV_SIZE 		0x00100000
#define BONITO_DEV_TOP			(BONITO_DEV_BASE+BONITO_DEV_SIZE-1)
#define BONITO_PCILO_BASE		0x10000000
#define BONITO_PCILO_SIZE		0x0c000000
#define BONITO_PCILO_TOP		(BONITO_PCILO_BASE+BONITO_PCILO_SIZE-1)
#define BONITO_PCILO0_BASE		0x10000000
#define BONITO_PCILO1_BASE		0x14000000
#define BONITO_PCILO2_BASE		0x18000000
#define BONITO_PCIHI_BASE		0x20000000
#define BONITO_PCIHI_SIZE		0x20000000
#define BONITO_PCIHI_TOP		(BONITO_PCIHI_BASE+BONITO_PCIHI_SIZE-1)
#define LS2F_PCIHI_BASE			0x40000000UL
#define LS2F_PCIHI_SIZE			0x40000000UL
#define LS2F_PCIHI_TOP			(LS2F_PCIHI_BASE+LS2F_PCIHI_SIZE-1)
#define BONITO_PCIIO_BASE		0x1fd00000
#define BONITO_PCIIO_LEGACY		0x00004000UL
#define BONITO_PCIIO_SIZE		0x00100000
#define BONITO_PCIIO_TOP		(BONITO_PCIIO_BASE+BONITO_PCIIO_SIZE-1)
#define BONITO_PCICFG_BASE		0x1fe80000
#define BONITO_PCICFG_SIZE		0x00080000
#define BONITO_PCICFG_TOP		(BONITO_PCICFG_BASE+BONITO_PCICFG_SIZE-1)


/* Bonito Register Bases */

#define BONITO_PCICONFIGBASE		0x00
#define BONITO_REGBASE			0x100


/* PCI Configuration  Registers */

#define BONITO_PCI_REG(x)		BONITO(BONITO_PCICONFIGBASE + (x))
#define BONITO_PCIDID			BONITO_PCI_REG(0x00)
#define BONITO_PCICMD			BONITO_PCI_REG(0x04)
#define BONITO_PCICLASS 		BONITO_PCI_REG(0x08)
#define BONITO_PCILTIMER		BONITO_PCI_REG(0x0c)
#define BONITO_PCIBASE0 		BONITO_PCI_REG(0x10)
#define BONITO_PCIBASE1 		BONITO_PCI_REG(0x14)
#define BONITO_PCIBASE2 		BONITO_PCI_REG(0x18)
#define BONITO_PCIEXPRBASE		BONITO_PCI_REG(0x30)
#define BONITO_PCIINT			BONITO_PCI_REG(0x3c)

#define BONITO_PCICMD_PERR_CLR		0x80000000
#define BONITO_PCICMD_SERR_CLR		0x40000000
#define BONITO_PCICMD_MABORT_CLR	0x20000000
#define BONITO_PCICMD_MTABORT_CLR	0x10000000
#define BONITO_PCICMD_TABORT_CLR	0x08000000
#define BONITO_PCICMD_MPERR_CLR 	0x01000000
#define BONITO_PCICMD_PERRRESPEN	0x00000040
#define BONITO_PCICMD_ASTEPEN		0x00000080
#define BONITO_PCICMD_SERREN		0x00000100
#define BONITO_PCILTIMER_BUSLATENCY	0x0000ff00
#define BONITO_PCILTIMER_BUSLATENCY_SHIFT	8


#define	BONITO_REV_FPGA(x)		((x) & 0x80)
#define	BONITO_REV_MAJOR(x)		(((x) >> 4) & 0x7)
#define	BONITO_REV_MINOR(x)		((x) & 0xf)


/* 1. Bonito h/w Configuration */
/* Power on register */

#define BONITO_BONPONCFG		BONITO(BONITO_REGBASE + 0x00)

#define BONITO_BONPONCFG_SYSCONTROLLERRD	0x00040000
#define BONITO_BONPONCFG_ROMCS1SAMP	0x00020000
#define BONITO_BONPONCFG_ROMCS0SAMP	0x00010000
#define BONITO_BONPONCFG_CPUBIGEND	0x00004000
#define BONITO_BONPONCFG_CPUPARITY	0x00002000
#define BONITO_BONPONCFG_BURSTORDER	0x00001000
#define BONITO_BONPONCFG_CPUTYPE	0x00000007
#define BONITO_BONPONCFG_CPUTYPE_SHIFT	0
#define BONITO_BONPONCFG_PCIRESET_OUT	0x00000008
#define BONITO_BONPONCFG_IS_ARBITER	0x00000010
#define BONITO_BONPONCFG_ROMBOOT	0x000000c0
#define BONITO_BONPONCFG_ROMBOOT_SHIFT	6

#define BONITO_BONPONCFG_ROMBOOT_FLASH	(0x0<<BONITO_BONPONCFG_ROMBOOT_SHIFT)
#define BONITO_BONPONCFG_ROMBOOT_SOCKET (0x1<<BONITO_BONPONCFG_ROMBOOT_SHIFT)
#define BONITO_BONPONCFG_ROMBOOT_SDRAM	(0x2<<BONITO_BONPONCFG_ROMBOOT_SHIFT)
#define BONITO_BONPONCFG_ROMBOOT_CPURESET	(0x3<<BONITO_BONPONCFG_ROMBOOT_SHIFT)

#define BONITO_BONPONCFG_ROMCS0WIDTH	0x00000100
#define BONITO_BONPONCFG_ROMCS1WIDTH	0x00000200
#define BONITO_BONPONCFG_ROMCS0FAST	0x00000400
#define BONITO_BONPONCFG_ROMCS1FAST	0x00000800
#define BONITO_BONPONCFG_CONFIG_DIS	0x00000020


/* Other Bonito configuration */

#define BONITO_BONGENCFG_OFFSET		0x4
#define BONITO_BONGENCFG		BONITO(BONITO_REGBASE + BONITO_BONGENCFG_OFFSET)

#define BONITO_BONGENCFG_DEBUGMODE	0x00000001
#define BONITO_BONGENCFG_SNOOPEN	0x00000002
#define BONITO_BONGENCFG_CPUSELFRESET	0x00000004

#define BONITO_BONGENCFG_FORCE_IRQA	0x00000008
#define BONITO_BONGENCFG_IRQA_ISOUT	0x00000010
#define BONITO_BONGENCFG_IRQA_FROM_INT1 0x00000020
#define BONITO_BONGENCFG_BYTESWAP	0x00000040

#define BONITO_BONGENCFG_UNCACHED	0x00000080
#define BONITO_BONGENCFG_PREFETCHEN	0x00000100
#define BONITO_BONGENCFG_WBEHINDEN	0x00000200
#define BONITO_BONGENCFG_CACHEALG	0x00000c00
#define BONITO_BONGENCFG_CACHEALG_SHIFT 10
#define BONITO_BONGENCFG_PCIQUEUE	0x00001000
#define BONITO_BONGENCFG_CACHESTOP	0x00002000
#define BONITO_BONGENCFG_MSTRBYTESWAP	0x00004000
#define BONITO_BONGENCFG_BUSERREN	0x00008000
#define BONITO_BONGENCFG_NORETRYTIMEOUT 0x00010000
#define BONITO_BONGENCFG_SHORTCOPYTIMEOUT 0x00020000

/* 2. IO & IDE configuration */

#define BONITO_IODEVCFG 		BONITO(BONITO_REGBASE + 0x08)

/* 3. IO & IDE configuration */

#define BONITO_SDCFG			BONITO(BONITO_REGBASE + 0x0c)

/* 4. PCI address map control */

#define BONITO_PCIMAP			BONITO(BONITO_REGBASE + 0x10)
#define BONITO_PCIMEMBASECFG		BONITO(BONITO_REGBASE + 0x14)
#define BONITO_PCIMAP_CFG		BONITO(BONITO_REGBASE + 0x18)

/* 5. ICU & GPIO regs */

/* GPIO Regs - r/w */

#define BONITO_GPIODATA_OFFSET 		0x1c
#define BONITO_GPIODATA 		BONITO(BONITO_REGBASE + BONITO_GPIODATA_OFFSET)
#define BONITO_GPIOIE			BONITO(BONITO_REGBASE + 0x20)

/* ICU Configuration Regs - r/w */

#define BONITO_INTEDGE			BONITO(BONITO_REGBASE + 0x24)
#define BONITO_INTSTEER 		BONITO(BONITO_REGBASE + 0x28)
#define BONITO_INTPOL			BONITO(BONITO_REGBASE + 0x2c)

/* ICU Enable Regs - IntEn & IntISR are r/o. */

#define BONITO_INTENSET 		BONITO(BONITO_REGBASE + 0x30)
#define BONITO_INTENCLR 		BONITO(BONITO_REGBASE + 0x34)
#define BONITO_INTEN			BONITO(BONITO_REGBASE + 0x38)
#define BONITO_INTISR			BONITO(BONITO_REGBASE + 0x3c)

/* PCI mail boxes */

#define BONITO_PCIMAIL0_OFFSET		0x40
#define BONITO_PCIMAIL1_OFFSET		0x44
#define BONITO_PCIMAIL2_OFFSET		0x48
#define BONITO_PCIMAIL3_OFFSET		0x4c
#define BONITO_PCIMAIL0 		BONITO(BONITO_REGBASE + 0x40)
#define BONITO_PCIMAIL1 		BONITO(BONITO_REGBASE + 0x44)
#define BONITO_PCIMAIL2 		BONITO(BONITO_REGBASE + 0x48)
#define BONITO_PCIMAIL3 		BONITO(BONITO_REGBASE + 0x4c)


/* 6. PCI cache */

#define BONITO_PCICACHECTRL		BONITO(BONITO_REGBASE + 0x50)
#define BONITO_PCICACHETAG		BONITO(BONITO_REGBASE + 0x54)

#define BONITO_PCIBADADDR		BONITO(BONITO_REGBASE + 0x58)
#define BONITO_PCIMSTAT 		BONITO(BONITO_REGBASE + 0x5c)


/*
#define BONITO_PCIRDPOST		BONITO(BONITO_REGBASE + 0x60)
#define BONITO_PCIDATA			BONITO(BONITO_REGBASE + 0x64)
*/

#define LS2F_CHIPCFG0			BONITO(BONITO_REGBASE + 0x80)
#define LS2FCFG_FREQSCALE_MASK		0x00000007
#define LS2FCFG_DISABLE_SCACHE		0x00000008	/* disable secondary cache */
#define LS2FCFG_BUFFER_CPU_TO_RAM	0x00000020
#define LS2FCFG_BUFFER_PCI_TO_RAM	0x00000040

/* 7. IDE DMA & Copier */

#define BONITO_CONFIGBASE		0x000
#define BONITO_BONITOBASE		0x100
#define BONITO_LDMABASE 		0x200
#define BONITO_COPBASE			0x300
#define BONITO_REG_BLOCKMASK		0x300

#define BONITO_LDMACTRL 		BONITO(BONITO_LDMABASE + 0x0)
#define BONITO_LDMASTAT 		BONITO(BONITO_LDMABASE + 0x0)
#define BONITO_LDMAADDR 		BONITO(BONITO_LDMABASE + 0x4)
#define BONITO_LDMAGO			BONITO(BONITO_LDMABASE + 0x8)
#define BONITO_LDMADATA 		BONITO(BONITO_LDMABASE + 0xc)

#define BONITO_COPCTRL			BONITO(BONITO_COPBASE + 0x0)
#define BONITO_COPSTAT			BONITO(BONITO_COPBASE + 0x0)
#define BONITO_COPPADDR 		BONITO(BONITO_COPBASE + 0x4)
#define BONITO_COPDADDR 		BONITO(BONITO_COPBASE + 0x8)
#define BONITO_COPGO			BONITO(BONITO_COPBASE + 0xc)


/* ###### Bit Definitions for individual Registers #### */

/* Gen DMA. */

#define BONITO_IDECOPDADDR_DMA_DADDR	0x0ffffffc
#define BONITO_IDECOPDADDR_DMA_DADDR_SHIFT	2
#define BONITO_IDECOPPADDR_DMA_PADDR	0xfffffffc
#define BONITO_IDECOPPADDR_DMA_PADDR_SHIFT	2
#define BONITO_IDECOPGO_DMA_SIZE	0x0000fffe
#define BONITO_IDECOPGO_DMA_SIZE_SHIFT	0
#define BONITO_IDECOPGO_DMA_WRITE	0x00010000
#define BONITO_IDECOPGO_DMAWCOUNT	0x000f0000
#define BONITO_IDECOPGO_DMAWCOUNT_SHIFT		16

#define BONITO_IDECOPCTRL_DMA_STARTBIT	0x80000000
#define BONITO_IDECOPCTRL_DMA_RSTBIT	0x40000000

/* DRAM - sdCfg */

#define BONITO_SDCFG_AROWBITS		0x00000003
#define BONITO_SDCFG_AROWBITS_SHIFT	0
#define BONITO_SDCFG_ACOLBITS		0x0000000c
#define BONITO_SDCFG_ACOLBITS_SHIFT	2
#define BONITO_SDCFG_ABANKBIT		0x00000010
#define BONITO_SDCFG_ASIDES		0x00000020
#define BONITO_SDCFG_AABSENT		0x00000040
#define BONITO_SDCFG_AWIDTH64		0x00000080

#define BONITO_SDCFG_BROWBITS		0x00000300
#define BONITO_SDCFG_BROWBITS_SHIFT	8
#define BONITO_SDCFG_BCOLBITS		0x00000c00
#define BONITO_SDCFG_BCOLBITS_SHIFT	10
#define BONITO_SDCFG_BBANKBIT		0x00001000
#define BONITO_SDCFG_BSIDES		0x00002000
#define BONITO_SDCFG_BABSENT		0x00004000
#define BONITO_SDCFG_BWIDTH64		0x00008000

#define BONITO_SDCFG_EXTRDDATA		0x00010000
#define BONITO_SDCFG_EXTRASCAS		0x00020000
#define BONITO_SDCFG_EXTPRECH		0x00040000
#define BONITO_SDCFG_EXTRASWIDTH	0x00180000
#define BONITO_SDCFG_EXTRASWIDTH_SHIFT	19
#define BONITO_SDCFG_DRAMMODESET	0x00200000
#define BONITO_SDCFG_DRAMEXTREGS	0x00400000
#define BONITO_SDCFG_DRAMPARITY 	0x00800000
#define BONITO_SDCFG_DRAMBURSTLEN 	0x03000000
#define BONITO_SDCFG_DRAMBURSTLEN_SHIFT 	24
#define BONITO_SDCFG_DRAMMODESET_DONE 	0x80000000

#define BONITO_SDCFG_DRAMRFSHMULT 	0xfc000000
#define BONITO_SDCFG_DRAMRFSHMULT_SHIFT	 	26

/* PCI Cache - pciCacheCtrl */

#define BONITO_PCICACHECTRL_CACHECMD	0x00000007
#define BONITO_PCICACHECTRL_CACHECMD_SHIFT	0
#define BONITO_PCICACHECTRL_CACHECMDLINE	0x00000018
#define BONITO_PCICACHECTRL_CACHECMDLINE_SHIFT	3
#define BONITO_PCICACHECTRL_CMDEXEC	0x00000020

#define BONITO_IODEVCFG_BUFFBIT_CS0	0x00000001
#define BONITO_IODEVCFG_SPEEDBIT_CS0	0x00000002
#define BONITO_IODEVCFG_MOREABITS_CS0	0x00000004

#define BONITO_IODEVCFG_BUFFBIT_CS1	0x00000008
#define BONITO_IODEVCFG_SPEEDBIT_CS1	0x00000010
#define BONITO_IODEVCFG_MOREABITS_CS1	0x00000020

#define BONITO_IODEVCFG_BUFFBIT_CS2	0x00000040
#define BONITO_IODEVCFG_SPEEDBIT_CS2	0x00000080
#define BONITO_IODEVCFG_MOREABITS_CS2	0x00000100

#define BONITO_IODEVCFG_BUFFBIT_CS3	0x00000200
#define BONITO_IODEVCFG_SPEEDBIT_CS3	0x00000400
#define BONITO_IODEVCFG_MOREABITS_CS3	0x00000800

#define BONITO_IODEVCFG_BUFFBIT_IDE	0x00001000
#define BONITO_IODEVCFG_SPEEDBIT_IDE	0x00002000
#define BONITO_IODEVCFG_WORDSWAPBIT_IDE 0x00004000
#define BONITO_IODEVCFG_MODEBIT_IDE	0x00008000
#define BONITO_IODEVCFG_DMAON_IDE	0x001f0000
#define BONITO_IODEVCFG_DMAON_IDE_SHIFT 16
#define BONITO_IODEVCFG_DMAOFF_IDE	0x01e00000
#define BONITO_IODEVCFG_DMAOFF_IDE_SHIFT	21
#define BONITO_IODEVCFG_EPROMSPLIT	0x02000000
#define BONITO_IODEVCFG_CPUCLOCKPERIOD	0xfc000000
#define BONITO_IODEVCFG_CPUCLOCKPERIOD_SHIFT	26

/* gpio */
#define BONITO_GPIO_GPIOW		0x000003ff
#define BONITO_GPIO_GPIOW_SHIFT 	0
#define BONITO_GPIO_GPIOR		0x01ff0000
#define BONITO_GPIO_GPIOR_SHIFT 	16
#define BONITO_GPIO_GPINR		0xfe000000
#define BONITO_GPIO_GPINR_SHIFT 	25
#define BONITO_GPIO_IOW(N)		(1<<(BONITO_GPIO_GPIOW_SHIFT+(N)))
#define BONITO_GPIO_IOR(N)		(1<<(BONITO_GPIO_GPIOR_SHIFT+(N)))
#define BONITO_GPIO_INR(N)		(1<<(BONITO_GPIO_GPINR_SHIFT+(N)))

/* ICU */
#define BONITO_ICU_MBOXES		0x0000000f
#define BONITO_ICU_MBOXES_SHIFT 	0
#define BONITO_ICU_DMARDY		0x00000010
#define BONITO_ICU_DMAEMPTY		0x00000020
#define BONITO_ICU_COPYRDY		0x00000040
#define BONITO_ICU_COPYEMPTY		0x00000080
#define BONITO_ICU_COPYERR		0x00000100
#define BONITO_ICU_PCIIRQ		0x00000200
#define BONITO_ICU_MASTERERR		0x00000400
#define BONITO_ICU_SYSTEMERR		0x00000800
#define BONITO_ICU_DRAMPERR		0x00001000
#define BONITO_ICU_RETRYERR		0x00002000
#define BONITO_ICU_GPIOS		0x01ff0000
#define BONITO_ICU_GPIOS_SHIFT		16
#define BONITO_ICU_GPINS		0x7e000000
#define BONITO_ICU_GPINS_SHIFT		25
#define BONITO_ICU_MBOX(N)		(1<<(BONITO_ICU_MBOXES_SHIFT+(N)))
#define BONITO_ICU_GPIO(N)		(1<<(BONITO_ICU_GPIOS_SHIFT+(N)))
#define BONITO_ICU_GPIN(N)		(1<<(BONITO_ICU_GPINS_SHIFT+(N)))

/* pcimap */

#define BONITO_PCIMAP_PCIMAP_LO0	0x0000003f
#define BONITO_PCIMAP_PCIMAP_LO0_SHIFT	0
#define BONITO_PCIMAP_PCIMAP_LO1	0x00000fc0
#define BONITO_PCIMAP_PCIMAP_LO1_SHIFT	6
#define BONITO_PCIMAP_PCIMAP_LO2	0x0003f000
#define BONITO_PCIMAP_PCIMAP_LO2_SHIFT	12
#define BONITO_PCIMAP_PCIMAP_2		0x00040000
#define BONITO_PCIMAP_WIN(WIN,ADDR)	((((ADDR)>>26) & BONITO_PCIMAP_PCIMAP_LO0) << ((WIN)*6))

#define BONITO_PCIMAP_WINSIZE		(1<<26)
#define BONITO_PCIMAP_WINOFFSET(ADDR)	((ADDR) & (BONITO_PCIMAP_WINSIZE - 1))
#define BONITO_PCIMAP_WINBASE(ADDR)	((ADDR) << 26)

/* pcimembaseCfg */

#define BONITO_PCIMEMBASECFG_MASK		0xf0000000
#define BONITO_PCIMEMBASECFG_MEMBASE0_MASK	0x0000001f
#define BONITO_PCIMEMBASECFG_MEMBASE0_MASK_SHIFT	0
#define BONITO_PCIMEMBASECFG_MEMBASE0_TRANS	0x000003e0
#define BONITO_PCIMEMBASECFG_MEMBASE0_TRANS_SHIFT	5
#define BONITO_PCIMEMBASECFG_MEMBASE0_CACHED	0x00000400
#define BONITO_PCIMEMBASECFG_MEMBASE0_IO	0x00000800

#define BONITO_PCIMEMBASECFG_MEMBASE1_MASK	0x0001f000
#define BONITO_PCIMEMBASECFG_MEMBASE1_MASK_SHIFT	12
#define BONITO_PCIMEMBASECFG_MEMBASE1_TRANS	0x003e0000
#define BONITO_PCIMEMBASECFG_MEMBASE1_TRANS_SHIFT	17
#define BONITO_PCIMEMBASECFG_MEMBASE1_CACHED	0x00400000
#define BONITO_PCIMEMBASECFG_MEMBASE1_IO	0x00800000

#define BONITO_PCIMEMBASECFG_ASHIFT	23
#define BONITO_PCIMEMBASECFG_AMASK	0x007fffff
#define BONITO_PCIMEMBASECFGSIZE(WIN,SIZE)	(((~((SIZE)-1))>>(BONITO_PCIMEMBASECFG_ASHIFT-BONITO_PCIMEMBASECFG_MEMBASE##WIN##_MASK_SHIFT)) & BONITO_PCIMEMBASECFG_MEMBASE##WIN##_MASK)
#define BONITO_PCIMEMBASECFGBASE(WIN,BASE)	(((BASE)>>(BONITO_PCIMEMBASECFG_ASHIFT-BONITO_PCIMEMBASECFG_MEMBASE##WIN##_TRANS_SHIFT)) & BONITO_PCIMEMBASECFG_MEMBASE##WIN##_TRANS)

#define BONITO_PCIMEMBASECFG_SIZE(WIN,CFG)	(((((~(CFG)) & BONITO_PCIMEMBASECFG_MEMBASE##WIN##_MASK)) << (BONITO_PCIMEMBASECFG_ASHIFT - BONITO_PCIMEMBASECFG_MEMBASE##WIN##_MASK_SHIFT)) | BONITO_PCIMEMBASECFG_AMASK)

#define BONITO_PCIMEMBASECFG_ADDRMASK(WIN,CFG)	((((CFG) & BONITO_PCIMEMBASECFG_MEMBASE##WIN##_MASK) >> BONITO_PCIMEMBASECFG_MEMBASE##WIN##_MASK_SHIFT) << BONITO_PCIMEMBASECFG_ASHIFT)
#define BONITO_PCIMEMBASECFG_ADDRMASK(WIN,CFG)	((((CFG) & BONITO_PCIMEMBASECFG_MEMBASE##WIN##_MASK) >> BONITO_PCIMEMBASECFG_MEMBASE##WIN##_MASK_SHIFT) << BONITO_PCIMEMBASECFG_ASHIFT)
#define BONITO_PCIMEMBASECFG_ADDRTRANS(WIN,CFG)	((((CFG) & BONITO_PCIMEMBASECFG_MEMBASE##WIN##_TRANS) >> BONITO_PCIMEMBASECFG_MEMBASE##WIN##_TRANS_SHIFT) << BONITO_PCIMEMBASECFG_ASHIFT)

#define	BONITO_PCITOPHYS(WIN,ADDR,CFG)	( \
					  (((ADDR) & (~(BONITO_PCIMEMBASECFG_MASK))) & (~(BONITO_PCIMEMBASECFG_ADDRMASK(WIN,CFG)))) | \
					  (BONITO_PCIMEMBASECFG_ADDRTRANS(WIN,CFG)) \
					)
/* PCIMAP Cfg */

#define BONITO_PCIMAPCFG_TYPE1		0x00010000

/* PCICmd */

#define BONITO_PCICMD_MEMEN		0x00000002
#define BONITO_PCICMD_MSTREN		0x00000004


#define BONITO_TIMERCFG			BONITO(BONITO_REGBASE + 0x60)

/*
 * Bonito interrupt assignments
 */

/*
 * Loongson 2F assignments
 */

#define	LOONGSON_INTR_GPIO0		0
#define	LOONGSON_INTR_GPIO1		1
#define	LOONGSON_INTR_GPIO2		2
#define	LOONGSON_INTR_GPIO3		3

/* pci interrupts */
#define	LOONGSON_INTR_PCIA		4
#define	LOONGSON_INTR_PCIB		5
#define	LOONGSON_INTR_PCIC		6
#define	LOONGSON_INTR_PCID		7

#define	LOONGSON_INTR_PCI_PARERR	8
#define	LOONGSON_INTR_PCI_SYSERR	9
#define	LOONGSON_INTR_DRAM_PARERR	10

/* non-PCI interrupts */
#define	LOONGSON_INTR_INT0		11
#define	LOONGSON_INTR_INT1		12
#define	LOONGSON_INTR_INT2		13
#define	LOONGSON_INTR_INT3		14

#define	LOONGSON_INTRMASK_GPIO0		0x00000001	/* can't interrupt */
#define	LOONGSON_INTRMASK_GPIO1		0x00000002
#define	LOONGSON_INTRMASK_GPIO2		0x00000004
#define	LOONGSON_INTRMASK_GPIO3		0x00000008

#define	LOONGSON_INTRMASK_GPIO		0x0000000f

/* pci interrupts */
#define	LOONGSON_INTRMASK_PCIA		0x00000010
#define	LOONGSON_INTRMASK_PCIB		0x00000020
#define	LOONGSON_INTRMASK_PCIC		0x00000040
#define	LOONGSON_INTRMASK_PCID		0x00000080

#define	LOONGSON_INTRMASK_PCI_PARERR	0x00000100
#define	LOONGSON_INTRMASK_PCI_SYSERR	0x00000200
#define	LOONGSON_INTRMASK_DRAM_PARERR	0x00000400

/* non-PCI interrupts */
#define	LOONGSON_INTRMASK_INT0		0x00000800
#define	LOONGSON_INTRMASK_INT1		0x00001000
#define	LOONGSON_INTRMASK_INT2		0x00002000
#define	LOONGSON_INTRMASK_INT3		0x00004000

#define	LOONGSON_INTRMASK_LVL0		0x00007800 /* not maskable in bonito */
#define	LOONGSON_INTRMASK_LVL4		0x000007ff

/*
 * Loongson 2E (Bonito64) assignments
 */

#define	BONITO_INTRMASK_MBOX		0x0000000f
#define	BONITO_INTR_MBOX		0
#define	BONITO_INTRMASK_DMARDY		0x00000010
#define	BONITO_INTRMASK_DMAEMPTY	0x00000020
#define	BONITO_INTRMASK_COPYRDY		0x00000040
#define	BONITO_INTRMASK_COPYEMPTY	0x00000080
#define	BONITO_INTRMASK_COPYERR		0x00000100
#define	BONITO_INTRMASK_PCIIRQ		0x00000200
#define	BONITO_INTRMASK_MASTERERR	0x00000400
#define	BONITO_INTRMASK_SYSTEMERR	0x00000800
#define	BONITO_INTRMASK_DRAMPERR	0x00001000
#define	BONITO_INTRMASK_RETRYERR	0x00002000
#define	BONITO_INTRMASK_GPIO		0x01ff0000
#define	BONITO_INTR_GPIO		16
#define	BONITO_INTRMASK_GPIN		0x7e000000
#define	BONITO_INTR_GPIN		25

/*
 * Bonito interrupt handling recipes:
 * - we have up to 32 interrupts at the Bonito level.
 * - systems with ISA devices also have 16 (well, 15) ISA interrupts with the
 *   usual 8259 pair. Bonito and ISA interrupts happen on two different levels.
 *
 * These arbitrary values may be changed as long as interrupt mask variables
 * use large enough integer types and always use the following macros to
 * handle interrupt masks.
 */

#define	INTPRI_BONITO		(INTPRI_CLOCK + 1)
#define	INTPRI_ISA		(INTPRI_BONITO + 1)

#define	BONITO_NDIRECT		32
#define	BONITO_NISA		16
#define	BONITO_NINTS		(BONITO_NDIRECT + BONITO_NISA)
#define	BONITO_ISA_IRQ(i)	((i) + BONITO_NDIRECT)
#define	BONITO_DIRECT_IRQ(i)	(i)
#define	BONITO_IRQ_IS_ISA(i)	((i) >= BONITO_NDIRECT)
#define	BONITO_IRQ_TO_ISA(i)	((i) - BONITO_NDIRECT)

#define	BONITO_DIRECT_MASK(imask)	((imask) & ((1L << BONITO_NDIRECT) - 1))
#define	BONITO_ISA_MASK(imask)		((imask) >> BONITO_NDIRECT)

#endif /* _BONITO_H_ */
