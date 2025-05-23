/*	$NetBSD$ */

/*-
 * Copyright (c) 2012  Genetec Corporation.  All rights reserved.
 * Written by Hiroyuki Bessho for Genetec Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include "imxgpio.h"

#include <sys/param.h>
#include <sys/device.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/pmf.h>

#include <machine/intr.h>

#include <dev/sdmmc/sdhcvar.h>
#include <dev/sdmmc/sdhcreg.h>
#include <dev/sdmmc/sdmmcvar.h>

#include <arm/imx/imx6_reg.h>
#include <arm/imx/imx6var.h>
#include <arm/imx/imx6_ccmvar.h>
#include <arm/imx/imx6_ccmreg.h>
#include <arm/imx/imxgpiovar.h>


struct sdhc_axi_softc {
	struct sdhc_softc sc_sdhc;
	/* we have only one slot */
	struct sdhc_host *sc_hosts[1];
	int32_t sc_gpio_cd;
	int32_t sc_gpio_cd_low_active;
	void *sc_ih;
};

static int sdhc_match(device_t, cfdata_t, void *);
static void sdhc_attach(device_t, device_t, void *);
static int imx6_sdhc_card_detect(struct sdhc_softc *);

CFATTACH_DECL_NEW(sdhc_axi, sizeof(struct sdhc_axi_softc),
    sdhc_match, sdhc_attach, NULL, NULL);

static int
sdhc_match(device_t parent, cfdata_t cf, void *aux)
{
	struct axi_attach_args *aa = aux;

	switch (aa->aa_addr) {
	case IMX6_AIPS2_BASE + AIPS2_USDHC1_BASE:
	case IMX6_AIPS2_BASE + AIPS2_USDHC2_BASE:
	case IMX6_AIPS2_BASE + AIPS2_USDHC3_BASE:
	case IMX6_AIPS2_BASE + AIPS2_USDHC4_BASE:
		return 1;
	}

	return 0;
}

static int
imx6_sdhc_card_detect(struct sdhc_softc *ssc)
{
	struct sdhc_axi_softc *sc;
	int detect;

	sc = device_private(ssc->sc_dev);
#if NIMXGPIO > 0
	if (sc->sc_gpio_cd >= 0) {
		detect = gpio_data_read(sc->sc_gpio_cd);
	} else
#endif
		detect = 1;
	if (sc->sc_gpio_cd_low_active)
		detect = detect ? 0 : 1;

	return detect;
}

static void
sdhc_set_gpio_cd(struct sdhc_axi_softc *sc, const char *name)
{
	prop_dictionary_t dict;
	const char *pin_data;
	int grp, pin;

	dict = device_properties(sc->sc_sdhc.sc_dev);
	if (!prop_dictionary_get_cstring_nocopy(dict, name, &pin_data))
		return;

	/*
	 * "!1,6" -> gpio_cd = GPIO_NO(1,6),  gpio_cd_low_active = 1
	 * "3,31" -> gpio_cd = GPIO_NO(3,31), gpio_cd_low_active = 0
	 * "!"    -> always not detected
	 * none   -> always detected
	 */
	if (*pin_data == '!') {
		sc->sc_gpio_cd_low_active = 1;
		pin_data++;
	} else
		sc->sc_gpio_cd_low_active = 0;

	sc->sc_gpio_cd = -1;
	if (*pin_data == '\0')
		return;

	for (grp = 0; (*pin_data >= '0') && (*pin_data <= '9'); pin_data++)
		grp = grp * 10 + *pin_data - '0';

	KASSERT(*pin_data == ',');
	pin_data++;

	for (pin = 0; (*pin_data >= '0') && (*pin_data <= '9'); pin_data++)
		pin = pin * 10 + *pin_data - '0';

	KASSERT(*pin_data == '\0');

	sc->sc_gpio_cd = GPIO_NO(grp, pin);
#if NIMXGPIO > 0
	gpio_set_direction(sc->sc_gpio_cd, GPIO_DIR_IN);
#endif
}

static void
sdhc_attach(device_t parent, device_t self, void *aux)
{
	struct sdhc_axi_softc *sc = device_private(self);
	struct axi_attach_args *aa = aux;
	bus_space_tag_t iot = aa->aa_iot;
	bus_space_handle_t ioh;
	u_int perclk = 0, v;

	sc->sc_sdhc.sc_dev = self;
	sc->sc_sdhc.sc_dmat = aa->aa_dmat;

	if (bus_space_map(iot, aa->aa_addr, AIPS2_USDHC_SIZE, 0, &ioh)) {
		aprint_error_dev(self, "can't map\n");
		return;
	}

	aprint_normal(": Ultra Secured Digial Host Controller\n");
	aprint_naive("\n");
	sc->sc_sdhc.sc_host = sc->sc_hosts;

	switch (aa->aa_addr) {
	case IMX6_AIPS2_BASE + AIPS2_USDHC1_BASE:
		v = imx6_ccm_read(CCM_CCGR6);
		imx6_ccm_write(CCM_CCGR6, v | CCM_CCGR6_USDHC1_CLK_ENABLE(3));
		perclk = imx6_get_clock(IMX6CLK_USDHC1);
		sdhc_set_gpio_cd(sc, "usdhc1-cd-gpio");
		break;
	case IMX6_AIPS2_BASE + AIPS2_USDHC2_BASE:
		v = imx6_ccm_read(CCM_CCGR6);
		imx6_ccm_write(CCM_CCGR6, v | CCM_CCGR6_USDHC2_CLK_ENABLE(3));
		perclk = imx6_get_clock(IMX6CLK_USDHC2);
		sdhc_set_gpio_cd(sc, "usdhc2-cd-gpio");
		break;
	case IMX6_AIPS2_BASE + AIPS2_USDHC3_BASE:
		v = imx6_ccm_read(CCM_CCGR6);
		imx6_ccm_write(CCM_CCGR6, v | CCM_CCGR6_USDHC3_CLK_ENABLE(3));
		perclk = imx6_get_clock(IMX6CLK_USDHC3);
		sdhc_set_gpio_cd(sc, "usdhc3-cd-gpio");
		break;
	case IMX6_AIPS2_BASE + AIPS2_USDHC4_BASE:
		v = imx6_ccm_read(CCM_CCGR6);
		imx6_ccm_write(CCM_CCGR6, v | CCM_CCGR6_USDHC4_CLK_ENABLE(3));
		perclk = imx6_get_clock(IMX6CLK_USDHC4);
		sdhc_set_gpio_cd(sc, "usdhc4-cd-gpio");
		break;
	}

	sc->sc_sdhc.sc_clkbase = perclk / 1000;
	sc->sc_sdhc.sc_flags |=
	    SDHC_FLAG_USE_DMA |
	    SDHC_FLAG_NO_PWR0 |
	    SDHC_FLAG_HAVE_DVS |
	    SDHC_FLAG_32BIT_ACCESS |
	    SDHC_FLAG_8BIT_MODE |
	    SDHC_FLAG_USE_ADMA2 |
	    SDHC_FLAG_POLL_CARD_DET |
	    SDHC_FLAG_USDHC;

	sc->sc_ih = intr_establish(aa->aa_irq, IPL_SDMMC, IST_LEVEL,
	    sdhc_intr, &sc->sc_sdhc);
	if (sc->sc_ih == NULL) {
		aprint_error_dev(self, "can't establish interrupt\n");
		return;
	}

	sc->sc_sdhc.sc_vendor_card_detect = imx6_sdhc_card_detect;
	if (sdhc_host_found(&sc->sc_sdhc, iot, ioh, AIPS2_USDHC_SIZE)) {
		aprint_error_dev(self, "can't initialize host\n");
		return;
	}

	if (!pmf_device_register1(self, sdhc_suspend, sdhc_resume,
	    sdhc_shutdown)) {
		aprint_error_dev(self, "can't establish power hook\n");
	}
}
