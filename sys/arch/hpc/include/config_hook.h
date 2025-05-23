/*	$NetBSD$	*/

/*-
 * Copyright (c) 1999-2001
 *         Shin Takemura and PocketBSD Project. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the PocketBSD project
 *	and its contributors.
 * 4. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef _CONFIG_HOOK_H_
#define _CONFIG_HOOK_H_

enum config_hook_mode {
	CONFIG_HOOK_SHARE,
	CONFIG_HOOK_REPLACE,
	CONFIG_HOOK_EXCLUSIVE,
};

typedef void *config_hook_tag;
typedef void *config_call_tag;

void	config_hook_init(void);
config_hook_tag config_hook(int, long, enum config_hook_mode,
    int (*func)(void *, int, long, void *), void *);
void	config_unhook(config_hook_tag);
int	__config_hook_call(int, long, void *, int);
config_call_tag config_connect(int, long);
void	config_disconnect(config_call_tag crx);
int	config_connected_call(config_call_tag, void *);

static __inline int
config_hook_call(int type, long id, void *msg)
{
	return __config_hook_call(type, id, msg, 0);
}

static __inline int
config_hook_call_reverse(int type, long id, void *msg)
{
	return __config_hook_call(type, id, msg, 1);
}

/*
 * hook types and IDs
 */
/* apm releted events */
#define CONFIG_HOOK_PMEVENT		0
#define CONFIG_HOOK_PMEVENT_STANDBYREQ		0
#define CONFIG_HOOK_PMEVENT_SUSPENDREQ		1
#define CONFIG_HOOK_PMEVENT_HARDPOWER		2
#define CONFIG_HOOK_PMEVENT_BATTERY		3
#define CONFIG_HOOK_PMEVENT_AC			4


/* get value */
#define CONFIG_HOOK_GET			1
/* set value and send event */
#define CONFIG_HOOK_SET			2
/* external event */
#define CONFIG_HOOK_EVENT		CONFIG_HOOK_SET


/* CONFIG_HOOK_GET/CONFIG_HOOK_SET/CONFIG_HOOK_EVENT event id's */
#define CONFIG_HOOK_POWER_COM0			0
#define CONFIG_HOOK_POWER_COM1			1
#define CONFIG_HOOK_POWER_COM2			2
#define CONFIG_HOOK_POWER_COM3			3
#define CONFIG_HOOK_POWER_LCDLIGHT		4
#define CONFIG_HOOK_POWER_LCD			5
#define CONFIG_HOOK_POWER_SPEAKER		6
#define CONFIG_HOOK_BUTTON_POWER		7
#define CONFIG_HOOK_BUTTON_OK			8
#define CONFIG_HOOK_BUTTON_CANCEL		9
#define CONFIG_HOOK_BUTTON_UP			10
#define CONFIG_HOOK_BUTTON_DOWN			11
#define CONFIG_HOOK_BUTTON_REC			12
#define CONFIG_HOOK_BUTTON_COVER		13
#define CONFIG_HOOK_BUTTON_LIGHT		14
#define CONFIG_HOOK_BUTTON_CONTRAST		15
#define CONFIG_HOOK_BUTTON_APP0			16
#define CONFIG_HOOK_BUTTON_APP1			17
#define CONFIG_HOOK_BUTTON_APP2			18
#define CONFIG_HOOK_BUTTON_APP3			19
#define CONFIG_HOOK_BUTTON_LEFT			20
#define CONFIG_HOOK_BUTTON_RIGHT		21
#define CONFIG_HOOK_BUTTON_CONTRAST_UP		22
#define CONFIG_HOOK_BUTTON_CONTRAST_DOWN	23
#define CONFIG_HOOK_BUTTON_LIGHT_UP		24
#define CONFIG_HOOK_BUTTON_LIGHT_DOWN		25
#define CONFIG_HOOK_LED				26
#define CONFIG_HOOK_LED_MAX			27
#define CONFIG_HOOK_CONTRAST			28
#define CONFIG_HOOK_CONTRAST_MAX		29
#define CONFIG_HOOK_BRIGHTNESS			30
#define CONFIG_HOOK_BRIGHTNESS_MAX		31
#define CONFIG_HOOK_COM0_CTS			32
#define CONFIG_HOOK_COM1_CTS			33
#define CONFIG_HOOK_COM2_CTS			34
#define CONFIG_HOOK_COM3_CTS			35
#define CONFIG_HOOK_COM0_DCD			36
#define CONFIG_HOOK_COM1_DCD			37
#define CONFIG_HOOK_COM2_DCD			38
#define CONFIG_HOOK_COM3_DCD			39
#define CONFIG_HOOK_COM0_RTS			40
#define CONFIG_HOOK_COM1_RTS			41
#define CONFIG_HOOK_COM2_RTS			42
#define CONFIG_HOOK_COM3_RTS			43
#define CONFIG_HOOK_COM0_DTR			44
#define CONFIG_HOOK_COM1_DTR			45
#define CONFIG_HOOK_COM2_DTR			46
#define CONFIG_HOOK_COM3_DTR			47
#define CONFIG_HOOK_BATTERYVAL			48
#define CONFIG_HOOK_ACADAPTER			49
#define CONFIG_HOOK_CHARGE			50

/* power control type */
#define CONFIG_HOOK_POWERCONTROL	3
#define CONFIG_HOOK_POWERCONTROL_COM0		CONFIG_HOOK_POWER_COM0
#define CONFIG_HOOK_POWERCONTROL_COM1		CONFIG_HOOK_POWER_COM1
#define CONFIG_HOOK_POWERCONTROL_COM2		CONFIG_HOOK_POWER_COM2
#define CONFIG_HOOK_POWERCONTROL_COM3		CONFIG_HOOK_POWER_COM3
#define CONFIG_HOOK_POWERCONTROL_LCDLIGHT	CONFIG_HOOK_POWER_LCDLIGHT
#define CONFIG_HOOK_POWERCONTROL_LCD		CONFIG_HOOK_POWER_LCD
#define CONFIG_HOOK_POWERCONTROL_SPEAKER	CONFIG_HOOK_POWER_SPEAKER
#define CONFIG_HOOK_POWERCONTROL_LED		CONFIG_HOOK_LED

/* button event type */
#define CONFIG_HOOK_BUTTONEVENT		4
#define CONFIG_HOOK_BUTTONEVENT_POWER		CONFIG_HOOK_BUTTON_POWER
#define CONFIG_HOOK_BUTTONEVENT_OK		CONFIG_HOOK_BUTTON_OK
#define CONFIG_HOOK_BUTTONEVENT_CANCEL		CONFIG_HOOK_BUTTON_CANCEL
#define CONFIG_HOOK_BUTTONEVENT_UP		CONFIG_HOOK_BUTTON_UP
#define CONFIG_HOOK_BUTTONEVENT_DOWN		CONFIG_HOOK_BUTTON_DOWN
#define CONFIG_HOOK_BUTTONEVENT_REC		CONFIG_HOOK_BUTTON_REC
#define CONFIG_HOOK_BUTTONEVENT_COVER		CONFIG_HOOK_BUTTON_COVER
#define CONFIG_HOOK_BUTTONEVENT_LIGHT		CONFIG_HOOK_BUTTON_LIGHT
#define CONFIG_HOOK_BUTTONEVENT_CONTRAST	CONFIG_HOOK_BUTTON_CONTRAST
#define CONFIG_HOOK_BUTTONEVENT_APP0		CONFIG_HOOK_BUTTON_APP0
#define CONFIG_HOOK_BUTTONEVENT_APP1		CONFIG_HOOK_BUTTON_APP1
#define CONFIG_HOOK_BUTTONEVENT_APP2		CONFIG_HOOK_BUTTON_APP2
#define CONFIG_HOOK_BUTTONEVENT_APP3		CONFIG_HOOK_BUTTON_APP3
#define CONFIG_HOOK_BUTTONEVENT_LEFT		CONFIG_HOOK_BUTTON_LEFT
#define CONFIG_HOOK_BUTTONEVENT_RIGHT		CONFIG_HOOK_BUTTON_RIGHT
#define CONFIG_HOOK_BUTTONEVENT_CONTRAST_UP	CONFIG_HOOK_BUTTON_CONTRAST_UP
#define CONFIG_HOOK_BUTTONEVENT_CONTRAST_DOWN	CONFIG_HOOK_BUTTON_CONTRAST_DOWN
#define CONFIG_HOOK_BUTTONEVENT_LIGHT_UP	CONFIG_HOOK_BUTTON_LIGHT_UP
#define CONFIG_HOOK_BUTTONEVENT_LIGHT_DOWN	CONFIG_HOOK_BUTTON_LIGHT_DOWN

/* PCI interrupt type */
#define CONFIG_HOOK_PCIINTR		5
#define CONFIG_HOOK_PCIINTR_ID(bus, dev, func) \
	(((bus) << 16) | ((dev) << 11) | ((func) << 8))
#define CONFIG_HOOK_PCIINTR_BUS(id)		(((id) >> 16) & 0xff)
#define CONFIG_HOOK_PCIINTR_DEVICE(id)		(((id) >> 11) & 0x1f)
#define CONFIG_HOOK_PCIINTR_FUNCTION(id)	(((id) >>  8) & 0x07)

#define CONFIG_HOOK_NTYPES 		6

/*
 * nicknames for including from configuration file.
 */
#ifdef CONFIG_HOOK_DEFINE_NICKNAME
/* type nicknames */
#define PMEVENT		CONFIG_HOOK_PMEVENT
#define SET		CONFIG_HOOK_SET
#define EVENT		CONFIG_HOOK_EVENT
#define GET		CONFIG_HOOK_GET
#define POWER		CONFIG_HOOK_POWERCONTROL
#define BUTTON		CONFIG_HOOK_BUTTONEVENT
#define PCIINTR		CONFIG_HOOK_PCIINTR

/* ID nicknames */
/* power contorol */
#define PWCTL_COM0	CONFIG_HOOK_POWER_COM0
#define PWCTL_COM1	CONFIG_HOOK_POWER_COM1
#define PWCTL_COM2	CONFIG_HOOK_POWER_COM2
#define PWCTL_COM3	CONFIG_HOOK_POWER_COM3
#define PWCTL_LCDLIGHT	CONFIG_HOOK_POWER_LCDLIGHT
#define PWCTL_LCD	CONFIG_HOOK_POWER_LCD
#define PWCTL_SPEAKER	CONFIG_HOOK_POWER_SPEAKER
#define PWCTL_LED	CONFIG_HOOK_LED
/* buttons */
#define BTN_POWER	CONFIG_HOOK_BUTTON_POWER
#define BTN_OK		CONFIG_HOOK_BUTTON_OK
#define BTN_CANCEL	CONFIG_HOOK_BUTTON_CANCEL
#define BTN_UP		CONFIG_HOOK_BUTTON_UP
#define BTN_DOWN	CONFIG_HOOK_BUTTON_DOWN
#define BTN_REC		CONFIG_HOOK_BUTTON_REC
#define BTN_COVER	CONFIG_HOOK_BUTTON_COVER
#define BTN_LIGHT	CONFIG_HOOK_BUTTON_LIGHT
#define BTN_CONTRAST	CONFIG_HOOK_BUTTON_CONTRAST
#define BTN_APP0	CONFIG_HOOK_BUTTON_APP0
#define BTN_APP1	CONFIG_HOOK_BUTTON_APP1
#define BTN_APP2	CONFIG_HOOK_BUTTON_APP2
#define BTN_APP3	CONFIG_HOOK_BUTTON_APP3
#define BTN_LEFT	CONFIG_HOOK_BUTTON_LEFT
#define BTN_RIGHT	CONFIG_HOOK_BUTTON_RIGHT
#define BTN_CONTRAST_UP	CONFIG_HOOK_BUTTON_CONTRAST_UP
#define BTN_CONTRAST_DN	CONFIG_HOOK_BUTTON_CONTRAST_DOWN
#define BTN_LIGHT_UP	CONFIG_HOOK_BUTTON_LIGHT_UP
#define BTN_LIGHT_DN	CONFIG_HOOK_BUTTON_LIGHT_DOWN
/* PMEVENT */
#define PME_STANBY	CONFIG_HOOK_PMEVENT_STANDBYREQ
#define PME_SUSPEND	CONFIG_HOOK_PMEVENT_SUSPENDREQ
#define PME_HARDPOWER	CONFIG_HOOK_PMEVENT_HARDPOWER
#define	PME_BATTERY	CONFIG_HOOK_PMEVENT_BATTERY
#define	PME_AC		CONFIG_HOOK_PMEVENT_AC
/* set/get */
#define POWER_COM0	CONFIG_HOOK_POWER_COM0	
#define POWER_COM1	CONFIG_HOOK_POWER_COM1	
#define POWER_COM2	CONFIG_HOOK_POWER_COM2	
#define POWER_COM3	CONFIG_HOOK_POWER_COM3	
#define POWER_LCDLIGHT	CONFIG_HOOK_POWER_LCDLIGHT
#define POWER_LCD	CONFIG_HOOK_POWER_LCD
#define POWER_SPEAKER	CONFIG_HOOK_POWER_SPEAKER
#define BUTTON_POWER	CONFIG_HOOK_BUTTON_POWER
#define BUTTON_OK	CONFIG_HOOK_BUTTON_OK
#define BUTTON_CANCEL	CONFIG_HOOK_BUTTON_CANCEL
#define BUTTON_UP	CONFIG_HOOK_BUTTON_UP
#define BUTTON_DOWN	CONFIG_HOOK_BUTTON_DOWN
#define BUTTON_REC	CONFIG_HOOK_BUTTON_REC
#define BUTTON_COVER	CONFIG_HOOK_BUTTON_COVER
#define BUTTON_LIGHT	CONFIG_HOOK_BUTTON_LIGHT
#define BUTTON_CONTRAST	CONFIG_HOOK_BUTTON_CONTRAST
#define BUTTON_APP0	CONFIG_HOOK_BUTTON_APP0
#define BUTTON_APP1	CONFIG_HOOK_BUTTON_APP1
#define BUTTON_APP2	CONFIG_HOOK_BUTTON_APP2
#define BUTTON_APP3	CONFIG_HOOK_BUTTON_APP3
#define BUTTON_LEFT	CONFIG_HOOK_BUTTON_LEFT
#define BUTTON_RIGHT	CONFIG_HOOK_BUTTON_RIGHT
#define BUTTON_CONTRAST_UP	CONFIG_HOOK_BUTTON_CONTRAST_UP
#define BUTTON_CONTRAST_DOWN	CONFIG_HOOK_BUTTON_CONTRAST_DOWN
#define BUTTON_LIFHT_UP	CONFIG_HOOK_BUTTON_LIGHT_UP
#define BUTTON_LIFHT_DOWN	CONFIG_HOOK_BUTTON_LIGHT_DOWN
#define LED		CONFIG_HOOK_LED
#define LED_MAX		CONFIG_HOOK_LED_MAX
#define CONTRAST	CONFIG_HOOK_CONTRAST
#define CONTRAST_MAX	CONFIG_HOOK_CONTRAST_MAX
#define BRIGHTNESS	CONFIG_HOOK_BRIGHTNESS
#define BRIGHTNESS_MAX	CONFIG_HOOK_BRIGHTNESS_MAX
#define COM0_CTS	CONFIG_HOOK_COM0_CTS
#define COM1_CTS	CONFIG_HOOK_COM1_CTS
#define COM2_CTS	CONFIG_HOOK_COM2_CTS
#define COM3_CTS	CONFIG_HOOK_COM3_CTS
#define COM0_DCD	CONFIG_HOOK_COM0_DCD
#define COM1_DCD	CONFIG_HOOK_COM1_DCD
#define COM2_DCD	CONFIG_HOOK_COM2_DCD
#define COM3_DCD	CONFIG_HOOK_COM3_DCD
#define COM0_RTS	CONFIG_HOOK_COM0_RTS
#define COM1_RTS	CONFIG_HOOK_COM1_RTS
#define COM2_RTS	CONFIG_HOOK_COM2_RTS
#define COM3_RTS	CONFIG_HOOK_COM3_RTS
#define COM0_DTR	CONFIG_HOOK_COM0_DTR
#define COM1_DTR	CONFIG_HOOK_COM1_DTR
#define COM2_DTR	CONFIG_HOOK_COM2_DTR
#define COM3_DTR	CONFIG_HOOK_COM3_DTR
#define BATTERYVAL	CONFIG_HOOK_BATTERYVAL
#define ACADAPTER	CONFIG_HOOK_ACADAPTER
#define CHARGE		CONFIG_HOOK_CHARGE
/* PCI interrupt */
#define PCIINTR_ID(bus, dev, func)	CONFIG_HOOK_PCIINTR_ID(bus, dev, func)
#define PCIINTR_00_00_00	PCIINTR_ID(0,  0, 0)
#define PCIINTR_00_00_01	PCIINTR_ID(0,  0, 1)
#define PCIINTR_00_01_00	PCIINTR_ID(0,  1, 0)
#define PCIINTR_00_12_00	PCIINTR_ID(0, 12, 0)
#define PCIINTR_00_12_02	PCIINTR_ID(0, 12, 2)
#define PCIINTR_00_16_00	PCIINTR_ID(0, 16, 0)
#define PCIINTR_00_19_00	PCIINTR_ID(0, 19, 0)
#define PCIINTR_00_19_02	PCIINTR_ID(0, 19, 2)
#endif /* CONFIG_HOOK_DEFINE_NICKNAME */

/*
 * POWERCONTROL value
 */
#define PWCTL_ON	((void *)1)
#define PWCTL_OFF	((void *)0)

/* CONFIG_HOOK typical value */
#define	CONFIG_HOOK_ON	1
#define CONFIG_HOOK_OFF	0

/*
 * CONFIG_HOOK_LED value
 */
#define CONFIG_HOOK_LED_ON	0	
#define	CONFIG_HOOK_LED_OFF	1
#define CONFIG_HOOK_LED_FLASH	2	/* flash 100msec */
#define CONFIG_HOOK_LED_FLASH2	3	/* flash 200msec */
#define CONFIG_HOOK_LED_FLASH5	4	/* flash 500msec */
#define CONFIG_HOOK_LED_BLINK	5	/* blink 1sec interval */
#define CONFIG_HOOK_LED_BLINK2	6	/* blink 2sec interval */

/*
 * CONFIG_HOOK_AC event value
 */
#define CONFIG_HOOK_AC_OFF		0
#define CONFIG_HOOK_AC_ON_CHARGE	1
#define CONFIG_HOOK_AC_ON_NOCHARGE	2
#define CONFIG_HOOK_AC_UNKNOWN		3

/*
 * CONFIG_HOOK_BATTERY event value
 */
#define CONFIG_HOOK_BATT_CRITICAL	0
#define CONFIG_HOOK_BATT_10P		1
#define CONFIG_HOOK_BATT_20P		2
#define CONFIG_HOOK_BATT_30P		3
#define CONFIG_HOOK_BATT_LOW		4
#define CONFIG_HOOK_BATT_40P		5
#define CONFIG_HOOK_BATT_50P		6
#define CONFIG_HOOK_BATT_60P		7
#define CONFIG_HOOK_BATT_70P		8
#define CONFIG_HOOK_BATT_HIGH		9
#define CONFIG_HOOK_BATT_80P		10
#define CONFIG_HOOK_BATT_90P		11
#define CONFIG_HOOK_BATT_100P		12
#define CONFIG_HOOK_BATT_UNKNOWN	13
#define CONFIG_HOOK_BATT_NO_SYSTEM_BATTERY	14

#define CONFIG_HOOK_MAXVALUE	14	/* max value, check in this file */
#define CONFIG_HOOK_VALUEP(p)						\
	((int)(p) >=  0 && (int)(p) <= CONFIG_HOOK_MAXVALUE)
#define CONFIG_HOOK_PTRP(p)	(!CONFIG_HOOK_VALUEP(p))

#endif /* _CONFIG_HOOK_H_ */
