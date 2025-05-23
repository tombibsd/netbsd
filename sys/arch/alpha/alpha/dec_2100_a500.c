/* $NetBSD$ */

/*-
 * Copyright (c) 1999 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe.
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
 * Copyright (c) 1995, 1996, 1997 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Chris G. Demetriou
 *
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

#include "opt_kgdb.h"

#include <sys/cdefs.h>			/* RCS ID & Copyright macro defns */

__KERNEL_RCSID(0, "$NetBSD$");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/termios.h>
#include <sys/conf.h>
#include <dev/cons.h>

#include <machine/rpb.h>
#include <machine/autoconf.h>
#include <machine/cpuconf.h>
#include <sys/bus.h>
#include <machine/alpha.h>

#include <dev/ic/comreg.h>
#include <dev/ic/comvar.h>

#include <dev/isa/isareg.h>
#include <dev/isa/isavar.h>
#include <dev/ic/i8042reg.h>
#include <dev/ic/pckbcvar.h>
#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>

#include <alpha/pci/ttwogareg.h>
#include <alpha/pci/ttwogavar.h>

#include <dev/scsipi/scsi_all.h>
#include <dev/scsipi/scsipi_all.h>
#include <dev/scsipi/scsiconf.h>

#include <dev/ic/mlxio.h>
#include <dev/ic/mlxvar.h>

#include "pckbd.h"

#ifndef CONSPEED
#define CONSPEED TTYDEF_SPEED
#endif
static int comcnrate = CONSPEED;

void _dec_2100_a500_init(void);
static void dec_2100_a500_cons_init(void);
static void dec_2100_a500_device_register(device_t, void *);
static void dec_2100_a500_machine_check(unsigned long, struct trapframe *,
	unsigned long, unsigned long);

#ifdef KGDB
#include <machine/db_machdep.h>

static const char *kgdb_devlist[] = {
	"com",
	NULL,
};
#endif /* KGDB */

void
_dec_2100_a500_init(void)
{

	switch (cputype) {
	case ST_DEC_2100_A500:
		if (alpha_implver() == ALPHA_IMPLVER_EV5)
			platform.family = "AlphaServer 2100 (\"Sable-Gamma\")";
		else
			platform.family = "AlphaServer 2100 (\"Sable\")";
		break;

	case ST_DEC_2100A_A500:
		platform.family = "AlphaServer 2100A (\"Lynx\")";
		break;

	default:
		panic("dec_2100_a500_init: Not a Sable, Sable-Gamma, or Lynx?");
	}

	if ((platform.model = alpha_dsr_sysname()) == NULL) {
		/* XXX don't know variations yet */
		platform.model = alpha_unknown_sysname();
	}

	platform.iobus = "ttwoga";
	platform.cons_init = dec_2100_a500_cons_init;
	platform.device_register = dec_2100_a500_device_register;
	platform.mcheck_handler = dec_2100_a500_machine_check;
}

static void
dec_2100_a500_cons_init(void)
{
	struct ctb *ctb;
	uint64_t ctbslot;
	struct ttwoga_config *tcp;

	ctb = (struct ctb *)(((char *)hwrpb) + hwrpb->rpb_ctb_off);
	ctbslot = ctb->ctb_turboslot;

	tcp = ttwoga_init(0, 0);

	switch (ctb->ctb_term_type) {
	case CTB_PRINTERPORT:
		/* serial console ... */
		assert(CTB_TURBOSLOT_HOSE(ctbslot) == 0);
		/* XXX */
		{
			/*
			 * Delay to allow PROM putchars to complete.
			 * FIFO depth * character time,
			 * character time = (1000000 / (defaultrate / 10))
			 */
			DELAY(160000000 / comcnrate);

			if(comcnattach(&tcp->tc_iot, 0x3f8, comcnrate,
			    COM_FREQ, COM_TYPE_NORMAL,
			    (TTYDEF_CFLAG & ~(CSIZE | PARENB)) | CS8))
				panic("can't init serial console");

			break;
		}

	case CTB_GRAPHICS:
#if NPCKBD > 0
		/* display console ... */
		/* XXX */
		(void) pckbc_cnattach(&tcp->tc_iot, IO_KBD, KBCMDP,
		    PCKBC_KBD_SLOT, 0);

		switch (CTB_TURBOSLOT_TYPE(ctbslot)) {
		case CTB_TURBOSLOT_TYPE_ISA:
		/*
		 * XXX The following is apparently necessary for an
		 * XXX ISA VGA card, but what happens with real EISA
		 * XXX VGA cards?
		 */
		case CTB_TURBOSLOT_TYPE_EISA:
			isa_display_console(&tcp->tc_iot, &tcp->tc_memt);
			break;

		case CTB_TURBOSLOT_TYPE_PCI:
			/* The display PCI might be different */
			tcp = ttwoga_init(0, CTB_TURBOSLOT_HOSE(ctbslot));
			pci_display_console(&tcp->tc_iot, &tcp->tc_memt,
			    &tcp->tc_pc, CTB_TURBOSLOT_BUS(ctbslot),
			    CTB_TURBOSLOT_SLOT(ctbslot), 0);
			break;

		default:
			printf("type = 0x%lx, hose = 0x%lx, bus = 0x%lx, "
			    "slot = 0x%lx\n",
			    CTB_TURBOSLOT_TYPE(ctbslot),
			    CTB_TURBOSLOT_HOSE(ctbslot),
			    CTB_TURBOSLOT_BUS(ctbslot),
			    CTB_TURBOSLOT_SLOT(ctbslot));
			panic("unknown display console type");
		}
#else
		panic("not configured to use display && keyboard console");
#endif
		break;

	default:
		printf("ctb->ctb_term_type = 0x%lx\n", ctb->ctb_term_type);
		printf("ctb->ctb_turboslot = 0x%lx\n", ctb->ctb_turboslot);

		panic("consinit: unknown console type %ld",
		    ctb->ctb_term_type);
	}
#ifdef KGDB
	/* Attach the KGDB device. */
	alpha_kgdb_init(kgdb_devlist, &tcp->tc_iot);
#endif /* KGDB */
}

static void
dec_2100_a500_device_register(device_t dev, void *aux)
{
	static int found, initted, diskboot, netboot;
	static device_t pcidev, ctrlrdev;
	struct bootdev_data *b = bootdev_data;
	device_t parent = device_parent(dev);

	if (found)
		return;

	if (!initted) {
		diskboot = (strcasecmp(b->protocol, "SCSI") == 0) ||
		    (strcasecmp(b->protocol, "RAID") == 0);
		netboot = (strcasecmp(b->protocol, "BOOTP") == 0) ||
		    (strcasecmp(b->protocol, "MOP") == 0);
#if 0
		printf("diskboot = %d, netboot = %d\n", diskboot, netboot);
#endif
		initted =1;
	}

	if (pcidev == NULL) {
		if (!device_is_a(dev, "pci"))
			return;
		else {
			struct pcibus_attach_args *pba = aux;

			if ((b->slot / 1000) != pba->pba_bus)
				return;
	
			pcidev = dev;
#if 0
			printf("\npcidev = %s\n", device_xname(dev));
#endif
			return;
		}
	}

	if (ctrlrdev == NULL) {
		if (parent != pcidev)
			return;
		else {
			struct pci_attach_args *pa = aux;
			int slot;

			slot = pa->pa_bus * 1000 + pa->pa_function * 100 +
			    pa->pa_device;
			if (b->slot != slot)
				return;
	
			if (netboot) {
				booted_device = dev;
#if 0
				printf("\nbooted_device = %s\n", device_xname(dev));
#endif
				found = 1;
			} else {
				ctrlrdev = dev;
#if 0
				printf("\nctrlrdev = %s\n", device_xname(dev));
#endif
			}
			return;
		}
	}

	if (!diskboot)
		return;

	if (device_is_a(dev, "sd") ||
	    device_is_a(dev, "st") ||
	    device_is_a(dev, "cd")) {
		struct scsipibus_attach_args *sa = aux;
		struct scsipi_periph *periph = sa->sa_periph;
		int unit;

		if (device_parent(parent) != ctrlrdev)
			return;

		unit = periph->periph_target * 100 + periph->periph_lun;
		if (b->unit != unit)
			return;
		if (b->channel != periph->periph_channel->chan_channel)
			return;

		/* we've found it! */
		booted_device = dev;
#if 0
		printf("\nbooted_device = %s\n", device_xname(dev));
#endif
		found = 1;
	}

	if (device_is_a(dev, "ld") && device_is_a(parent, "mlx")) {
		/*
		 * Argh!  The attach arguments for ld devices is not
		 * consistent, so each supported raid controller requires
		 * different checks.
		 */
		struct mlx_attach_args *mlxa = aux;

		if (parent != ctrlrdev)
			return;

		if (b->unit != mlxa->mlxa_unit)
			return;
		/* we've found it! */
		booted_device = dev;
#if 0
		printf("\nbooted_device = %s\n", device_xname(dev));
#endif
		found = 1;
	}
}

/*
 * Sable, Sable-Gamma, and Lynx machine check handlers.
 */

static void
dec_2100_a500_machine_check(unsigned long mces, struct trapframe *framep,
    unsigned long vector, unsigned long param)
{
	struct mchkinfo *mcp = &curcpu()->ci_mcinfo;

	/*
	 * This is a work-around for a T2 core logic bug.  See
	 * alpha/pci/ttwoga_pci.c.
	 */

	if (ttwoga_conf_cpu == cpu_number())
		mcp->mc_expected = 1;

	machine_check(mces, framep, vector, param);
}
