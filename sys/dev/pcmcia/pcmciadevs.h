/*	$NetBSD$	*/

/*
 * THIS FILE AUTOMATICALLY GENERATED.  DO NOT EDIT.
 *
 * generated from:
 *	NetBSD: pcmciadevs,v 1.232 2016/06/01 23:31:53 pgoyette Exp
 */
/* $FreeBSD: src/sys/dev/pccard/pccarddevs,v 1.20 2001/11/19 05:02:55 imp Exp $*/

/*-
 * Copyright (c) 1998, 2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Christos Zoulas.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Use "make -f Makefile.pcmicadevs" to regenerate pcmicadevs.h and
 * pcmicadevs_data.h
 */

/*
 * List of known PCMCIA vendors, sorted by numeric ID.
 */

#define	PCMCIA_VENDOR_FUJITSU	0x0004	/* Fujitsu Corporation */
#define	PCMCIA_VENDOR_INTERSIL	0x000b	/* Netgear */
#define	PCMCIA_VENDOR_PANASONIC	0x0032	/* Matsushita Electric Industrial Co. */
#define	PCMCIA_VENDOR_SANDISK	0x0045	/* Sandisk Corporation */
#define	PCMCIA_VENDOR_NEWMEDIA	0x0057	/* New Media Corporation */
#define	PCMCIA_VENDOR_INTEL	0x0089	/* Intel */
#define	PCMCIA_VENDOR_IBM	0x00a4	/* IBM Corporation */
#define	PCMCIA_VENDOR_SHARP	0x00b0	/* Sharp Corporation */
#define	PCMCIA_VENDOR_DIGITAL	0x0100	/* Digital Equipment Corporation */
#define	PCMCIA_VENDOR_3COM	0x0101	/* 3Com Corporation */
#define	PCMCIA_VENDOR_MEGAHERTZ	0x0102	/* Megahertz Corporation */
#define	PCMCIA_VENDOR_SOCKET	0x0104	/* Socket Communications */
#define	PCMCIA_VENDOR_TDK	0x0105	/* TDK Corporation */
#define	PCMCIA_VENDOR_XIRCOM	0x0105	/* Xircom */
#define	PCMCIA_VENDOR_SMC	0x0108	/* Standard Microsystems Corporation */
#define	PCMCIA_VENDOR_MOTOROLA	0x0109	/* Motorola Corporation */
#define	PCMCIA_VENDOR_NI	0x010b	/* National Instruments */
#define	PCMCIA_VENDOR_USROBOTICS	0x0115	/* US Robotics Corporation */
#define	PCMCIA_VENDOR_OLICOM	0x0121	/* Olicom */
#define	PCMCIA_VENDOR_PROXIM	0x0126	/* Proxim */
#define	PCMCIA_VENDOR_MEGAHERTZ2	0x0128	/* Megahertz Corporation */
#define	PCMCIA_VENDOR_ADAPTEC	0x012f	/* Adaptec Corporation */
#define	PCMCIA_VENDOR_QUATECH	0x0137	/* Quatech */
#define	PCMCIA_VENDOR_COMPAQ	0x0138	/* Compaq */
#define	PCMCIA_VENDOR_OSITECH	0x0140	/* Ositech */
#define	PCMCIA_VENDOR_DLINK_2	0x0143	/* D-Link */
#define	PCMCIA_VENDOR_DLINK_3	0x0149	/* D-Link */
#define	PCMCIA_VENDOR_LINKSYS	0x0149	/* Linksys Corporation */
#define	PCMCIA_VENDOR_NETGEAR	0x0149	/* Netgear */
#define	PCMCIA_VENDOR_IODATA3	0x0149	/* I-O DATA */
#define	PCMCIA_VENDOR_SIMPLETECH	0x014d	/* Simple Technology */
#define	PCMCIA_VENDOR_SYMBOL2	0x014d	/* Symbol */
#define	PCMCIA_VENDOR_LUCENT	0x0156	/* Lucent Technologies */
#define	PCMCIA_VENDOR_AIRONET	0x015f	/* Aironet Wireless Communications */
#define	PCMCIA_VENDOR_ERICSSON	0x016b	/* Ericsson */
#define	PCMCIA_VENDOR_PSION	0x016c	/* Psion */
#define	PCMCIA_VENDOR_COMPAQ2	0x0183	/* Compaq */
#define	PCMCIA_VENDOR_KINGSTON	0x0186	/* Kingston */
#define	PCMCIA_VENDOR_SIERRA	0x0192	/* Sierra Wireless */
#define	PCMCIA_VENDOR_DAYNA	0x0194	/* Dayna Corporation */
#define	PCMCIA_VENDOR_RAYTHEON	0x01a6	/* Raytheon */
#define	PCMCIA_VENDOR_IODATA	0x01bf	/* I-O DATA */
#define	PCMCIA_VENDOR_BELKIN	0x01bf	/* Belkin */
#define	PCMCIA_VENDOR_BAY	0x01eb	/* Bay Networks */
#define	PCMCIA_VENDOR_FARALLON	0x0200	/* Farallon Communications */
#define	PCMCIA_VENDOR_TELECOMDEVICE	0x021b	/* Telecom Device */
#define	PCMCIA_VENDOR_NOKIA	0x023d	/* Nokia Communications */
#define	PCMCIA_VENDOR_SAMSUNG	0x0250	/* Samsung */
#define	PCMCIA_VENDOR_ANYCOM	0x0264	/* Anycom */
#define	PCMCIA_VENDOR_ALVARION	0x0268	/* Alvarion Ltd. */
#define	PCMCIA_VENDOR_ARTEM	0x0268	/* ARtem */
#define	PCMCIA_VENDOR_SYMBOL	0x026c	/* Symbol */
#define	PCMCIA_VENDOR_BUFFALO	0x026f	/* BUFFALO (Melco Corporation) */
#define	PCMCIA_VENDOR_LINKSYS2	0x0274	/* The Linksys Group */
#define	PCMCIA_VENDOR_NECINFRONTIA	0x0288	/* NEC Infrontia */
#define	PCMCIA_VENDOR_IODATA2	0x028a	/* I-O DATA */
#define	PCMCIA_VENDOR_ASUSTEK	0x02aa	/* Asustek Computer */
#define	PCMCIA_VENDOR_SIEMENS	0x02ac	/* Siemens */
#define	PCMCIA_VENDOR_MICROSOFT	0x02d2	/* Microsoft Corporation */
#define	PCMCIA_VENDOR_AMBICOM	0x02df	/* AmbiCom Inc */
#define	PCMCIA_VENDOR_BREEZECOM	0x0a02	/* BreezeCOM */
#define	PCMCIA_VENDOR_NEWMEDIA2	0x10cd	/* NewMedia */
#define	PCMCIA_VENDOR_ACTIONTEC	0x1668	/* ACTIONTEC */
#define	PCMCIA_VENDOR_LASAT	0x3401	/* Lasat Communications A/S */
#define	PCMCIA_VENDOR_LEXARMEDIA	0x4e01	/* Lexar Media */
#define	PCMCIA_VENDOR_ARCHOS	0x5241	/* Archos */
#define	PCMCIA_VENDOR_DUAL	0x890f	/* Dual */
#define	PCMCIA_VENDOR_EDIMAX	0x890f	/* Edimax Technology Inc. */
#define	PCMCIA_VENDOR_COMPEX	0x8a01	/* Compex Corporation */
#define	PCMCIA_VENDOR_MELCO	0x8a01	/* Melco Corporation */
#define	PCMCIA_VENDOR_CONTEC	0xc001	/* Contec */
#define	PCMCIA_VENDOR_MACNICA	0xc00b	/* MACNICA */
#define	PCMCIA_VENDOR_ROLAND	0xc00c	/* Roland */
#define	PCMCIA_VENDOR_COREGA	0xc00f	/* Corega K.K. */
#define	PCMCIA_VENDOR_ALLIEDTELESIS	0xc00f	/* Allied Telesis K.K. */
#define	PCMCIA_VENDOR_HAGIWARASYSCOM	0xc012	/* Hagiwara SYS-COM */
#define	PCMCIA_VENDOR_RATOC	0xc015	/* RATOC System Inc. */
#define	PCMCIA_VENDOR_NEXTCOM	0xc020	/* NextCom K.K. */
#define	PCMCIA_VENDOR_EMTAC	0xc250	/* EMTAC Technology Corporation */
#define	PCMCIA_VENDOR_ELSA	0xd601	/* Elsa */

/*
 * List of known products.  Grouped by vendor, sorted by number within the
 * group.
 */

/* 3COM Products */
#define	PCMCIA_CIS_3COM_3CRWE737A	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_3COM_3CRWE737A	0x0001
#define	PCMCIA_CIS_3COM_3CXM056BNW	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_3COM_3CXM056BNW	0x002f
#define	PCMCIA_CIS_3COM_3CXEM556	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_3COM_3CXEM556	0x0035
#define	PCMCIA_CIS_3COM_3CXEM556INT	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_3COM_3CXEM556INT	0x003d
#define	PCMCIA_CIS_3COM_3CRWB6096	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_3COM_3CRWB6096	0x0040
#define	PCMCIA_CIS_3COM_3CCFEM556BI	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_3COM_3CCFEM556BI	0x0556
#define	PCMCIA_CIS_3COM_3C562	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_3COM_3C562	0x0562
#define	PCMCIA_CIS_3COM_3C589	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_3COM_3C589	0x0589
#define	PCMCIA_CIS_3COM_3C574	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_3COM_3C574	0x0574
#define	PCMCIA_CIS_3COM_3CRWE777A	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_3COM_3CRWE777A	0x0777
#define	PCMCIA_CIS_3COM_3C1	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_3COM_3C1	0x0cf1
#define	PCMCIA_CIS_3COM_3CRWE62092A	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_3COM_3CRWE62092A	0x2092

/* ACTIONTEC Products */
#define	PCMCIA_CIS_ACTIONTEC_PRISM	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ACTIONTEC_PRISM	0x0101

/* Adaptec Products */
#define	PCMCIA_CIS_ADAPTEC_APA1460	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ADAPTEC_APA1460	0x0001
#define	PCMCIA_CIS_ADAPTEC_APA1460A	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ADAPTEC_APA1460A	0x0002

/* Aironet */
#define	PCMCIA_CIS_AIRONET_PC4500	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_AIRONET_PC4500	0x0005
#define	PCMCIA_CIS_AIRONET_PC4800	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_AIRONET_PC4800	0x0007
#define	PCMCIA_CIS_AIRONET_350	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_AIRONET_350	0x000a

/* Allied Telesis K.K. */
#define	PCMCIA_CIS_ALLIEDTELESIS_LA_PCM	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ALLIEDTELESIS_LA_PCM	0x0002

/* Alvarion Ltd. */
#define	PCMCIA_CIS_ALVARION_BREEZENET	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ALVARION_BREEZENET	0x0001

/* AmbiCom Inc */
#define	PCMCIA_CIS_AMBICOM_WL54CF	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_AMBICOM_WL54CF	0x8103

/* Anycom */
#define	PCMCIA_CIS_ANYCOM_LSE041	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ANYCOM_LSE041	0x0004
#define	PCMCIA_CIS_ANYCOM_LSE039	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ANYCOM_LSE039	0x0008
#define	PCMCIA_CIS_ANYCOM_LSE139	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ANYCOM_LSE139	0x000b

/* Archos */
#define	PCMCIA_CIS_ARCHOS_ARC_ATAPI	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ARCHOS_ARC_ATAPI	0x0043

/* ARtem */
#define	PCMCIA_CIS_ARTEM_ONAIR	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ARTEM_ONAIR	0x0001

/* Asustek */
#define	PCMCIA_CIS_ASUSTEK_WL_100	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ASUSTEK_WL_100	0x0002

/* Bay Networks */
#define	PCMCIA_CIS_BAY_STACK_650	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_BAY_STACK_650	0x0804
#define	PCMCIA_CIS_BAY_SURFER_PRO	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_BAY_SURFER_PRO	0x0806
#define	PCMCIA_CIS_BAY_STACK_660	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_BAY_STACK_660	0x0807
#define	PCMCIA_CIS_BAY_EMOBILITY_11B	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_BAY_EMOBILITY_11B	0x080a

/* BreezeCOM */
#define	PCMCIA_CIS_BREEZECOM_BREEZENET	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_BREEZECOM_BREEZENET	0x0102

/* Belkin */
#define	PCMCIA_CIS_BELKIN_F5D5020	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_BELKIN_F5D5020	0x2328

/* BUFFALO */
#define	PCMCIA_CIS_BUFFALO_WLI_PCM_S11	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_BUFFALO_WLI_PCM_S11	0x0305
#define	PCMCIA_CIS_BUFFALO_LPC_CF_CLT	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_BUFFALO_LPC_CF_CLT	0x0307
#define	PCMCIA_CIS_BUFFALO_LPC4_CLX	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_BUFFALO_LPC4_CLX	0x0309
#define	PCMCIA_CIS_BUFFALO_LPC3_CLT	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_BUFFALO_LPC3_CLT	0x030a
#define	PCMCIA_CIS_BUFFALO_WLI_CF_S11G	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_BUFFALO_WLI_CF_S11G	0x030b

/* Compaq Products */
#define	PCMCIA_CIS_COMPAQ_NC5004	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_COMPAQ_NC5004	0x0002
#define	PCMCIA_CIS_COMPAQ2_CPQ_10_100	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_COMPAQ2_CPQ_10_100	0x010a

/* Compex Products */
#define	PCMCIA_CIS_COMPEX_AMP_WIRELESS	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_COMPEX_AMP_WIRELESS	0x0066
#define	PCMCIA_CIS_COMPEX_LINKPORT_ENET_B	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_COMPEX_LINKPORT_ENET_B	0x0100

/* Contec C-NET(PC) */
#define	PCMCIA_CIS_CONTEC_CNETPC	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_CONTEC_CNETPC	0x0000
#define	PCMCIA_CIS_CONTEC_FX_DS110_PCC	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_CONTEC_FX_DS110_PCC	0x0008

/* Dayna Products */
#define	PCMCIA_CIS_DAYNA_COMMUNICARD_E_1	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_DAYNA_COMMUNICARD_E_1	0x002d
#define	PCMCIA_CIS_DAYNA_COMMUNICARD_E_2	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_DAYNA_COMMUNICARD_E_2	0x002f

/* DIGITAL Products */
#define	PCMCIA_CIS_DIGITAL_MOBILE_MEDIA_CDROM	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_DIGITAL_MOBILE_MEDIA_CDROM	0x0d00

/* D-Link Products */
#define	PCMCIA_CIS_DLINK_2_DMF560TX	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_DLINK_2_DMF560TX	0xc0ab

/* Dual */
#define	PCMCIA_CIS_DUAL_NE2000	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_DUAL_NE2000	0x0100

/* Edimax Products */
#define	PCMCIA_CIS_EDIMAX_EP4000A	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_EDIMAX_EP4000A	0x0100
#define	PCMCIA_CIS_EDIMAX_EP4101	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_EDIMAX_EP4101	0x1090

/* ELSA Products */
#define	PCMCIA_CIS_ELSA_MC2_IEEE	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ELSA_MC2_IEEE	0x0001
#define	PCMCIA_CIS_ELSA_XI300_IEEE	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ELSA_XI300_IEEE	0x0002
#define	PCMCIA_CIS_ELSA_XI800_IEEE	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ELSA_XI800_IEEE	0x0004
#define	PCMCIA_CIS_ELSA_XI325_IEEE	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ELSA_XI325_IEEE	0x0005
#define	PCMCIA_CIS_ELSA_SMC2531WB	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ELSA_SMC2531WB	0x0010

/* EMTAC */
#define	PCMCIA_CIS_EMTAC_WLAN	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_EMTAC_WLAN	0x0002

/* Ericsson */
#define	PCMCIA_CIS_ERICSSON_WIRELESSLAN	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ERICSSON_WIRELESSLAN	0x0001

/* Farallon */
#define	PCMCIA_CIS_FARALLON_SKYLINE	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_FARALLON_SKYLINE	0x0a01

/* Fujutsu Products */
#define	PCMCIA_CIS_FUJITSU_SCSI600	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_FUJITSU_SCSI600	0x0401
#define	PCMCIA_CIS_FUJITSU_LA10S	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_FUJITSU_LA10S	0x1003
#define	PCMCIA_CIS_FUJITSU_LA501	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_FUJITSU_LA501	0x2000
#define	PCMCIA_CIS_FUJITSU_WL110	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_FUJITSU_WL110	0x2003

/* IBM Products */
#define	PCMCIA_CIS_IBM_MICRODRIVE	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IBM_MICRODRIVE	0x0000
#define	PCMCIA_CIS_IBM_3270	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IBM_3270	0x0001
#define	PCMCIA_CIS_IBM_INFOMOVER	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IBM_INFOMOVER	0x0002
#define	PCMCIA_CIS_IBM_5250	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IBM_5250	0x000b
#define	PCMCIA_CIS_IBM_TROPIC	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IBM_TROPIC	0x001e
#define	PCMCIA_CIS_IBM_SCSI	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IBM_SCSI	0x0026
#define	PCMCIA_CIS_IBM_PORTABLE_CDROM	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IBM_PORTABLE_CDROM	0x002d
#define	PCMCIA_CIS_IBM_HOME_AND_AWAY	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IBM_HOME_AND_AWAY	0x002e
#define	PCMCIA_CIS_IBM_WIRELESS_LAN_ENTRY	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IBM_WIRELESS_LAN_ENTRY	0x0032
#define	PCMCIA_CIS_IBM_SMART_CAPTURE_II	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IBM_SMART_CAPTURE_II	0x003c
#define	PCMCIA_CIS_IBM_ETHERJET	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IBM_ETHERJET	0x003f

/* Intel Products */
#define	PCMCIA_CIS_INTEL_PRO_WLAN_2011	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_INTEL_PRO_WLAN_2011	0x0001
#define	PCMCIA_CIS_INTEL_EEPRO100	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_INTEL_EEPRO100	0x010a

/* Intersil */
/* OEMs sell these things under different marketing names */
#define	PCMCIA_CIS_INTERSIL_ISL37100P	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_INTERSIL_ISL37100P	0x7100
#define	PCMCIA_CIS_INTERSIL_ISL37110P	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_INTERSIL_ISL37110P	0x7110
#define	PCMCIA_CIS_INTERSIL_ISL37300P	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_INTERSIL_ISL37300P	0x7300

/* I-O DATA */
#define	PCMCIA_CIS_IODATA_PCLATE	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IODATA_PCLATE	0x2216
#define	PCMCIA_CIS_IODATA2_WNB11PCM	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IODATA2_WNB11PCM	0x0002
#define	PCMCIA_CIS_IODATA2_WCF12	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IODATA2_WCF12	0x0673
#define	PCMCIA_CIS_IODATA3_PCETTXR	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_IODATA3_PCETTXR	0x0230

/* Kingston Products */
#define	PCMCIA_CIS_KINGSTON_KNE2	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_KINGSTON_KNE2	0x0100

/* Lasat Products */
#define	PCMCIA_CIS_LASAT_CREDIT_288	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_LASAT_CREDIT_288	0x2811

/* Lexar Media */
#define	PCMCIA_CIS_LEXARMEDIA_COMPACTFLASH	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_LEXARMEDIA_COMPACTFLASH	0x0100

/* Linksys corporation */
#define	PCMCIA_CIS_LINKSYS_TRUST_COMBO_ECARD	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_LINKSYS_TRUST_COMBO_ECARD	0x021b
#define	PCMCIA_CIS_LINKSYS_ETHERFAST	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_LINKSYS_ETHERFAST	0x0230
#define	PCMCIA_CIS_LINKSYS_ECARD_1	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_LINKSYS_ECARD_1	0x0265
#define	PCMCIA_CIS_LINKSYS_COMBO_ECARD	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_LINKSYS_COMBO_ECARD	0xc1ab
#define	PCMCIA_CIS_LINKSYS2_IWN	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_LINKSYS2_IWN	0x1612
#define	PCMCIA_CIS_LINKSYS2_IWN3	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_LINKSYS2_IWN3	0x1613
#define	PCMCIA_CIS_LINKSYS2_WCF11	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_LINKSYS2_WCF11	0x3301

/* Lucent */
#define	PCMCIA_CIS_LUCENT_HERMES	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_LUCENT_HERMES	0x0002
#define	PCMCIA_CIS_LUCENT_HERMES2	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_LUCENT_HERMES2	0x0003
#define	PCMCIA_CIS_LUCENT_HERMES25	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_LUCENT_HERMES25	0x0004

/* MACNICA */
#define	PCMCIA_CIS_MACNICA_ME1_JEIDA	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_MACNICA_ME1_JEIDA	0x3300

/* Megahertz Products */
#define	PCMCIA_CIS_MEGAHERTZ_EM3336	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_MEGAHERTZ_EM3336	0x0006
#define	PCMCIA_CIS_MEGAHERTZ_XJ4288	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_MEGAHERTZ_XJ4288	0x0023
#define	PCMCIA_CIS_MEGAHERTZ_XJ4336	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_MEGAHERTZ_XJ4336	0x0027
#define	PCMCIA_CIS_MEGAHERTZ_XJ5560	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_MEGAHERTZ_XJ5560	0x0034
#define	PCMCIA_CIS_MEGAHERTZ2_EM1144	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_MEGAHERTZ2_EM1144	0x0101
#define	PCMCIA_CIS_MEGAHERTZ2_XJACK	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_MEGAHERTZ2_XJACK	0x0103

/* Melco Products */
#define	PCMCIA_CIS_MELCO_LPC3_TX	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_MELCO_LPC3_TX	0xc1ab

/* Microsoft Products */
#define	PCMCIA_CIS_MICROSOFT_MN_520	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_MICROSOFT_MN_520	0x0001

/* Motorola Products */
#define	PCMCIA_CIS_MOTOROLA_POWER144	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_MOTOROLA_POWER144	0x0105
#define	PCMCIA_CIS_MOTOROLA_PM100C	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_MOTOROLA_PM100C	0x0302
#define	PCMCIA_CIS_MOTOROLA_MONTANA_336	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_MOTOROLA_MONTANA_336	0x0505

/* NEC Infrontia Products */
#define	PCMCIA_CIS_NECINFRONTIA_AX420N	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_NECINFRONTIA_AX420N	0x1b01

/* New Media Products */
#define	PCMCIA_CIS_NEWMEDIA_BASICS	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_NEWMEDIA_BASICS	0x0019
#define	PCMCIA_CIS_NEWMEDIA_LANSURFER	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_NEWMEDIA_LANSURFER	0x0021
#define	PCMCIA_CIS_NEWMEDIA_LIVEWIRE	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_NEWMEDIA_LIVEWIRE	0x1004
#define	PCMCIA_CIS_NEWMEDIA_MULTIMEDIA	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_NEWMEDIA_MULTIMEDIA	0x100b
#define	PCMCIA_CIS_NEWMEDIA_BUSTOASTER	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_NEWMEDIA_BUSTOASTER	0xc102
#define	PCMCIA_CIS_NEWMEDIA_WAVJAMMER	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_NEWMEDIA_WAVJAMMER	0xe005
#define	PCMCIA_CIS_NEWMEDIA2_BUSTOASTER	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_NEWMEDIA2_BUSTOASTER	0x0001

/* Netgear */
#define	PCMCIA_CIS_NETGEAR_FA410TX	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_NETGEAR_FA410TX	0x0230
#define	PCMCIA_CIS_NETGEAR_FA410TXC	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_NETGEAR_FA410TXC	0x4530
#define	PCMCIA_CIS_NETGEAR_FA411	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_NETGEAR_FA411	0x0411

/* National Instruments */
#define	PCMCIA_CIS_NI_PCMCIA_GPIB	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_NI_PCMCIA_GPIB	0x4882

/* NextCom K.K. */
#define	PCMCIA_CIS_NEXTCOM_NEXTHAWK	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_NEXTCOM_NEXTHAWK	0x0001

/* Nokia Products */
#define	PCMCIA_CIS_NOKIA_C020_WLAN	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_NOKIA_C020_WLAN	0x20c0

/* Olicom Products */
#define	PCMCIA_CIS_OLICOM_OC2220	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_OLICOM_OC2220	0x0101
#define	PCMCIA_CIS_OLICOM_TR	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_OLICOM_TR	0x2132
#define	PCMCIA_CIS_OLICOM_OC2231	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_OLICOM_OC2231	0x3122
#define	PCMCIA_CIS_OLICOM_OC3231	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_OLICOM_OC3231	0x3132
#define	PCMCIA_CIS_OLICOM_OC2232	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_OLICOM_OC2232	0x3222
#define	PCMCIA_CIS_OLICOM_OC3232	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_OLICOM_OC3232	0x3232

/* Ositech Products */
#define	PCMCIA_CIS_OSITECH_TRUMPCARD_SOD	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_OSITECH_TRUMPCARD_SOD	0x0008

/* Panasonic Products */
#define	PCMCIA_CIS_PANASONIC_KXLC002	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_PANASONIC_KXLC002	0x0304
#define	PCMCIA_CIS_PANASONIC_KXLC003	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_PANASONIC_KXLC003	0x0504
#define	PCMCIA_CIS_PANASONIC_KXLC005	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_PANASONIC_KXLC005	0x2704
#define	PCMCIA_CIS_PANASONIC_KME	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_PANASONIC_KME	0x2604

/* Proxim */
#define	PCMCIA_CIS_PROXIM_ROAMABOUT_2400FH	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_PROXIM_ROAMABOUT_2400FH	0x1058
#define	PCMCIA_CIS_PROXIM_RANGELAN2_7401	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_PROXIM_RANGELAN2_7401	0x1158
#define	PCMCIA_CIS_PROXIM_RANGELANDS_8430	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_PROXIM_RANGELANDS_8430	0x8000

/* Psion */
#define	PCMCIA_CIS_PSION_GOLDCARD	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_PSION_GOLDCARD	0x0020

/* Quatech */
#define	PCMCIA_CIS_QUATECH_SPP_100	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_QUATECH_SPP_100	0x0003
#define	PCMCIA_CIS_QUATECH_DSP_225	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_QUATECH_DSP_225	0x0008

/* Raylink/WebGear */
#define	PCMCIA_CIS_RAYTHEON_WLAN	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_RAYTHEON_WLAN	0x0000

/* Roland */
#define	PCMCIA_CIS_ROLAND_SCP55	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_ROLAND_SCP55	0x0001

/* Samsung */
#define	PCMCIA_CIS_SAMSUNG_SWL_2000N	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SAMSUNG_SWL_2000N	0x0002

/* Sandisk Products */
#define	PCMCIA_CIS_SANDISK_SDCFB	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SANDISK_SDCFB	0x0401

/* Sharp Products */
#define	PCMCIA_CIS_SHARP_PALDIO611S	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SHARP_PALDIO611S	0x0000

/* Sierra Wireless */
#define	PCMCIA_CIS_SIERRA_AC850	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SIERRA_AC850	0x0710

/* Simple Technology Products */
#define	PCMCIA_CIS_SIMPLETECH_COMMUNICATOR288	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SIMPLETECH_COMMUNICATOR288	0x0100
/* Simpletech ID also used by Symbol */
#define	PCMCIA_CIS_SIMPLETECH_SPECTRUM24	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SIMPLETECH_SPECTRUM24	0x801

/* Standard Microsystems Corporation Products */
#define	PCMCIA_CIS_SMC_8016	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SMC_8016	0x0105
#define	PCMCIA_CIS_SMC_EZCARD	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SMC_EZCARD	0x8022

/* Socket Communications Products */
#define	PCMCIA_CIS_SOCKET_EA_ETHER	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SOCKET_EA_ETHER	0x0000
#define	PCMCIA_CIS_SOCKET_LP_WLAN_CF	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SOCKET_LP_WLAN_CF	0x0001
#define	PCMCIA_CIS_SOCKET_PAGECARD	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SOCKET_PAGECARD	0x0003
#define	PCMCIA_CIS_SOCKET_DUAL_RS232	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SOCKET_DUAL_RS232	0x0006
#define	PCMCIA_CIS_SOCKET_LP_ETHER	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SOCKET_LP_ETHER	0x000d
#define	PCMCIA_CIS_SOCKET_LP_ETHER_CF	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SOCKET_LP_ETHER_CF	0x0075
#define	PCMCIA_CIS_SOCKET_DUAL_RS232_A	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SOCKET_DUAL_RS232_A	0x00a1
#define	PCMCIA_CIS_SOCKET_LP_ETH_10_100_CF	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SOCKET_LP_ETH_10_100_CF	0x0145

/* SpeedStream */
#define	PCMCIA_CIS_SIEMENS_SS1021	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SIEMENS_SS1021	0x0002

/* Symbol */
#define	PCMCIA_CIS_SYMBOL_LA4100	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SYMBOL_LA4100	0x0001

/* TDK Products */
#define	PCMCIA_CIS_TDK_LAK_CD011WL	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_TDK_LAK_CD011WL	0x0000
#define	PCMCIA_CIS_TDK_LAK_CD021BX	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_TDK_LAK_CD021BX	0x0200
#define	PCMCIA_CIS_TDK_LAK_CF010	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_TDK_LAK_CF010	0x0900
#define	PCMCIA_CIS_TDK_DFL9610	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_TDK_DFL9610	0x0d0a
#define	PCMCIA_CIS_TDK_C6500012	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_TDK_C6500012	0x410a
#define	PCMCIA_CIS_TDK_BLUETOOTH_PCCARD	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_TDK_BLUETOOTH_PCCARD	0x4254

/* Telecom Device */
#define	PCMCIA_CIS_TELECOMDEVICE_TCD_HPC100	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_TELECOMDEVICE_TCD_HPC100	0x0202

/* US Robotics Products */
#define	PCMCIA_CIS_USROBOTICS_WORLDPORT144	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_USROBOTICS_WORLDPORT144	0x3330

/* Xircom Products */
#define	PCMCIA_CIS_XIRCOM_CE	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_XIRCOM_CE	0x0108
#define	PCMCIA_CIS_XIRCOM_CE3	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_XIRCOM_CE3	0x010a
#define	PCMCIA_CIS_XIRCOM_CE2	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_XIRCOM_CE2	0x010b
#define	PCMCIA_CIS_XIRCOM_XE2000	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_XIRCOM_XE2000	0x0153
#define	PCMCIA_CIS_XIRCOM_CNW_801	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_XIRCOM_CNW_801	0x0801
#define	PCMCIA_CIS_XIRCOM_CNW_802	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_XIRCOM_CNW_802	0x0802
#define	PCMCIA_CIS_XIRCOM_CT2	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_XIRCOM_CT2	0x1101
#define	PCMCIA_CIS_XIRCOM_CEM	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_XIRCOM_CEM	0x110a
#define	PCMCIA_CIS_XIRCOM_REM56	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_XIRCOM_REM56	0x110a
#define	PCMCIA_CIS_XIRCOM_CEM28	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_XIRCOM_CEM28	0x110b
#define	PCMCIA_CIS_XIRCOM_CEM33	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_XIRCOM_CEM33	0x110b
#define	PCMCIA_CIS_XIRCOM_CEM56	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_XIRCOM_CEM56	0x110b

/*
 * Cards we know only by their cis, sorted by name.
 */
#define	PCMCIA_VENDOR_ACCTON	-1	/* ACCTON */
#define	PCMCIA_VENDOR_AMD	-1	/* AMD */
#define	PCMCIA_VENDOR_BILLIONTON	-1	/* Billionton Systems Inc. */
#define	PCMCIA_VENDOR_CNET	-1	/* CNet */
#define	PCMCIA_VENDOR_DLINK	-1	/* D-Link */
#define	PCMCIA_VENDOR_DYNALINK	-1	/* DynaLink */
#define	PCMCIA_VENDOR_EIGERLABS	-1	/* Eiger labs,Inc. */
#define	PCMCIA_VENDOR_EPSON	-1	/* Seiko Epson Corporation */
#define	PCMCIA_VENDOR_EXP	-1	/* EXP Computer Inc */
#define	PCMCIA_VENDOR_GEMTEK	-1	/* Gem Tek */
#define	PCMCIA_VENDOR_GENIUS	-1	/* Genius */
#define	PCMCIA_VENDOR_ICOM	-1	/* ICOM Inc */
#define	PCMCIA_VENDOR_INTERSIL2	-1	/* Intersil */
#define	PCMCIA_VENDOR_LANTECH	-1	/* Lantech Computer Company */
#define	PCMCIA_VENDOR_NAKAGAWAMETAL	-1	/* NAKAGAWA METAL */
#define	PCMCIA_VENDOR_NDC	-1	/* NDC */
#define	PCMCIA_VENDOR_PLANET	-1	/* Planet */
#define	PCMCIA_VENDOR_PLANEX	-1	/* Planex Communications Inc */
#define	PCMCIA_VENDOR_PREMAX	-1	/* Premax */
#define	PCMCIA_VENDOR_RPTI	-1	/* RPTI */
#define	PCMCIA_VENDOR_SVEC	-1	/* SVEC/Hawking Technology */
#define	PCMCIA_VENDOR_SYNERGY21	-1	/* Synergy 21 */
#define	PCMCIA_VENDOR_TAMARACK	-1	/* TAMARACK */
#define	PCMCIA_VENDOR_TEAC	-1	/* TEAC */
#define	PCMCIA_VENDOR_YEDATA	-1	/* Y-E DATA */

#define	PCMCIA_CIS_ACCTON_EN2212	{ "ACCTON", "EN2212", NULL, NULL }
#define	PCMCIA_PRODUCT_ACCTON_EN2212	-1
#define	PCMCIA_CIS_ACCTON_EN2216	{ "ACCTON", "EN2216-PCMCIA-ETHERNET", NULL, NULL }
#define	PCMCIA_PRODUCT_ACCTON_EN2216	-1
#define	PCMCIA_CIS_AMBICOM_AMB8002T	{ "AmbiCom Inc", "AMB8002T", NULL, NULL }
#define	PCMCIA_PRODUCT_AMBICOM_AMB8002T	-1
#define	PCMCIA_CIS_AMBICOM_AMB8110	{ "AmbiCom,Inc.", "Fast Ethernet PC Card(AMB8110)", NULL, NULL }
#define	PCMCIA_PRODUCT_AMBICOM_AMB8110	-1
#define	PCMCIA_CIS_AMD_AM79C930	{ "AMD", "Am79C930", NULL, NULL }
#define	PCMCIA_PRODUCT_AMD_AM79C930	-1
#define	PCMCIA_CIS_BILLIONTON_CFLT10N	{ "CF", "10Base-Ethernet", "1.0", NULL }
#define	PCMCIA_PRODUCT_BILLIONTON_CFLT10N	-1
#define	PCMCIA_CIS_BILLIONTON_LNT10TN	{ "PCMCIA", "LNT-10TN", NULL, NULL }
#define	PCMCIA_PRODUCT_BILLIONTON_LNT10TN	-1
#define	PCMCIA_CIS_CNET_NE2000	{ "CNet", "CN40BC Ethernet", NULL, NULL }
#define	PCMCIA_PRODUCT_CNET_NE2000	-1
#define	PCMCIA_CIS_COREGA_ETHER_CF_TD	{ "corega K.K.", "corega Ether CF-TD LAN Card", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_ETHER_CF_TD	-1
#define	PCMCIA_CIS_COREGA_ETHER_PCC_T	{ "corega K.K.", "corega Ether PCC-T", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_ETHER_PCC_T	-1
#define	PCMCIA_CIS_COREGA_ETHER_PCC_TD	{ "corega K.K.", "corega Ether PCC-TD", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_ETHER_PCC_TD	-1
#define	PCMCIA_CIS_COREGA_ETHER_PCC_TL	{ "corega K.K.", "corega Ether PCC-TL", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_ETHER_PCC_TL	-1
#define	PCMCIA_CIS_COREGA_ETHER_II_PCC_TD	{ "corega K.K.", "corega EtherII PCC-TD", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_ETHER_II_PCC_TD	-1
#define	PCMCIA_CIS_COREGA_ETHER_II_PCC_T	{ "corega K.K.", "corega EtherII PCC-T", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_ETHER_II_PCC_T	-1
#define	PCMCIA_CIS_COREGA_FAST_ETHER_PCC_TX	{ "corega K.K.", "corega FastEther PCC-TX", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_FAST_ETHER_PCC_TX	-1
#define	PCMCIA_CIS_COREGA_FETHER_PCC_TXD	{ "corega K.K.", "corega FEther PCC-TXD", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_FETHER_PCC_TXD	-1
#define	PCMCIA_CIS_COREGA_FETHER_PCC_TXF	{ "corega", "FEther PCC-TXF", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_FETHER_PCC_TXF	-1
#define	PCMCIA_CIS_COREGA_FETHER_II_PCC_TXD	{ "corega K.K.", "corega FEtherII PCC-TXD", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_FETHER_II_PCC_TXD	-1
#define	PCMCIA_CIS_COREGA_LAPCCTXD	{ "corega K.K.", "(CG-LAPCCTXD)", "(HardwareFirmwareVer.)", NULL }
#define	PCMCIA_PRODUCT_COREGA_LAPCCTXD	-1
#define	PCMCIA_CIS_COREGA_WIRELESS_LAN_PCC_11	{ "corega K.K.", "Wireless LAN PCC-11", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_WIRELESS_LAN_PCC_11	-1
#define	PCMCIA_CIS_COREGA_WIRELESS_LAN_PCCA_11	{ "corega K.K.", "Wireless LAN PCCA-11", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_WIRELESS_LAN_PCCA_11	-1
#define	PCMCIA_CIS_COREGA_WIRELESS_LAN_PCCB_11	{ "corega_K.K.", "Wireless_LAN_PCCB-11", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_WIRELESS_LAN_PCCB_11	-1
#define	PCMCIA_CIS_COREGA_WIRELESS_LAN_PCCL_11	{ "corega", "WL PCCL-11", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_WIRELESS_LAN_PCCL_11	-1
#define	PCMCIA_CIS_COREGA_WIRELESS_LAN_WLCFL_11	{ "corega", "WLCFL-11", NULL, NULL }
#define	PCMCIA_PRODUCT_COREGA_WIRELESS_LAN_WLCFL_11	-1

#define	PCMCIA_CIS_DLINK_DE650	{ "D-Link", "DE-650", NULL, NULL }
#define	PCMCIA_PRODUCT_DLINK_DE650	-1
#define	PCMCIA_CIS_DLINK_DE660	{ "D-Link", "DE-660", NULL, NULL }
#define	PCMCIA_PRODUCT_DLINK_DE660	-1
#define	PCMCIA_CIS_DLINK_DE660PLUS	{ "D-Link", "DE-660+", NULL, NULL }
#define	PCMCIA_PRODUCT_DLINK_DE660PLUS	-1
#define	PCMCIA_CIS_DLINK_DFE670TXD	{ "D-Link", "DFE-670TXD", NULL, NULL }
#define	PCMCIA_PRODUCT_DLINK_DFE670TXD	-1
#define	PCMCIA_CIS_DLINK_DWL650H	{ "D-Link Corporation", "D-Link DWL-650H 11Mbps WLAN Adapter", NULL, NULL }
#define	PCMCIA_PRODUCT_DLINK_DWL650H	-1
#define	PCMCIA_CIS_DYNALINK_L10C	{ "DYNALINK", "L10C", NULL, NULL }
#define	PCMCIA_PRODUCT_DYNALINK_L10C	-1

/*
 * vendor ID of EPX_AA2000 is Fujitsu (0x0004) and product ID is 0x2000, but
 * it conflicts with Fujitsu Towa LA501 Ethernet.
 */
#define	PCMCIA_CIS_EIGERLABS_EPX_AA2000	{ "Eiger labs,Inc.", "EPX-AA2000 PC Sound Card", NULL, NULL }
#define	PCMCIA_PRODUCT_EIGERLABS_EPX_AA2000	-1
#define	PCMCIA_CIS_EPSON_EEN10B	{ "Seiko Epson Corp.", "Ethernet", "P/N: EEN10B Rev. 00", NULL }
#define	PCMCIA_PRODUCT_EPSON_EEN10B	-1
#define	PCMCIA_CIS_EXP_EXPMULTIMEDIA	{ "EXP   ", "PnPIDE", "F1", NULL }
#define	PCMCIA_PRODUCT_EXP_EXPMULTIMEDIA	-1
#define	PCMCIA_CIS_FUJITSU_FMV_J181	{ "PCMCIA MBH10302", "01", NULL, NULL }
#define	PCMCIA_PRODUCT_FUJITSU_FMV_J181	-1
#define	PCMCIA_CIS_FUJITSU_FMV_J182	{ "FUJITSU", "LAN Card(FMV-J182)", "Ver.01", NULL }
#define	PCMCIA_PRODUCT_FUJITSU_FMV_J182	-1
#define	PCMCIA_CIS_FUJITSU_FMV_J182A	{ "FUJITSU", "LAN Card(FMV-J182)", "Ver.02", NULL }
#define	PCMCIA_PRODUCT_FUJITSU_FMV_J182A	-1
#define	PCMCIA_CIS_FUJITSU_ITCFJ182A	{ "FUJITSU", "LAN Card(ITCFJ182)", "Ver.01", NULL }
#define	PCMCIA_PRODUCT_FUJITSU_ITCFJ182A	-1
#define	PCMCIA_CIS_GEMTEK_WLAN	{ "Intersil", "PRISM 2_5 PCMCIA ADAPTER", NULL, NULL }
#define	PCMCIA_PRODUCT_GEMTEK_WLAN	-1
#define	PCMCIA_CIS_GENIUS_ME3000II	{ "PCMCIA", "PCMCIA-ETHERNET-CARD", "UE2216", NULL }
#define	PCMCIA_PRODUCT_GENIUS_ME3000II	-1
#define	PCMCIA_CIS_ICOM_SL200	{ "Icom", "SL-200", NULL, NULL }
#define	PCMCIA_PRODUCT_ICOM_SL200	-1
#define	PCMCIA_CIS_INTERSIL2_PRISM2	{ "INTERSIL", "HFA384x/IEEE", NULL, NULL }
#define	PCMCIA_PRODUCT_INTERSIL2_PRISM2	-1
#define	PCMCIA_CIS_IODATA_CBIDE2	{ "IO DATA", "CBIDE2      ", NULL, NULL }
#define	PCMCIA_PRODUCT_IODATA_CBIDE2	-1
#define	PCMCIA_CIS_IODATA_PCLAT	{ "I-O DATA", "PCLA", "ETHERNET", NULL }
#define	PCMCIA_PRODUCT_IODATA_PCLAT	-1
#define	PCMCIA_CIS_LANTECH_FASTNETTX	{ "ASIX", "AX88190", NULL, NULL }
#define	PCMCIA_PRODUCT_LANTECH_FASTNETTX	-1
#define	PCMCIA_CIS_LINKSYS_ECARD_2	{ "LINKSYS", "E-CARD", NULL, NULL }
#define	PCMCIA_PRODUCT_LINKSYS_ECARD_2	-1
#define	PCMCIA_CIS_MEGAHERTZ_XJ2288	{ "MEGAHERTZ", "MODEM XJ2288", NULL, NULL }
#define	PCMCIA_PRODUCT_MEGAHERTZ_XJ2288	-1
#define	PCMCIA_CIS_MELCO_LPC2_TX	{ "MELCO", "LPC2-TX", NULL, NULL }
#define	PCMCIA_PRODUCT_MELCO_LPC2_TX	-1
#define	PCMCIA_CIS_NAKAGAWAMETAL_LNT10TN	{ "PCMCIA", "LNT-10TN", NULL, NULL }
#define	PCMCIA_PRODUCT_NAKAGAWAMETAL_LNT10TN	-1
#define	PCMCIA_CIS_NANOSPEED_PRISM2	{ "NANOSPEED", "HFA384x/IEEE", NULL, NULL }
#define	PCMCIA_PRODUCT_NANOSPEED_PRISM2	-1
#define	PCMCIA_CIS_NDC_ND5100_E	{ "NDC", "Ethernet", "A", NULL }
#define	PCMCIA_PRODUCT_NDC_ND5100_E	-1
#define	PCMCIA_CIS_NEC_CMZ_RT_WP	{ "NEC", "Wireless Card CMZ-RT-WP", NULL, NULL }
#define	PCMCIA_PRODUCT_NEC_CMZ_RT_WP	-1
#define	PCMCIA_CIS_NTT_ME_WLAN	{ "NTT-ME", "11Mbps Wireless LAN PC Card", NULL, NULL }
#define	PCMCIA_PRODUCT_NTT_ME_WLAN	-1
#define	PCMCIA_CIS_PLANET_SMARTCOM2000	{ "PCMCIA", "UE2212", NULL, NULL }
#define	PCMCIA_PRODUCT_PLANET_SMARTCOM2000	-1
/*
 * vendor ID of both FNW-3600-T and FNW-3700-T is LINKSYS (0x0149) and
 * product ID is 0xc1ab, but it conflicts with LINKSYS Combo EthernetCard.
 */
#define	PCMCIA_CIS_PLANEX_FNW3600T	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_PLANEX_FNW3600T	-1
#define	PCMCIA_CIS_PLANEX_FNW3700T	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_PLANEX_FNW3700T	-1
#define	PCMCIA_CIS_PLANEX_GWNS11H	{ "PLANEX", "GW-NS11H Wireless LAN PC Card", NULL, NULL }
#define	PCMCIA_PRODUCT_PLANEX_GWNS11H	-1

/*
 * RATOC System Inc. uses the same product ID for many different cards.
 * For such cards, we can't use product ID for identification.
 */
/* vendor = 0xc015, product = 0x0001 */
#define	PCMCIA_CIS_RATOC_REX_9530	{ "RATOC System Inc.", "SCSI2 CARD 37", NULL, NULL }
#define	PCMCIA_PRODUCT_RATOC_REX_9530	-1
#define	PCMCIA_CIS_RATOC_REX_CFU1	{ "RATOC", "USB HOST CF+ Card", NULL, NULL }
#define	PCMCIA_PRODUCT_RATOC_REX_CFU1	-1
#define	PCMCIA_CIS_RATOC_REX_R280	{ "RATOC System Inc.", "10BASE_T CARD R280", NULL, NULL }
#define	PCMCIA_PRODUCT_RATOC_REX_R280	-1

#define	PCMCIA_CIS_RPTI_EP400	{ "RPTI LTD.", "EP400", NULL, NULL }
#define	PCMCIA_PRODUCT_RPTI_EP400	-1
#define	PCMCIA_CIS_RPTI_EP401	{ "RPTI", "EP401 Ethernet NE2000 Compatible", NULL, NULL }
#define	PCMCIA_PRODUCT_RPTI_EP401	-1
#define	PCMCIA_CIS_PREMAX_PE200	{ "PMX   ", "PE-200", NULL, NULL }
#define	PCMCIA_PRODUCT_PREMAX_PE200	-1
#define	PCMCIA_CIS_SMC_2632W	{ "SMC", "SMC2632W", NULL, NULL }
#define	PCMCIA_PRODUCT_SMC_2632W	-1
#define	PCMCIA_CIS_SMC_8041	{ "SMC", "8041TX-10/100-PC-Card-V2", NULL, NULL }
#define	PCMCIA_PRODUCT_SMC_8041	-1
#define	PCMCIA_CIS_SVEC_COMBOCARD	{ "Ethernet", "Adapter", NULL, NULL }
#define	PCMCIA_PRODUCT_SVEC_COMBOCARD	-1
#define	PCMCIA_CIS_SVEC_LANCARD	{ "SVEC", "FD605 PCMCIA EtherNet Card", NULL, NULL }
#define	PCMCIA_PRODUCT_SVEC_LANCARD	-1
/*
 * vendor ID of PN650TX is LINKSYS (0x0149) and product ID is 0xc1ab, but
 * it conflicts with LINKSYS Combo EthernetCard.
 */
#define	PCMCIA_CIS_SVEC_PN650TX	{ NULL, NULL, NULL, NULL }
#define	PCMCIA_PRODUCT_SVEC_PN650TX	-1
#define	PCMCIA_CIS_SYNERGY21_S21810	{ "PCMCIA", "Ethernet", "A", NULL }
#define	PCMCIA_PRODUCT_SYNERGY21_S21810	-1
#define	PCMCIA_CIS_TAMARACK_ETHERNET	{ "TAMARACK", "Ethernet", "A", NULL }
#define	PCMCIA_PRODUCT_TAMARACK_ETHERNET	-1
#define	PCMCIA_CIS_TEAC_IDECARDII	{ " ", "NinjaATA-", NULL, NULL }
#define	PCMCIA_PRODUCT_TEAC_IDECARDII	-1
#define	PCMCIA_CIS_TOSHIBA_CBIDE2	{ "LOOKMEET", "CBIDE2      ", NULL, NULL }
#define	PCMCIA_PRODUCT_TOSHIBA_CBIDE2	-1
#define	PCMCIA_CIS_XIRCOM_CFE_10	{ "Xircom", "CompactCard Ethernet", "CFE-10", NULL }
#define	PCMCIA_PRODUCT_XIRCOM_CFE_10	-1
#define	PCMCIA_CIS_YEDATA_EXTERNAL_FDD	{ "Y-E DATA", "External FDD", NULL, NULL }
#define	PCMCIA_PRODUCT_YEDATA_EXTERNAL_FDD	-1
#define	PCMCIA_CIS_ZOOM_AIR_4000	{ "Zoom", "Air-4000", NULL, NULL }
#define	PCMCIA_PRODUCT_ZOOM_AIR_4000	-1
