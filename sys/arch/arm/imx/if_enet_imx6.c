/*	$NetBSD$	*/

/*
 * Copyright (c) 2014 Ryo Shimizu <ryo@nerv.org>
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include "locators.h"
#include "imxccm.h"
#include "imxocotp.h"

#include <sys/param.h>
#include <sys/device.h>
#include <sys/bus.h>

#include <arm/imx/imx6var.h>
#include <arm/imx/imx6_reg.h>
#include <arm/imx/imx6_ccmreg.h>
#include <arm/imx/imx6_ccmvar.h>
#include <arm/imx/imx6_ocotpreg.h>
#include <arm/imx/imx6_ocotpvar.h>
#include <arm/imx/if_enetreg.h>
#include <arm/imx/if_enetvar.h>

int
enet_match(device_t parent __unused, struct cfdata *match __unused, void *aux)
{
	struct axi_attach_args *aa;

	aa = aux;

	switch (aa->aa_addr) {
	case (IMX6_AIPS2_BASE + AIPS2_ENET_BASE):
		return 1;
	}

	return 0;
}

void
enet_attach(device_t parent, device_t self, void *aux)
{
	struct enet_softc *sc;
	struct axi_attach_args *aa;
#if NIMXOCOTP > 0
	uint32_t eaddr;
#endif

	aa = aux;
	sc = device_private(self);

	if (aa->aa_size == AXICF_SIZE_DEFAULT)
		aa->aa_size = AIPS_ENET_SIZE;

	sc->sc_imxtype = 6;	/* i.MX6 */
	sc->sc_unit = 0;

#if NIMXOCOTP > 0
	/* get mac-address from OCOTP */
	eaddr = imxocotp_read(OCOTP_MAC1);
	sc->sc_enaddr[0] = eaddr >> 8;
	sc->sc_enaddr[1] = eaddr;
	eaddr = imxocotp_read(OCOTP_MAC0);
	sc->sc_enaddr[2] = eaddr >> 24;
	sc->sc_enaddr[3] = eaddr >> 16;
	sc->sc_enaddr[4] = eaddr >> 8;
	sc->sc_enaddr[5] = eaddr;
#endif

#if NIMXCCM > 0
	/* PLL power up */
	if (imx6_pll_power(CCM_ANALOG_PLL_ENET, 1) != 0) {
		aprint_error_dev(sc->sc_dev,
		    "couldn't enable CCM_ANALOG_PLL_ENET\n");
		return;
	}

	sc->sc_pllclock = imx6_get_clock(IMX6CLK_PLL6);
#else
	sc->sc_pllclock = 50000000;
#endif

	enet_attach_common(self, aa->aa_iot, aa->aa_dmat, aa->aa_addr,
	    aa->aa_size, aa->aa_irq);
}
