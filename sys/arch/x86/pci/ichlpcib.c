/*	$NetBSD$	*/

/*-
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Minoura Makoto and Matthew R. Green.
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
 * Intel I/O Controller Hub (ICHn) LPC Interface Bridge driver
 *
 *  LPC Interface Bridge is basically a pcib (PCI-ISA Bridge), but has
 *  some power management and monitoring functions.
 *  Currently we support the watchdog timer, SpeedStep (on some systems),
 *  the gpio interface, hpet timer, hardware random number generator,
 *  and the power management timer.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/sysctl.h>
#include <sys/timetc.h>
#include <sys/gpio.h>
#include <sys/bus.h>

#include <dev/pci/pcivar.h>
#include <dev/pci/pcireg.h>
#include <dev/pci/pcidevs.h>

#include <dev/gpio/gpiovar.h>

#include <dev/ic/acpipmtimer.h>
#include <dev/ic/i82801lpcreg.h>
#include <dev/ic/i82801lpcvar.h>
#include <dev/ic/hpetreg.h>
#include <dev/ic/hpetvar.h>

#include <arch/x86/pci/tco.h>

#include "pcibvar.h"
#include "gpio.h"
#include "fwhrng.h"

#define LPCIB_GPIO_NPINS 64

struct lpcib_softc {
	/* we call pcibattach() which assumes this starts like this: */
	struct pcib_softc	sc_pcib;

	struct pci_attach_args	sc_pa;
	int			sc_has_rcba;
	int			sc_has_ich5_hpet;

	/* RCBA */
	bus_space_tag_t		sc_rcbat;
	bus_space_handle_t	sc_rcbah;
	pcireg_t		sc_rcba_reg;

	/* Power management variables. */
	bus_space_tag_t		sc_iot;
	bus_space_handle_t	sc_ioh;
	bus_size_t		sc_iosize;

	/* HPET variables. */
	uint32_t		sc_hpet_reg;

#if NGPIO > 0
	device_t		sc_gpiobus;
	kmutex_t		sc_gpio_mtx;
	bus_space_tag_t		sc_gpio_iot;
	bus_space_handle_t	sc_gpio_ioh;
	bus_size_t		sc_gpio_ios;
	struct gpio_chipset_tag	sc_gpio_gc;
	gpio_pin_t		sc_gpio_pins[LPCIB_GPIO_NPINS];
#endif

#if NFWHRNG > 0
	device_t		sc_fwhbus;
#endif

	/* Speedstep */
	pcireg_t		sc_pmcon_orig;

	/* Power management */
	pcireg_t		sc_pirq[2];
	pcireg_t		sc_pmcon;
	pcireg_t		sc_fwhsel2;

	/* Child devices */
	device_t		sc_tco;
	device_t		sc_hpetbus;
	acpipmtimer_t		sc_pmtimer;
	pcireg_t		sc_acpi_cntl;

	struct sysctllog	*sc_log;
};

static int lpcibmatch(device_t, cfdata_t, void *);
static void lpcibattach(device_t, device_t, void *);
static int lpcibdetach(device_t, int);
static void lpcibchilddet(device_t, device_t);
static int lpcibrescan(device_t, const char *, const int *);
static bool lpcib_suspend(device_t, const pmf_qual_t *);
static bool lpcib_resume(device_t, const pmf_qual_t *);
static bool lpcib_shutdown(device_t, int);

static void pmtimer_configure(device_t);
static int pmtimer_unconfigure(device_t, int);

static void tcotimer_configure(device_t);
static int tcotimer_unconfigure(device_t, int);

static void speedstep_configure(device_t);
static void speedstep_unconfigure(device_t);
static int speedstep_sysctl_helper(SYSCTLFN_ARGS);

static void lpcib_hpet_configure(device_t);
static int lpcib_hpet_unconfigure(device_t, int);

#if NGPIO > 0
static void lpcib_gpio_configure(device_t);
static int lpcib_gpio_unconfigure(device_t, int);
static int lpcib_gpio_pin_read(void *, int);
static void lpcib_gpio_pin_write(void *, int, int);
static void lpcib_gpio_pin_ctl(void *, int, int);
#endif

#if NFWHRNG > 0
static void lpcib_fwh_configure(device_t);
static int lpcib_fwh_unconfigure(device_t, int);
#endif

struct lpcib_softc *speedstep_cookie;	/* XXX */

CFATTACH_DECL2_NEW(ichlpcib, sizeof(struct lpcib_softc),
    lpcibmatch, lpcibattach, lpcibdetach, NULL, lpcibrescan, lpcibchilddet);

static struct lpcib_device {
	pcireg_t vendor, product;
	int has_rcba;
	int has_ich5_hpet;
} lpcib_devices[] = {
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_3400_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_3420_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_3450_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_6300ESB_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_63XXESB_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801AA_LPC, 0, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801AB_LPC, 0, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801BA_LPC, 0, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801BAM_LPC, 0, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801CA_LPC, 0, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801CAM_LPC, 0, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801DB_LPC, 0, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801DBM_LPC, 0, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801E_LPC, 0, 1 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801EB_LPC, 0, 1 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801FB_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801FBM_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801G_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801GBM_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801GH_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801GHM_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801H_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801HEM_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801HH_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801HO_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801HBM_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801IB_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801IH_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801IM_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801IO_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801IR_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801IEM_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801JD_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801JDO_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801JIB_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_82801JIR_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_C202_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_C204_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_C206_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_C216_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_NM10_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_H55_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_H57_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_HM55_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_HM57_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_P55_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_PM55_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_Q57_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_QM57_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_QS57_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_B65_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_H61_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_H67_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_HM65_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_HM67_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_P67_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_Q65_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_Q67_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_QM67_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_QS67_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_UM67_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_Z68_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_B75_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_H77_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_MOBILE_HM70_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_MOBILE_HM75_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_MOBILE_HM76_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_MOBILE_HM77_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_MOBILE_QM77_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_MOBILE_QS77_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_MOBILE_UM77_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_NM70_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_Q75_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_Q77_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_Z75_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_Z77_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_Z87_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_Z85_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_HM86_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_H87_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_HM87_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_Q85_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_Q87_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_QM87_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_B85_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_H97_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_Z97_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_X99_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_X99_LPC_2, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_CORE5G_M_LPC_4, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_CORE5G_M_LPC_7, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_C222_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_C224_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_C226_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_H81_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_C600_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_DH89XXCC_LPC, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_DH89XXCL_LPC, 1, 0 },
#if 0
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_C2000_PCU_1, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_C2000_PCU_2, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_C2000_PCU_3, 1, 0 },
	{ PCI_VENDOR_INTEL, PCI_PRODUCT_INTEL_C2000_PCU_4, 1, 0 },
#endif

	{ 0, 0, 0, 0 },
};

/*
 * Autoconf callbacks.
 */
static int
lpcibmatch(device_t parent, cfdata_t match, void *aux)
{
	struct pci_attach_args *pa = aux;
	struct lpcib_device *lpcib_dev;

	/* We are ISA bridge, of course */
	if (PCI_CLASS(pa->pa_class) != PCI_CLASS_BRIDGE ||
	    PCI_SUBCLASS(pa->pa_class) != PCI_SUBCLASS_BRIDGE_ISA)
		return 0;

	for (lpcib_dev = lpcib_devices; lpcib_dev->vendor; ++lpcib_dev) {
		if (PCI_VENDOR(pa->pa_id) == lpcib_dev->vendor &&
		    PCI_PRODUCT(pa->pa_id) == lpcib_dev->product)
			return 10;
	}

	return 0;
}

static void
lpcibattach(device_t parent, device_t self, void *aux)
{
	struct pci_attach_args *pa = aux;
	struct lpcib_softc *sc = device_private(self);
	struct lpcib_device *lpcib_dev;
	pcireg_t pmbase;

	sc->sc_pa = *pa;

	for (lpcib_dev = lpcib_devices; lpcib_dev->vendor; ++lpcib_dev) {
		if (PCI_VENDOR(pa->pa_id) != lpcib_dev->vendor ||
		    PCI_PRODUCT(pa->pa_id) != lpcib_dev->product)
			continue;
		sc->sc_has_rcba = lpcib_dev->has_rcba;
		sc->sc_has_ich5_hpet = lpcib_dev->has_ich5_hpet;
		break;
	}

	pcibattach(parent, self, aux);

	/*
	 * Part of our I/O registers are used as ACPI PM regs.
	 * Since our ACPI subsystem accesses the I/O space directly so far,
	 * we do not have to bother bus_space I/O map confliction.
	 *
	 * The PMBASE register is alike PCI BAR but not completely compatible
	 * with it. The PMBASE define the base address and the type but
	 * not describe the size. The value of the register may be lower
	 * than LPCIB_PCI_PM_SIZE. It makes impossible to use
	 * pci_mapreg_submap() because the function does range check.
	 */
	sc->sc_iot = pa->pa_iot;
	pmbase = pci_conf_read(pa->pa_pc, pa->pa_tag, LPCIB_PCI_PMBASE);
	if (bus_space_map(sc->sc_iot, PCI_MAPREG_IO_ADDR(pmbase),
	    LPCIB_PCI_PM_SIZE, 0, &sc->sc_ioh) != 0) {
		aprint_error_dev(self,
	    	"can't map power management i/o space\n");
		return;
	}

	sc->sc_pmcon_orig = pci_conf_read(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
	    LPCIB_PCI_GEN_PMCON_1);

	/* For ICH6 and later, always enable RCBA */
	if (sc->sc_has_rcba) {
		pcireg_t rcba;

		sc->sc_rcbat = sc->sc_pa.pa_memt;

		rcba = pci_conf_read(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
		     LPCIB_RCBA);
		if ((rcba & LPCIB_RCBA_EN) == 0) {
			aprint_error_dev(self, "RCBA is not enabled\n");
			return;
		}
		rcba &= ~LPCIB_RCBA_EN;

		if (bus_space_map(sc->sc_rcbat, rcba, LPCIB_RCBA_SIZE, 0,
				  &sc->sc_rcbah)) {
			aprint_error_dev(self, "RCBA could not be mapped\n");
			return;
		}
	}

	/* Set up the power management timer. */
	pmtimer_configure(self);

	/* Set up the TCO (watchdog). */
	tcotimer_configure(self);

	/* Set up SpeedStep. */
	speedstep_configure(self);

	/* Set up HPET. */
	lpcib_hpet_configure(self);

#if NGPIO > 0
	/* Set up GPIO */
	lpcib_gpio_configure(self);
#endif

#if NFWHRNG > 0
	lpcib_fwh_configure(self);
#endif

	/* Install power handler */
	if (!pmf_device_register1(self, lpcib_suspend, lpcib_resume,
	    lpcib_shutdown))
		aprint_error_dev(self, "couldn't establish power handler\n");
}

static void
lpcibchilddet(device_t self, device_t child)
{
	struct lpcib_softc *sc = device_private(self);
	uint32_t val;

#if NFWHRNG > 0
	if (sc->sc_fwhbus == child) {
		sc->sc_fwhbus = NULL;
		return;
	}
#endif
#if NGPIO > 0
	if (sc->sc_gpiobus == child) {
		sc->sc_gpiobus = NULL;
		return;
	}
#endif
	if (sc->sc_tco == child) {
		sc->sc_tco = NULL;
		return;
	}

	if (sc->sc_hpetbus != child) {
		pcibchilddet(self, child);
		return;
	}
	sc->sc_hpetbus = NULL;
	if (sc->sc_has_ich5_hpet) {
		val = pci_conf_read(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
		    LPCIB_PCI_GEN_CNTL);
		switch (val & LPCIB_ICH5_HPTC_WIN_MASK) {
		case LPCIB_ICH5_HPTC_0000:
		case LPCIB_ICH5_HPTC_1000:
		case LPCIB_ICH5_HPTC_2000:
		case LPCIB_ICH5_HPTC_3000:
			break;
		default:
			return;
		}
		val &= ~LPCIB_ICH5_HPTC_EN;
		pci_conf_write(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
		    LPCIB_PCI_GEN_CNTL, val);
	} else if (sc->sc_has_rcba) {
		val = bus_space_read_4(sc->sc_rcbat, sc->sc_rcbah,
		    LPCIB_RCBA_HPTC);
		switch (val & LPCIB_RCBA_HPTC_WIN_MASK) {
		case LPCIB_RCBA_HPTC_0000:
		case LPCIB_RCBA_HPTC_1000:
		case LPCIB_RCBA_HPTC_2000:
		case LPCIB_RCBA_HPTC_3000:
			break;
		default:
			return;
		}
		val &= ~LPCIB_RCBA_HPTC_EN;
		bus_space_write_4(sc->sc_rcbat, sc->sc_rcbah, LPCIB_RCBA_HPTC,
		    val);
	}
}

static int
lpcibrescan(device_t self, const char *ifattr, const int *locators)
{
	struct lpcib_softc *sc = device_private(self);

	if(ifattr_match(ifattr, "tcoichbus") && sc->sc_tco == NULL)
		tcotimer_configure(self);

#if NFWHRNG > 0
	if (ifattr_match(ifattr, "fwhichbus") && sc->sc_fwhbus == NULL)
		lpcib_fwh_configure(self);
#endif

	if (ifattr_match(ifattr, "hpetichbus") && sc->sc_hpetbus == NULL)
		lpcib_hpet_configure(self);

#if NGPIO > 0
	if (ifattr_match(ifattr, "gpiobus") && sc->sc_gpiobus == NULL)
		lpcib_gpio_configure(self);
#endif

	return pcibrescan(self, ifattr, locators);
}

static int
lpcibdetach(device_t self, int flags)
{
	struct lpcib_softc *sc = device_private(self);
	int rc;

	pmf_device_deregister(self);

#if NFWHRNG > 0
	if ((rc = lpcib_fwh_unconfigure(self, flags)) != 0)
		return rc;
#endif

	if ((rc = lpcib_hpet_unconfigure(self, flags)) != 0)
		return rc;

#if NGPIO > 0
	if ((rc = lpcib_gpio_unconfigure(self, flags)) != 0)
		return rc;
#endif

	/* Set up SpeedStep. */
	speedstep_unconfigure(self);

	if ((rc = tcotimer_unconfigure(self, flags)) != 0)
		return rc;

	if ((rc = pmtimer_unconfigure(self, flags)) != 0)
		return rc;

	if (sc->sc_has_rcba)
		bus_space_unmap(sc->sc_rcbat, sc->sc_rcbah, LPCIB_RCBA_SIZE);

	bus_space_unmap(sc->sc_iot, sc->sc_ioh, sc->sc_iosize);

	return pcibdetach(self, flags);
}

static bool
lpcib_shutdown(device_t dv, int howto)
{
	struct lpcib_softc *sc = device_private(dv);

	pci_conf_write(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
	    LPCIB_PCI_GEN_PMCON_1, sc->sc_pmcon_orig);

	return true;
}

static bool
lpcib_suspend(device_t dv, const pmf_qual_t *qual)
{
	struct lpcib_softc *sc = device_private(dv);
	pci_chipset_tag_t pc = sc->sc_pcib.sc_pc;
	pcitag_t tag = sc->sc_pcib.sc_tag;

	/* capture PIRQ routing control registers */
	sc->sc_pirq[0] = pci_conf_read(pc, tag, LPCIB_PCI_PIRQA_ROUT);
	sc->sc_pirq[1] = pci_conf_read(pc, tag, LPCIB_PCI_PIRQE_ROUT);

	sc->sc_pmcon = pci_conf_read(pc, tag, LPCIB_PCI_GEN_PMCON_1);
	sc->sc_fwhsel2 = pci_conf_read(pc, tag, LPCIB_PCI_GEN_STA);

	if (sc->sc_has_rcba) {
		sc->sc_rcba_reg = pci_conf_read(pc, tag, LPCIB_RCBA);
		sc->sc_hpet_reg = bus_space_read_4(sc->sc_rcbat, sc->sc_rcbah,
		    LPCIB_RCBA_HPTC);
	} else if (sc->sc_has_ich5_hpet) {
		sc->sc_hpet_reg = pci_conf_read(pc, tag, LPCIB_PCI_GEN_CNTL);
	}

	return true;
}

static bool
lpcib_resume(device_t dv, const pmf_qual_t *qual)
{
	struct lpcib_softc *sc = device_private(dv);
	pci_chipset_tag_t pc = sc->sc_pcib.sc_pc;
	pcitag_t tag = sc->sc_pcib.sc_tag;

	/* restore PIRQ routing control registers */
	pci_conf_write(pc, tag, LPCIB_PCI_PIRQA_ROUT, sc->sc_pirq[0]);
	pci_conf_write(pc, tag, LPCIB_PCI_PIRQE_ROUT, sc->sc_pirq[1]);

	pci_conf_write(pc, tag, LPCIB_PCI_GEN_PMCON_1, sc->sc_pmcon);
	pci_conf_write(pc, tag, LPCIB_PCI_GEN_STA, sc->sc_fwhsel2);

	if (sc->sc_has_rcba) {
		pci_conf_write(pc, tag, LPCIB_RCBA, sc->sc_rcba_reg);
		bus_space_write_4(sc->sc_rcbat, sc->sc_rcbah, LPCIB_RCBA_HPTC,
		    sc->sc_hpet_reg);
	} else if (sc->sc_has_ich5_hpet) {
		pci_conf_write(pc, tag, LPCIB_PCI_GEN_CNTL, sc->sc_hpet_reg);
	}

	return true;
}

/*
 * Initialize the power management timer.
 */
static void
pmtimer_configure(device_t self)
{
	struct lpcib_softc *sc = device_private(self);
	pcireg_t control;

	/* 
	 * Check if power management I/O space is enabled and enable the ACPI_EN
	 * bit if it's disabled.
	 */
	control = pci_conf_read(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
	    LPCIB_PCI_ACPI_CNTL);
	sc->sc_acpi_cntl = control;
	if ((control & LPCIB_PCI_ACPI_CNTL_EN) == 0) {
		control |= LPCIB_PCI_ACPI_CNTL_EN;
		pci_conf_write(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
		    LPCIB_PCI_ACPI_CNTL, control);
	}

	/* Attach our PM timer with the generic acpipmtimer function */
	sc->sc_pmtimer = acpipmtimer_attach(self, sc->sc_iot, sc->sc_ioh,
	    LPCIB_PM1_TMR, 0);
}

static int
pmtimer_unconfigure(device_t self, int flags)
{
	struct lpcib_softc *sc = device_private(self);
	int rc;

	if (sc->sc_pmtimer != NULL &&
	    (rc = acpipmtimer_detach(sc->sc_pmtimer, flags)) != 0)
		return rc;

	pci_conf_write(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
	    LPCIB_PCI_ACPI_CNTL, sc->sc_acpi_cntl);

	return 0;
}

/*
 * Configure the watchdog timer.
 */
static void
tcotimer_configure(device_t self)
{
	struct lpcib_softc *sc = device_private(self);
	struct lpcib_tco_attach_args arg;

	arg.ta_iot = sc->sc_iot;
	arg.ta_ioh = sc->sc_ioh;
	arg.ta_rcbat = sc->sc_rcbat;
	arg.ta_rcbah = sc->sc_rcbah;
	arg.ta_has_rcba = sc->sc_has_rcba;
	arg.ta_pcib = &(sc->sc_pcib);

	sc->sc_tco = config_found_ia(self, "tcoichbus", &arg, NULL);
}

static int
tcotimer_unconfigure(device_t self, int flags)
{
	struct lpcib_softc *sc = device_private(self);
	int rc;

	if (sc->sc_tco != NULL &&
	    (rc = config_detach(sc->sc_tco, flags)) != 0)
		return rc;

	return 0;
}


/*
 * Intel ICH SpeedStep support.
 */
#define SS_READ(sc, reg) \
	bus_space_read_1((sc)->sc_iot, (sc)->sc_ioh, (reg))
#define SS_WRITE(sc, reg, val) \
	bus_space_write_1((sc)->sc_iot, (sc)->sc_ioh, (reg), (val))

/*
 * Linux driver says that SpeedStep on older chipsets cause
 * lockups on Dell Inspiron 8000 and 8100.
 * It should also not be enabled on systems with the 82855GM
 * Hub, which typically have an EST-enabled CPU.
 */
static int
speedstep_bad_hb_check(const struct pci_attach_args *pa)
{

	if (PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_INTEL_82815_FULL_HUB &&
	    PCI_REVISION(pa->pa_class) < 5)
		return 1;

	if (PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_INTEL_82855GM_MCH)
		return 1;

	return 0;
}

static void
speedstep_configure(device_t self)
{
	struct lpcib_softc *sc = device_private(self);
	const struct sysctlnode	*node, *ssnode;
	int rv;

	/* Supported on ICH2-M, ICH3-M and ICH4-M.  */
	if (PCI_PRODUCT(sc->sc_pa.pa_id) == PCI_PRODUCT_INTEL_82801DBM_LPC ||
	    PCI_PRODUCT(sc->sc_pa.pa_id) == PCI_PRODUCT_INTEL_82801CAM_LPC ||
	    (PCI_PRODUCT(sc->sc_pa.pa_id) == PCI_PRODUCT_INTEL_82801BAM_LPC &&
	     pci_find_device(&sc->sc_pa, speedstep_bad_hb_check) == 0)) {
		pcireg_t pmcon;

		/* Enable SpeedStep if it isn't already enabled. */
		pmcon = pci_conf_read(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
				      LPCIB_PCI_GEN_PMCON_1);
		if ((pmcon & LPCIB_PCI_GEN_PMCON_1_SS_EN) == 0)
			pci_conf_write(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
				       LPCIB_PCI_GEN_PMCON_1,
				       pmcon | LPCIB_PCI_GEN_PMCON_1_SS_EN);

		/* Put in machdep.speedstep_state (0 for low, 1 for high). */
		if ((rv = sysctl_createv(&sc->sc_log, 0, NULL, &node,
		    CTLFLAG_PERMANENT, CTLTYPE_NODE, "machdep", NULL,
		    NULL, 0, NULL, 0, CTL_MACHDEP, CTL_EOL)) != 0)
			goto err;

		/* CTLFLAG_ANYWRITE? kernel option like EST? */
		if ((rv = sysctl_createv(&sc->sc_log, 0, &node, &ssnode,
		    CTLFLAG_READWRITE, CTLTYPE_INT, "speedstep_state", NULL,
		    speedstep_sysctl_helper, 0, NULL, 0, CTL_CREATE,
		    CTL_EOL)) != 0)
			goto err;

		/* XXX save the sc for IO tag/handle */
		speedstep_cookie = sc;
		aprint_verbose_dev(self, "SpeedStep enabled\n");
	}

	return;

err:
	aprint_normal("%s: sysctl_createv failed (rv = %d)\n", __func__, rv);
}

static void
speedstep_unconfigure(device_t self)
{
	struct lpcib_softc *sc = device_private(self);

	sysctl_teardown(&sc->sc_log);
	pci_conf_write(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
	    LPCIB_PCI_GEN_PMCON_1, sc->sc_pmcon_orig);

	speedstep_cookie = NULL;
}

/*
 * get/set the SpeedStep state: 0 == low power, 1 == high power.
 */
static int
speedstep_sysctl_helper(SYSCTLFN_ARGS)
{
	struct sysctlnode	node;
	struct lpcib_softc 	*sc = speedstep_cookie;
	uint8_t			state, state2;
	int			ostate, nstate, s, error = 0;

	/*
	 * We do the dance with spl's to avoid being at high ipl during
	 * sysctl_lookup() which can both copyin and copyout.
	 */
	s = splserial();
	state = SS_READ(sc, LPCIB_PM_SS_CNTL);
	splx(s);
	if ((state & LPCIB_PM_SS_STATE_LOW) == 0)
		ostate = 1;
	else
		ostate = 0;
	nstate = ostate;

	node = *rnode;
	node.sysctl_data = &nstate;

	error = sysctl_lookup(SYSCTLFN_CALL(&node));
	if (error || newp == NULL)
		goto out;

	/* Only two states are available */
	if (nstate != 0 && nstate != 1) {
		error = EINVAL;
		goto out;
	}

	s = splserial();
	state2 = SS_READ(sc, LPCIB_PM_SS_CNTL);
	if ((state2 & LPCIB_PM_SS_STATE_LOW) == 0)
		ostate = 1;
	else
		ostate = 0;

	if (ostate != nstate) {
		uint8_t cntl;

		if (nstate == 0)
			state2 |= LPCIB_PM_SS_STATE_LOW;
		else
			state2 &= ~LPCIB_PM_SS_STATE_LOW;

		/*
		 * Must disable bus master arbitration during the change.
		 */
		cntl = SS_READ(sc, LPCIB_PM_CTRL);
		SS_WRITE(sc, LPCIB_PM_CTRL, cntl | LPCIB_PM_SS_CNTL_ARB_DIS);
		SS_WRITE(sc, LPCIB_PM_SS_CNTL, state2);
		SS_WRITE(sc, LPCIB_PM_CTRL, cntl);
	}
	splx(s);
out:
	return error;
}

static void
lpcib_hpet_configure(device_t self)
{
	struct lpcib_softc *sc = device_private(self);
	struct lpcib_hpet_attach_args arg;
	uint32_t hpet_reg, val;

	if (sc->sc_has_ich5_hpet) {
		val = pci_conf_read(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
		    LPCIB_PCI_GEN_CNTL);
		switch (val & LPCIB_ICH5_HPTC_WIN_MASK) {
		case LPCIB_ICH5_HPTC_0000:
			hpet_reg = LPCIB_ICH5_HPTC_0000_BASE;
			break;
		case LPCIB_ICH5_HPTC_1000:
			hpet_reg = LPCIB_ICH5_HPTC_1000_BASE;
			break;
		case LPCIB_ICH5_HPTC_2000:
			hpet_reg = LPCIB_ICH5_HPTC_2000_BASE;
			break;
		case LPCIB_ICH5_HPTC_3000:
			hpet_reg = LPCIB_ICH5_HPTC_3000_BASE;
			break;
		default:
			return;
		}
		val |= sc->sc_hpet_reg | LPCIB_ICH5_HPTC_EN;
		pci_conf_write(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
		    LPCIB_PCI_GEN_CNTL, val);
	} else if (sc->sc_has_rcba) {
		val = bus_space_read_4(sc->sc_rcbat, sc->sc_rcbah,
		    LPCIB_RCBA_HPTC);
		switch (val & LPCIB_RCBA_HPTC_WIN_MASK) {
		case LPCIB_RCBA_HPTC_0000:
			hpet_reg = LPCIB_RCBA_HPTC_0000_BASE;
			break;
		case LPCIB_RCBA_HPTC_1000:
			hpet_reg = LPCIB_RCBA_HPTC_1000_BASE;
			break;
		case LPCIB_RCBA_HPTC_2000:
			hpet_reg = LPCIB_RCBA_HPTC_2000_BASE;
			break;
		case LPCIB_RCBA_HPTC_3000:
			hpet_reg = LPCIB_RCBA_HPTC_3000_BASE;
			break;
		default:
			return;
		}
		val |= LPCIB_RCBA_HPTC_EN;
		bus_space_write_4(sc->sc_rcbat, sc->sc_rcbah, LPCIB_RCBA_HPTC,
		    val);
	} else {
		/* No HPET here */
		return;
	}

	arg.hpet_mem_t = sc->sc_pa.pa_memt;
	arg.hpet_reg = hpet_reg;

	sc->sc_hpetbus = config_found_ia(self, "hpetichbus", &arg, NULL);
}

static int
lpcib_hpet_unconfigure(device_t self, int flags)
{
	struct lpcib_softc *sc = device_private(self);
	int rc;

	if (sc->sc_hpetbus != NULL &&
	    (rc = config_detach(sc->sc_hpetbus, flags)) != 0)
		return rc;

	return 0;
}

#if NGPIO > 0
static void
lpcib_gpio_configure(device_t self)
{
	struct lpcib_softc *sc = device_private(self);
	struct gpiobus_attach_args gba;
	pcireg_t gpio_cntl;
	uint32_t use, io, bit;
	int pin, shift, base_reg, cntl_reg, reg;
	int rv;

	/* this implies ICH >= 6, and thus different mapreg */
	if (sc->sc_has_rcba) {
		base_reg = LPCIB_PCI_GPIO_BASE_ICH6;
		cntl_reg = LPCIB_PCI_GPIO_CNTL_ICH6;
	} else {
		base_reg = LPCIB_PCI_GPIO_BASE;
		cntl_reg = LPCIB_PCI_GPIO_CNTL;
	}

	gpio_cntl = pci_conf_read(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
				  cntl_reg);

	/* Is GPIO enabled? */
	if ((gpio_cntl & LPCIB_PCI_GPIO_CNTL_EN) == 0)
		return;
	/*
	 * The GPIO_BASE register is alike PCI BAR but not completely
	 * compatible with it. The PMBASE define the base address and the type
	 * but not describe the size. The value of the register may be lower
	 * than LPCIB_PCI_GPIO_SIZE. It makes impossible to use
	 * pci_mapreg_submap() because the function does range check.
	 */
	sc->sc_gpio_iot = sc->sc_pa.pa_iot;
	reg = pci_conf_read(sc->sc_pa.pa_pc, sc->sc_pa.pa_tag, base_reg);
	rv = bus_space_map(sc->sc_gpio_iot, PCI_MAPREG_IO_ADDR(reg),
	    LPCIB_PCI_GPIO_SIZE, 0, &sc->sc_gpio_ioh);
	if (rv != 0) {
		aprint_error_dev(self, "can't map general purpose i/o space(rv = %d)\n", rv);
		return;
	}

	mutex_init(&sc->sc_gpio_mtx, MUTEX_DEFAULT, IPL_NONE);

	for (pin = 0; pin < LPCIB_GPIO_NPINS; pin++) {
		sc->sc_gpio_pins[pin].pin_num = pin;

		/* Read initial state */
		reg = (pin < 32) ? LPCIB_GPIO_GPIO_USE_SEL : LPCIB_GPIO_GPIO_USE_SEL2;
		use = bus_space_read_4(sc->sc_gpio_iot, sc->sc_gpio_ioh, reg);
		reg = (pin < 32) ? LPCIB_GPIO_GP_IO_SEL : LPCIB_GPIO_GP_IO_SEL;
		io = bus_space_read_4(sc->sc_gpio_iot, sc->sc_gpio_ioh, 4);
		shift = pin % 32;
		bit = __BIT(shift);

		if ((use & bit) != 0) {
			sc->sc_gpio_pins[pin].pin_caps =
			    GPIO_PIN_INPUT | GPIO_PIN_OUTPUT;
			if (pin < 32)
				sc->sc_gpio_pins[pin].pin_caps |=
				    GPIO_PIN_PULSATE;
			if ((io & bit) != 0)
				sc->sc_gpio_pins[pin].pin_flags =
				    GPIO_PIN_INPUT;
			else
				sc->sc_gpio_pins[pin].pin_flags =
				    GPIO_PIN_OUTPUT;
		} else
			sc->sc_gpio_pins[pin].pin_caps = 0;

		if (lpcib_gpio_pin_read(sc, pin) == 0)
			sc->sc_gpio_pins[pin].pin_state = GPIO_PIN_LOW;
		else
			sc->sc_gpio_pins[pin].pin_state = GPIO_PIN_HIGH;

	}

	/* Create controller tag */
	sc->sc_gpio_gc.gp_cookie = sc;
	sc->sc_gpio_gc.gp_pin_read = lpcib_gpio_pin_read;
	sc->sc_gpio_gc.gp_pin_write = lpcib_gpio_pin_write;
	sc->sc_gpio_gc.gp_pin_ctl = lpcib_gpio_pin_ctl;

	memset(&gba, 0, sizeof(gba));

	gba.gba_gc = &sc->sc_gpio_gc;
	gba.gba_pins = sc->sc_gpio_pins;
	gba.gba_npins = LPCIB_GPIO_NPINS;

	sc->sc_gpiobus = config_found_ia(self, "gpiobus", &gba, gpiobus_print);
}

static int
lpcib_gpio_unconfigure(device_t self, int flags)
{
	struct lpcib_softc *sc = device_private(self);
	int rc;

	if (sc->sc_gpiobus != NULL &&
	    (rc = config_detach(sc->sc_gpiobus, flags)) != 0)
		return rc;

	mutex_destroy(&sc->sc_gpio_mtx);

	bus_space_unmap(sc->sc_gpio_iot, sc->sc_gpio_ioh, sc->sc_gpio_ios);

	return 0;
}

static int
lpcib_gpio_pin_read(void *arg, int pin)
{
	struct lpcib_softc *sc = arg;
	uint32_t data;
	int reg, shift;
	
	reg = (pin < 32) ? LPCIB_GPIO_GP_LVL : LPCIB_GPIO_GP_LVL2;
	shift = pin % 32;

	mutex_enter(&sc->sc_gpio_mtx);
	data = bus_space_read_4(sc->sc_gpio_iot, sc->sc_gpio_ioh, reg);
	mutex_exit(&sc->sc_gpio_mtx);
	
	return (__SHIFTOUT(data, __BIT(shift)) ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
}

static void
lpcib_gpio_pin_write(void *arg, int pin, int value)
{
	struct lpcib_softc *sc = arg;
	uint32_t data;
	int reg, shift;

	reg = (pin < 32) ? LPCIB_GPIO_GP_LVL : LPCIB_GPIO_GP_LVL2;
	shift = pin % 32;

	mutex_enter(&sc->sc_gpio_mtx);

	data = bus_space_read_4(sc->sc_gpio_iot, sc->sc_gpio_ioh, reg);

	if(value)
		data |= __BIT(shift);
	else
		data &= ~__BIT(shift);

	bus_space_write_4(sc->sc_gpio_iot, sc->sc_gpio_ioh, reg, data);

	mutex_exit(&sc->sc_gpio_mtx);
}

static void
lpcib_gpio_pin_ctl(void *arg, int pin, int flags)
{
	struct lpcib_softc *sc = arg;
	uint32_t data;
	int reg, shift;

	shift = pin % 32;
	reg = (pin < 32) ? LPCIB_GPIO_GP_IO_SEL : LPCIB_GPIO_GP_IO_SEL2;
	
	mutex_enter(&sc->sc_gpio_mtx);
	
	data = bus_space_read_4(sc->sc_gpio_iot, sc->sc_gpio_ioh, reg);
	
	if (flags & GPIO_PIN_OUTPUT)
		data &= ~__BIT(shift);

	if (flags & GPIO_PIN_INPUT)
		data |= __BIT(shift);

	bus_space_write_4(sc->sc_gpio_iot, sc->sc_gpio_ioh, reg, data);


	if (pin < 32) {
		reg = LPCIB_GPIO_GPO_BLINK;
		data = bus_space_read_4(sc->sc_gpio_iot, sc->sc_gpio_ioh, reg);

		if (flags & GPIO_PIN_PULSATE)
			data |= __BIT(shift);
		else
			data &= ~__BIT(shift);

		bus_space_write_4(sc->sc_gpio_iot, sc->sc_gpio_ioh, reg, data);
	}

	mutex_exit(&sc->sc_gpio_mtx);
}
#endif

#if NFWHRNG > 0
static void
lpcib_fwh_configure(device_t self)
{
	struct lpcib_softc *sc;
	pcireg_t pr;

	sc = device_private(self);

	if (sc->sc_has_rcba) {
		/*
		 * Very unlikely to find a 82802 on a ICH6 or newer.
		 * Also the write enable register moved at that point.
		 */
		return;
	} else {
		/* Enable FWH write to identify FWH. */
		pr = pci_conf_read(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
		    LPCIB_PCI_BIOS_CNTL);
		pci_conf_write(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
		    LPCIB_PCI_BIOS_CNTL, pr|LPCIB_PCI_BIOS_CNTL_BWE);
	}

	sc->sc_fwhbus = config_found_ia(self, "fwhichbus", NULL, NULL);

	/* restore previous write enable setting */
	pci_conf_write(sc->sc_pcib.sc_pc, sc->sc_pcib.sc_tag,
	    LPCIB_PCI_BIOS_CNTL, pr);
}

static int
lpcib_fwh_unconfigure(device_t self, int flags)
{
	struct lpcib_softc *sc = device_private(self);
	int rc;

	if (sc->sc_fwhbus != NULL &&
	    (rc = config_detach(sc->sc_fwhbus, flags)) != 0)
		return rc;

	return 0;
}
#endif
