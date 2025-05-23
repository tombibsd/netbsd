/*	$NetBSD$	*/

/* adapted from:
 *	NetBSD: sdp24xx_machdep.c,v 1.4 2008/08/27 11:03:10 matt Exp
 */

/*
 * Machine dependent functions for kernel setup for TI OSK5912 board.
 * Based on lubbock_machdep.c which in turn was based on iq80310_machhdep.c
 *
 * Copyright (c) 2002, 2003, 2005  Genetec Corporation.  All rights reserved.
 * Written by Hiroyuki Bessho for Genetec Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of Genetec Corporation may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GENETEC CORPORATION ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL GENETEC CORPORATION
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright (c) 2001 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Jason R. Thorpe for Wasabi Systems, Inc.
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
 *	This product includes software developed for the NetBSD Project by
 *	Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright (c) 1997,1998 Mark Brinicombe.
 * Copyright (c) 1997,1998 Causality Limited.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Mark Brinicombe
 *	for the NetBSD Project.
 * 4. The name of the company nor the name of the author may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Copyright (c) 2007 Microsoft
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Microsoft
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTERS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include "opt_machdep.h"
#include "opt_ddb.h"
#include "opt_kgdb.h"
#include "opt_ipkdb.h"
#include "opt_md.h"
#include "opt_com.h"
#include "opt_gemini.h"
#include "geminiwdt.h"
#include "geminiipm.h"

#include <sys/param.h>
#include <sys/device.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/exec.h>
#include <sys/proc.h>
#include <sys/msgbuf.h>
#include <sys/reboot.h>
#include <sys/termios.h>
#include <sys/ksyms.h>
#include <sys/bus.h>
#include <sys/cpu.h>
#include <sys/conf.h>

#include <uvm/uvm_extern.h>

#include <dev/cons.h>
#include <dev/md.h>

#include <machine/db_machdep.h>
#include <ddb/db_sym.h>
#include <ddb/db_extern.h>
#ifdef KGDB
#include <sys/kgdb.h>
#endif

#include <arm/locore.h>
#include <arm/undefined.h>

#include <arm/arm32/machdep.h>

#include <machine/bootconfig.h>

#include <arm/gemini/gemini_reg.h>
#include <arm/gemini/gemini_var.h>
#include <arm/gemini/gemini_wdtvar.h>
#include <arm/gemini/gemini_com.h>
#include <arm/gemini/lpc_com.h>

#include <evbarm/gemini/gemini.h>

#if defined(VERBOSE_INIT_ARM)
# define GEMINI_PUTCHAR(c)	gemini_putchar(c)
# define GEMINI_PUTHEX(n)	gemini_puthex(n)
#else	/* VERBOSE_INIT_ARM */
# define GEMINI_PUTCHAR(c)
# define GEMINI_PUTHEX(n)
#endif	/* VERBOSE_INIT_ARM */

BootConfig bootconfig;		/* Boot config storage */
char *boot_args = NULL;
char *boot_file = NULL;

/* Physical address of the beginning of SDRAM. */
paddr_t physical_start;
/* Physical address of the first byte after the end of SDRAM. */
paddr_t physical_end;

/* Same things, but for the free (unused by the kernel) memory. */
static paddr_t physical_freestart, physical_freeend;
static u_int free_pages;

/* Physical address of the message buffer. */
paddr_t msgbufphys;

extern char KERNEL_BASE_phys[];
extern char KERNEL_BASE_virt[];
extern char etext[], __data_start[], _edata[], __bss_start[], __bss_end__[];
extern char _end[];

#define KERNEL_PT_SYS		0	/* Page table for mapping proc0 zero page */
#define KERNEL_PT_KERNEL	1	/* Page table for mapping kernel */
#define	KERNEL_PT_KERNEL_NUM	4
#define KERNEL_PT_VMDATA	(KERNEL_PT_KERNEL+KERNEL_PT_KERNEL_NUM)
				        /* Page tables for mapping kernel VM */
#define	KERNEL_PT_VMDATA_NUM	4	/* start with 16MB of KVM */
#define NUM_KERNEL_PTS		(KERNEL_PT_VMDATA + KERNEL_PT_VMDATA_NUM)

pv_addr_t kernel_pt_table[NUM_KERNEL_PTS];


#if (NGEMINIIPM > 0)
pv_addr_t ipmq_pt;		/* L2 Page table for mapping IPM queues */
#if defined(DEBUG) || 1
unsigned long gemini_ipmq_pbase = GEMINI_IPMQ_PBASE;
unsigned long gemini_ipmq_vbase = GEMINI_IPMQ_VBASE;
#endif	/* DEBUG */
#endif	/* NGEMINIIPM > 0 */


/*
 * Macros to translate between physical and virtual for a subset of the
 * kernel address space.  *Not* for general use.
 */
#define KERNEL_BASE_PHYS ((paddr_t)&KERNEL_BASE_phys)

#define KERN_VTOPHYS(va) \
	((paddr_t)((vaddr_t)va - KERNEL_BASE + GEMINI_DRAM_BASE))
#define KERN_PHYSTOV(pa) \
	((vaddr_t)((paddr_t)pa - GEMINI_DRAM_BASE + KERNEL_BASE))

/* Prototypes */

void gemini_intr_init(bus_space_tag_t);
void consinit(void);
#ifdef KGDB
static void kgdb_port_init(void);
#endif

static void setup_real_page_tables(void);
static void init_clocks(void);

bs_protos(bs_notimpl);

#include "com.h"
#if NCOM > 0
#include <dev/ic/comreg.h>
#include <dev/ic/comvar.h>
#endif


static void gemini_global_reset(void) __attribute__ ((noreturn));
static void gemini_cpu1_start(void);
static void gemini_memchk(void);

static void
gemini_global_reset(void)
{
#if defined(GEMINI_MASTER) || defined(GEMINI_SINGLE)
	volatile uint32_t *rp;
	uint32_t r;

	rp = (volatile uint32_t *)
		(GEMINI_GLOBAL_VBASE + GEMINI_GLOBAL_RESET_CTL);
	r = *rp;
	r |= GLOBAL_RESET_GLOBAL;
	*rp = r;
#endif
	for(;;);
	/* NOTREACHED */
}

static void
gemini_cpu1_start(void)
{
#ifdef GEMINI_MASTER
	volatile uint32_t *rp;
	uint32_t r;

	rp = (volatile uint32_t *)
		(GEMINI_GLOBAL_VBASE + GEMINI_GLOBAL_RESET_CTL);
	r = *rp;
	r &= ~GLOBAL_RESET_CPU1;
	*rp = r;
#endif
}

static void
gemini_memchk(void)
{
	volatile uint32_t *rp;
	uint32_t r;
	uint32_t base;
	uint32_t size;

	rp = (volatile uint32_t *)
		(GEMINI_DRAMC_VBASE + GEMINI_DRAMC_RMCR);
	r = *rp;
	base = (r & DRAMC_RMCR_RMBAR) >> DRAMC_RMCR_RMBAR_SHFT;
	size = (r & DRAMC_RMCR_RMSZR) >> DRAMC_RMCR_RMSZR_SHFT;
#if defined(GEMINI_SINGLE)
	if (r != 0)
		panic("%s: RMCR %#x, MEMSIZE %d mismatch\n",
			__FUNCTION__, r, MEMSIZE);
#elif defined(GEMINI_MASTER)
	if (base != MEMSIZE)
		panic("%s: RMCR %#x, MEMSIZE %d mismatch\n",
			__FUNCTION__, r, MEMSIZE);
#elif defined(GEMINI_SLAVE)
	if (size != MEMSIZE)
		panic("%s: RMCR %#x, MEMSIZE %d mismatch\n",
			__FUNCTION__, r, MEMSIZE);
#endif
#if defined(VERBOSE_INIT_ARM) || 1
	printf("DRAM Remap: base=%dMB, size=%dMB\n", base, size);
#endif
}

/*
 * void cpu_reboot(int howto, char *bootstr)
 *
 * Reboots the system
 *
 * Deal with any syncing, unmounting, dumping and shutdown hooks,
 * then reset the CPU.
 */
void
cpu_reboot(int howto, char *bootstr)
{
	extern struct geminitmr_softc *ref_sc;

#ifdef DIAGNOSTIC
	/* info */
	printf("boot: howto=%08x curproc=%p\n", howto, curproc);
#endif

	/*
	 * If we are still cold then hit the air brakes
	 * and crash to earth fast
	 */
	if (cold) {
		doshutdownhooks();
		pmf_system_shutdown(boothowto);
		printf("The operating system has halted.\n");
		printf("Please press any key to reboot.\n\n");
		cngetc();
		printf("rebooting...\n");
		if (ref_sc != NULL)
			delay(2000);			/* cnflush(); */
		gemini_global_reset();
		/*NOTREACHED*/
	}

	/* Disable console buffering */
	cnpollc(1);

	/*
	 * If RB_NOSYNC was not specified sync the discs.
	 * Note: Unless cold is set to 1 here, syslogd will die during the
	 * unmount.  It looks like syslogd is getting woken up only to find
	 * that it cannot page part of the binary in as the filesystem has
	 * been unmounted.
	 */
	if (!(howto & RB_NOSYNC))
		bootsync();

	/* Say NO to interrupts */
	splhigh();

	/* Do a dump if requested. */
	if ((howto & (RB_DUMP | RB_HALT)) == RB_DUMP)
		dumpsys();

	/* Run any shutdown hooks */
	doshutdownhooks();

	pmf_system_shutdown(boothowto);

	/* Make sure IRQ's are disabled */
	IRQdisable;

	if (howto & RB_HALT) {
		printf("The operating system has halted.\n");
		printf("Please press any key to reboot.\n\n");
		cngetc();
	}

	printf("rebooting...\n");
	if (ref_sc != NULL)
		delay(2000);			/* cnflush(); */
	gemini_global_reset();
	/*NOTREACHED*/
}

/*
 * Static device mappings. These peripheral registers are mapped at
 * fixed virtual addresses very early in initarm() so that we can use
 * them while booting the kernel, and stay at the same address
 * throughout whole kernel's life time.
 *
 * We use this table twice; once with bootstrap page table, and once
 * with kernel's page table which we build up in initarm().
 *
 * Since we map these registers into the bootstrap page table using
 * pmap_devmap_bootstrap() which calls pmap_map_chunk(), we map
 * registers segment-aligned and segment-rounded in order to avoid
 * using the 2nd page tables.
 */

#define	_A(a)	((a) & ~L1_S_OFFSET)
#define	_S(s)	(((s) + L1_S_SIZE - 1) & ~(L1_S_SIZE-1))

static const struct pmap_devmap devmap[] = {
	/* Global regs */
	{
		.pd_va = _A(GEMINI_GLOBAL_VBASE),
		.pd_pa = _A(GEMINI_GLOBAL_BASE),
		.pd_size = _S(L1_S_SIZE),
		.pd_prot = VM_PROT_READ|VM_PROT_WRITE,
		.pd_cache = PTE_NOCACHE
	},

	/* Watchdog */
	{
		.pd_va = _A(GEMINI_WATCHDOG_VBASE),
		.pd_pa = _A(GEMINI_WATCHDOG_BASE),
		.pd_size = _S(L1_S_SIZE),
		.pd_prot = VM_PROT_READ|VM_PROT_WRITE,
		.pd_cache = PTE_NOCACHE
	},

	/* UART */
	{
		.pd_va = _A(GEMINI_UART_VBASE),
		.pd_pa = _A(GEMINI_UART_BASE),
		.pd_size = _S(L1_S_SIZE),
		.pd_prot = VM_PROT_READ|VM_PROT_WRITE,
		.pd_cache = PTE_NOCACHE
	},

	/* LPCHC */
	{
		.pd_va = _A(GEMINI_LPCHC_VBASE),
		.pd_pa = _A(GEMINI_LPCHC_BASE),
		.pd_size = _S(L1_S_SIZE),
		.pd_prot = VM_PROT_READ|VM_PROT_WRITE,
		.pd_cache = PTE_NOCACHE
	},

	/* LPCIO */
	{
		.pd_va = _A(GEMINI_LPCIO_VBASE),
		.pd_pa = _A(GEMINI_LPCIO_BASE),
		.pd_size = _S(L1_S_SIZE),
		.pd_prot = VM_PROT_READ|VM_PROT_WRITE,
		.pd_cache = PTE_NOCACHE
	},

	/* Timers */
	{
		.pd_va = _A(GEMINI_TIMER_VBASE),
		.pd_pa = _A(GEMINI_TIMER_BASE),
		.pd_size = _S(L1_S_SIZE),
		.pd_prot = VM_PROT_READ|VM_PROT_WRITE,
		.pd_cache = PTE_NOCACHE
	},

	/* DRAM Controller */
	{
		.pd_va = _A(GEMINI_DRAMC_VBASE),
		.pd_pa = _A(GEMINI_DRAMC_BASE),
		.pd_size = _S(L1_S_SIZE),
		.pd_prot = VM_PROT_READ|VM_PROT_WRITE,
		.pd_cache = PTE_NOCACHE
	},

#if defined(MEMORY_DISK_DYNAMIC) 
	/* Ramdisk */
	{
		.pd_va = _A(GEMINI_RAMDISK_VBASE),
		.pd_pa = _A(GEMINI_RAMDISK_PBASE),
		.pd_size = _S(GEMINI_RAMDISK_SIZE),
		.pd_prot = VM_PROT_READ|VM_PROT_WRITE,
		.pd_cache = PTE_NOCACHE
	}, 
#endif

	{0}	/* list terminator */
};

#undef	_A
#undef	_S

#ifdef DDB
static void gemini_db_trap(int where)
{
#if  NGEMINIWDT > 0
	static int oldwatchdogstate;

	if (where) {
		oldwatchdogstate = geminiwdt_enable(0);
	} else {
		geminiwdt_enable(oldwatchdogstate);
	}
#endif
}
#endif

#if defined(VERBOSE_INIT_ARM) || 1
void gemini_putchar(char c);
void
gemini_putchar(char c)
{
	unsigned char *com0addr = (unsigned char *)GEMINI_UART_VBASE;
	int timo = 150000;

	while ((com0addr[COM_REG_LSR * 4] & LSR_TXRDY) == 0)
		if (--timo == 0)
			break;

	com0addr[COM_REG_TXDATA] = c;

	while ((com0addr[COM_REG_LSR * 4] & LSR_TSRE) == 0)
		if (--timo == 0)
			break;
}

void gemini_puthex(unsigned int);
void
gemini_puthex(unsigned int val)
{
	char hexc[] = "0123456789abcdef";

	gemini_putchar('0');
	gemini_putchar('x');
	gemini_putchar(hexc[(val >> 28) & 0xf]);
	gemini_putchar(hexc[(val >> 24) & 0xf]);
	gemini_putchar(hexc[(val >> 20) & 0xf]);
	gemini_putchar(hexc[(val >> 16) & 0xf]);
	gemini_putchar(hexc[(val >> 12) & 0xf]);
	gemini_putchar(hexc[(val >> 8) & 0xf]);
	gemini_putchar(hexc[(val >> 4) & 0xf]);
	gemini_putchar(hexc[(val >> 0) & 0xf]);
}
#endif	/* VERBOSE_INIT_ARM */

/*
 * u_int initarm(...)
 *
 * Initial entry point on startup. This gets called before main() is
 * entered.
 * It should be responsible for setting up everything that must be
 * in place when main is called.
 * This includes
 *   Taking a copy of the boot configuration structure.
 *   Initialising the physical console so characters can be printed.
 *   Setting up page tables for the kernel
 *   Relocating the kernel to the bottom of physical memory
 */
u_int
initarm(void *arg)
{
	GEMINI_PUTCHAR('0');

	/*
	 * start cpu#1 now
	 */
	gemini_cpu1_start();

	/*
	 * When we enter here, we are using a temporary first level
	 * translation table with section entries in it to cover the OBIO
	 * peripherals and SDRAM.  The temporary first level translation table
	 * is at the end of SDRAM.
	 */

	/* Heads up ... Setup the CPU / MMU / TLB functions. */
	GEMINI_PUTCHAR('1');
	if (set_cpufuncs())
		panic("cpu not recognized!");

	GEMINI_PUTCHAR('2');
	init_clocks();
	GEMINI_PUTCHAR('3');

	/* The console is going to try to map things.  Give pmap a devmap. */
	pmap_devmap_register(devmap);
	GEMINI_PUTCHAR('4');
	consinit();
	GEMINI_PUTCHAR('5');
#ifdef KGDB
	kgdb_port_init();
#endif

	/* Talk to the user */
	printf("\nNetBSD/evbarm (gemini) booting ...\n");

#ifdef BOOT_ARGS
	char mi_bootargs[] = BOOT_ARGS;
	parse_mi_bootargs(mi_bootargs);
#endif

#ifdef VERBOSE_INIT_ARM
	printf("initarm: Configuring system ...\n");
#endif

	/*
	 * Set up the variables that define the availability of physical
	 * memory.
	 */
	gemini_memchk();
	physical_start = GEMINI_DRAM_BASE;
#define	MEMSIZE_BYTES 	(MEMSIZE * 1024 * 1024)
	physical_end = (physical_start & ~(0x400000-1)) + MEMSIZE_BYTES;
	physmem = (physical_end - physical_start) / PAGE_SIZE;

	/* Fake bootconfig structure for the benefit of pmap.c. */
	bootconfig.dramblocks = 1;
	bootconfig.dram[0].address = physical_start;
	bootconfig.dram[0].pages = physmem;

	/*
	 * Our kernel is at the beginning of memory, so set our free space to
	 * all the memory after the kernel.
	 */
	physical_freestart = KERN_VTOPHYS(round_page((vaddr_t) _end));
	physical_freeend = physical_end;
	free_pages = (physical_freeend - physical_freestart) / PAGE_SIZE;

	/*
	 * This is going to do all the hard work of setting up the first and
	 * and second level page tables.  Pages of memory will be allocated
	 * and mapped for other structures that are required for system
	 * operation.  When it returns, physical_freestart and free_pages will
	 * have been updated to reflect the allocations that were made.  In
	 * addition, kernel_l1pt, kernel_pt_table[], systempage, irqstack,
	 * abtstack, undstack, kernelstack, msgbufphys will be set to point to
	 * the memory that was allocated for them.
	 */
	setup_real_page_tables();

	/*
	 * Moved from cpu_startup() as data_abort_handler() references
	 * this during uvm init.
	 */
	uvm_lwp_setuarea(&lwp0, kernelstack.pv_va);

#ifdef VERBOSE_INIT_ARM
	printf("bootstrap done.\n");
#endif

	arm32_vector_init(ARM_VECTORS_HIGH, ARM_VEC_ALL);

	/*
	 * Pages were allocated during the secondary bootstrap for the
	 * stacks for different CPU modes.
	 * We must now set the r13 registers in the different CPU modes to
	 * point to these stacks.
	 * Since the ARM stacks use STMFD etc. we must set r13 to the top end
	 * of the stack memory.
	 */
#ifdef VERBOSE_INIT_ARM
	printf("init subsystems: stacks ");
#endif

	set_stackptr(PSR_FIQ32_MODE, fiqstack.pv_va + FIQ_STACK_SIZE * PAGE_SIZE);
	set_stackptr(PSR_IRQ32_MODE, irqstack.pv_va + IRQ_STACK_SIZE * PAGE_SIZE);
	set_stackptr(PSR_ABT32_MODE, abtstack.pv_va + ABT_STACK_SIZE * PAGE_SIZE);
	set_stackptr(PSR_UND32_MODE, undstack.pv_va + UND_STACK_SIZE * PAGE_SIZE);

	/*
	 * Well we should set a data abort handler.
	 * Once things get going this will change as we will need a proper
	 * handler.
	 * Until then we will use a handler that just panics but tells us
	 * why.
	 * Initialisation of the vectors will just panic on a data abort.
	 * This just fills in a slightly better one.
	 */
#ifdef VERBOSE_INIT_ARM
	printf("vectors ");
#endif
	data_abort_handler_address = (u_int)data_abort_handler;
	prefetch_abort_handler_address = (u_int)prefetch_abort_handler;
	undefined_handler_address = (u_int)undefinedinstruction_bounce;

	/* Initialise the undefined instruction handlers */
#ifdef VERBOSE_INIT_ARM
	printf("undefined ");
#endif
	undefined_init();

	/* Load memory into UVM. */
#ifdef VERBOSE_INIT_ARM
	printf("page ");
#endif
	uvm_setpagesize();        /* initialize PAGE_SIZE-dependent variables */

#if (GEMINI_RAM_RESV_PBASE != 0)
	uvm_page_physload(atop(physical_freestart), atop(GEMINI_RAM_RESV_PBASE),
	    atop(physical_freestart), atop(GEMINI_RAM_RESV_PBASE),
	    VM_FREELIST_DEFAULT);
	uvm_page_physload(atop(GEMINI_RAM_RESV_PEND), atop(physical_freeend),
	    atop(GEMINI_RAM_RESV_PEND), atop(physical_freeend),
	    VM_FREELIST_DEFAULT);
#else
	uvm_page_physload(atop(physical_freestart), atop(physical_freeend),
	    atop(physical_freestart), atop(physical_freeend),
	    VM_FREELIST_DEFAULT);
#endif
	uvm_page_physload(atop(GEMINI_DRAM_BASE), atop(KERNEL_BASE_phys),
	    atop(GEMINI_DRAM_BASE), atop(KERNEL_BASE_phys),
	    VM_FREELIST_DEFAULT);

	/* Boot strap pmap telling it where the kernel page table is */
#ifdef VERBOSE_INIT_ARM
	printf("pmap ");
#endif
	pmap_bootstrap(KERNEL_VM_BASE, KERNEL_VM_BASE + KERNEL_VM_SIZE);

#ifdef VERBOSE_INIT_ARM
	printf("done.\n");
#endif

#ifdef IPKDB
	/* Initialise ipkdb */
	ipkdb_init();
	if (boothowto & RB_KDB)
		ipkdb_connect(0);
#endif

#if defined(MEMORY_DISK_DYNAMIC)
	md_root_setconf((char *)GEMINI_RAMDISK_VBASE, GEMINI_RAMDISK_SIZE);
#endif

#ifdef KGDB
	if (boothowto & RB_KDB) {
		kgdb_debug_init = 1;
		kgdb_connect(1);
	}
#endif

#ifdef DDB
	db_trap_callback = gemini_db_trap;
	db_machine_init();

	/* Firmware doesn't load symbols. */
	ddb_init(0, NULL, NULL);

	if (boothowto & RB_KDB)
		Debugger();
#endif
	printf("initarm done.\n");

	/* We return the new stack pointer address */
	return(kernelstack.pv_va + USPACE_SVC_STACK_TOP);
}

static void
init_clocks(void)
{
}

#ifndef CONSADDR
#error Specify the address of the console UART with the CONSADDR option.
#endif
#ifndef CONSPEED
#define CONSPEED 19200
#endif
#ifndef CONMODE
#define CONMODE ((TTYDEF_CFLAG & ~(CSIZE | CSTOPB | PARENB)) | CS8) /* 8N1 */
#endif

static const bus_addr_t consaddr = CONSADDR;
static const int conspeed = CONSPEED;
static const int conmode = CONMODE;

#if CONSADDR==0x42000000
/*
 * console initialization for obio com console
 */
void
consinit(void)
{
	static int consinit_called = 0;

	if (consinit_called != 0)
		return;
	consinit_called = 1;

	if (comcnattach(&gemini_a4x_bs_tag, consaddr, conspeed,
		GEMINI_COM_FREQ, COM_TYPE_16550_NOERS, conmode))
			panic("Serial console can not be initialized.");
}

#elif CONSADDR==0x478003f8 
# include <arm/gemini/gemini_lpcvar.h>
/*
 * console initialization for lpc com console
 */
void
consinit(void)
{
	static int consinit_called = 0;
	bus_space_tag_t iot = &gemini_bs_tag;
	bus_space_handle_t lpchc_ioh;
	bus_space_handle_t lpcio_ioh;
	bus_size_t sz = L1_S_SIZE;
	gemini_lpc_softc_t lpcsoftc;
	gemini_lpc_bus_ops_t *ops;
	void *lpctag = &lpcsoftc;
	uint32_t r;
	extern gemini_lpc_bus_ops_t gemini_lpc_bus_ops;

	ops = &gemini_lpc_bus_ops;

	if (consinit_called != 0)
		return;
	consinit_called = 1;

	if (bus_space_map(iot, GEMINI_LPCHC_BASE, sz, 0, &lpchc_ioh))
		panic("consinit: LPCHC can not be mapped.");

	if (bus_space_map(iot, GEMINI_LPCIO_BASE, sz, 0, &lpcio_ioh))
		panic("consinit: LPCIO can not be mapped.");

	/* enable the LPC bus */
	r = bus_space_read_4(iot, lpchc_ioh, GEMINI_LPCHC_CSR);
	r |= LPCHC_CSR_BEN;
	bus_space_write_4(iot, lpchc_ioh, GEMINI_LPCHC_CSR, r);

	memset(&lpcsoftc, 0, sizeof(lpcsoftc));
	lpcsoftc.sc_iot = iot;
	lpcsoftc.sc_ioh = lpcio_ioh;

	/* activate Serial Port 1 */
	(*ops->lpc_pnp_enter)(lpctag);
	(*ops->lpc_pnp_write)(lpctag, 1, 0x30, 0x01);
	(*ops->lpc_pnp_exit)(lpctag);

	if (comcnattach(iot, consaddr, conspeed,
		IT8712F_COM_FREQ, COM_TYPE_NORMAL, conmode)) {
			panic("Serial console can not be initialized.");
	}

	bus_space_unmap(iot, lpcio_ioh, sz);
	bus_space_unmap(iot, lpchc_ioh, sz);
}
#else
# error unknown console
#endif

#ifdef KGDB
#ifndef KGDB_DEVADDR
#error Specify the address of the kgdb UART with the KGDB_DEVADDR option.
#endif
#ifndef KGDB_DEVRATE
#define KGDB_DEVRATE 19200
#endif

#ifndef KGDB_DEVMODE
#define KGDB_DEVMODE ((TTYDEF_CFLAG & ~(CSIZE | CSTOPB | PARENB)) | CS8) /* 8N1 */
#endif
static const vaddr_t comkgdbaddr = KGDB_DEVADDR;
static const int comkgdbspeed = KGDB_DEVRATE;
static const int comkgdbmode = KGDB_DEVMODE;

void
static kgdb_port_init(void)
{
	static int kgdbsinit_called = 0;

	if (kgdbsinit_called != 0)
		return;

	kgdbsinit_called = 1;

	bus_space_handle_t bh;
	if (bus_space_map(&gemini_a4x_bs_tag, comkgdbaddr,
		GEMINI_UART_SIZE, 0, &bh))
			panic("kgdb port can not be mapped.");

	if (com_kgdb_attach(&gemini_a4x_bs_tag, comkgdbaddr, comkgdbspeed,
		GEMINI_UART_SIZE, COM_TYPE_16550_NOERS, comkgdbmode))
			panic("KGDB uart can not be initialized.");

	bus_space_unmap(&gemini_a4x_bs_tag, bh, GEMINI_UART_SIZE);
}
#endif

static void
setup_real_page_tables(void)
{
	/*
	 * We need to allocate some fixed page tables to get the kernel going.
	 *
	 * We are going to allocate our bootstrap pages from the beginning of
	 * the free space that we just calculated.  We allocate one page
	 * directory and a number of page tables and store the physical
	 * addresses in the kernel_pt_table array.
	 *
	 * The kernel page directory must be on a 16K boundary.  The page
	 * tables must be on 4K boundaries.  What we do is allocate the
	 * page directory on the first 16K boundary that we encounter, and
	 * the page tables on 4K boundaries otherwise.  Since we allocate
	 * at least 3 L2 page tables, we are guaranteed to encounter at
	 * least one 16K aligned region.
	 */

#ifdef VERBOSE_INIT_ARM
	printf("Allocating page tables\n");
#endif

	/*
	 * Define a macro to simplify memory allocation.  As we allocate the
	 * memory, make sure that we don't walk over our temporary first level
	 * translation table.
	 */
#define valloc_pages(var, np)						\
	(var).pv_pa = physical_freestart;				\
	physical_freestart += ((np) * PAGE_SIZE);			\
	if (physical_freestart > (physical_freeend - L1_TABLE_SIZE))	\
		panic("initarm: out of memory");			\
	free_pages -= (np);						\
	(var).pv_va = KERN_PHYSTOV((var).pv_pa);			\
	memset((char *)(var).pv_va, 0, ((np) * PAGE_SIZE));

	int loop, pt_index;

	pt_index = 0;
	kernel_l1pt.pv_pa = 0;
	kernel_l1pt.pv_va = 0;
#ifdef VERBOSE_INIT_ARM
	printf("%s: physical_freestart %#lx\n", __func__, physical_freestart);
#endif
	for (loop = 0; loop <= NUM_KERNEL_PTS; ++loop) {
		/* Are we 16KB aligned for an L1 ? */
		if ((physical_freestart & (L1_TABLE_SIZE - 1)) == 0
		    && kernel_l1pt.pv_pa == 0) {
			valloc_pages(kernel_l1pt, L1_TABLE_SIZE / PAGE_SIZE);
		} else {
			valloc_pages(kernel_pt_table[pt_index],
			    L2_TABLE_SIZE / PAGE_SIZE);
			++pt_index;
		}
	}

#if (NGEMINIIPM > 0)
	valloc_pages(ipmq_pt, L2_TABLE_SIZE / PAGE_SIZE);
#endif

#ifdef VERBOSE_INIT_ARM
	pt_index=0;
	printf("%s: kernel_l1pt: %#lx:%#lx\n",
		__func__, kernel_l1pt.pv_va, kernel_l1pt.pv_pa);
	printf("%s: kernel_pt_table:\n", __func__);
	for (loop = 0; loop < NUM_KERNEL_PTS; ++loop) {
		printf("\t%#lx:%#lx\n", kernel_pt_table[pt_index].pv_va,
			kernel_pt_table[pt_index].pv_pa);
		++pt_index;
	}
#if (NGEMINIIPM > 0)
	printf("%s: ipmq_pt:\n", __func__);
	printf("\t%#lx:%#lx\n", ipmq_pt.pv_va, ipmq_pt.pv_pa);
#endif
#endif

	/* This should never be able to happen but better confirm that. */
	if (!kernel_l1pt.pv_pa || (kernel_l1pt.pv_pa & (L1_TABLE_SIZE-1)) != 0)
		panic("initarm: Failed to align the kernel page directory");

	/*
	 * Allocate a page for the system page mapped to V0x00000000
	 * This page will just contain the system vectors and can be
	 * shared by all processes.
	 */
	valloc_pages(systempage, 1);
	systempage.pv_va = ARM_VECTORS_HIGH;

	/* Allocate stacks for all modes */
	valloc_pages(fiqstack, FIQ_STACK_SIZE);
	valloc_pages(irqstack, IRQ_STACK_SIZE);
	valloc_pages(abtstack, ABT_STACK_SIZE);
	valloc_pages(undstack, UND_STACK_SIZE);
	valloc_pages(kernelstack, UPAGES);

	/* Allocate the message buffer. */
	pv_addr_t msgbuf;
	int msgbuf_pgs = round_page(MSGBUFSIZE) / PAGE_SIZE;
	valloc_pages(msgbuf, msgbuf_pgs);
	msgbufphys = msgbuf.pv_pa;

	/*
	 * Ok we have allocated physical pages for the primary kernel
	 * page tables
	 */

#ifdef VERBOSE_INIT_ARM
	printf("Creating L1 page table at 0x%08lx\n", kernel_l1pt.pv_pa);
#endif

	/*
	 * Now we start construction of the L1 page table
	 * We start by mapping the L2 page tables into the L1.
	 * This means that we can replace L1 mappings later on if necessary
	 */
	vaddr_t l1_va = kernel_l1pt.pv_va;
	paddr_t l1_pa = kernel_l1pt.pv_pa;

	/* Map the L2 pages tables in the L1 page table */
	pmap_link_l2pt(l1_va, ARM_VECTORS_HIGH & ~(0x00400000 - 1),
		       &kernel_pt_table[KERNEL_PT_SYS]);
	for (loop = 0; loop < KERNEL_PT_KERNEL_NUM; loop++)
		pmap_link_l2pt(l1_va, KERNEL_BASE + loop * 0x00400000,
			       &kernel_pt_table[KERNEL_PT_KERNEL + loop]);
	for (loop = 0; loop < KERNEL_PT_VMDATA_NUM; loop++)
		pmap_link_l2pt(l1_va, KERNEL_VM_BASE + loop * 0x00400000,
			       &kernel_pt_table[KERNEL_PT_VMDATA + loop]);

	/* update the top of the kernel VM */
	pmap_curmaxkvaddr =
	    KERNEL_VM_BASE + (KERNEL_PT_VMDATA_NUM * 0x00400000);

#if (NGEMINIIPM > 0)
printf("%s:%d: pmap_link_l2pt ipmq_pt\n", __FUNCTION__, __LINE__);
	pmap_link_l2pt(l1_va, GEMINI_IPMQ_VBASE, &ipmq_pt);
#endif

#ifdef VERBOSE_INIT_ARM
	printf("Mapping kernel\n");
#endif

	/* Now we fill in the L2 pagetable for the kernel static code/data */
#define round_L_page(x) (((x) + L2_L_OFFSET) & L2_L_FRAME)
	size_t textsize = round_L_page(etext - KERNEL_BASE_virt);
	size_t totalsize = round_L_page(_end - KERNEL_BASE_virt);
	/* offset of kernel in RAM */
	u_int offset = (u_int)KERNEL_BASE_virt - KERNEL_BASE;

#ifdef DDB
	/* Map text section read-write. */
	offset += pmap_map_chunk(l1_va,
				(vaddr_t)KERNEL_BASE + offset,
				 physical_start + offset, textsize,
				 VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE,
				 PTE_CACHE);
#else
	/* Map text section read-only. */
	offset += pmap_map_chunk(l1_va,
				(vaddr_t)KERNEL_BASE + offset,
				 physical_start + offset, textsize,
				 VM_PROT_READ|VM_PROT_EXECUTE, PTE_CACHE);
#endif
	/* Map data and bss sections read-write. */
	offset += pmap_map_chunk(l1_va,
				(vaddr_t)KERNEL_BASE + offset,
				 physical_start + offset, totalsize - textsize,
				 VM_PROT_READ|VM_PROT_WRITE, PTE_CACHE);

#ifdef VERBOSE_INIT_ARM
	printf("Constructing L2 page tables\n");
#endif

	/* Map the stack pages */
	pmap_map_chunk(l1_va, fiqstack.pv_va, fiqstack.pv_pa,
	    FIQ_STACK_SIZE * PAGE_SIZE, VM_PROT_READ|VM_PROT_WRITE, PTE_CACHE);
	pmap_map_chunk(l1_va, irqstack.pv_va, irqstack.pv_pa,
	    IRQ_STACK_SIZE * PAGE_SIZE, VM_PROT_READ|VM_PROT_WRITE, PTE_CACHE);
	pmap_map_chunk(l1_va, abtstack.pv_va, abtstack.pv_pa,
	    ABT_STACK_SIZE * PAGE_SIZE, VM_PROT_READ|VM_PROT_WRITE, PTE_CACHE);
	pmap_map_chunk(l1_va, undstack.pv_va, undstack.pv_pa,
	    UND_STACK_SIZE * PAGE_SIZE, VM_PROT_READ|VM_PROT_WRITE, PTE_CACHE);
	pmap_map_chunk(l1_va, kernelstack.pv_va, kernelstack.pv_pa,
	    UPAGES * PAGE_SIZE, VM_PROT_READ | VM_PROT_WRITE, PTE_CACHE);

	pmap_map_chunk(l1_va, kernel_l1pt.pv_va, kernel_l1pt.pv_pa,
	    L1_TABLE_SIZE, VM_PROT_READ | VM_PROT_WRITE, PTE_PAGETABLE);

	for (loop = 0; loop < NUM_KERNEL_PTS; ++loop) {
		pmap_map_chunk(l1_va, kernel_pt_table[loop].pv_va,
			       kernel_pt_table[loop].pv_pa, L2_TABLE_SIZE,
			       VM_PROT_READ|VM_PROT_WRITE, PTE_PAGETABLE);
	}

	/* Map the vector page. */
	pmap_map_entry(l1_va, ARM_VECTORS_HIGH, systempage.pv_pa,
		       VM_PROT_READ|VM_PROT_WRITE, PTE_CACHE);

#if (NGEMINIIPM > 0)
	/* Map the IPM queue l2pt */
	pmap_map_chunk(l1_va, ipmq_pt.pv_va, ipmq_pt.pv_pa,
		L2_TABLE_SIZE, VM_PROT_READ|VM_PROT_WRITE, PTE_PAGETABLE);

	/* Map the IPM queue pages */
	pmap_map_chunk(l1_va, GEMINI_IPMQ_VBASE, GEMINI_IPMQ_PBASE,
	    GEMINI_IPMQ_SIZE, VM_PROT_READ|VM_PROT_WRITE, PTE_NOCACHE);

#ifdef GEMINI_SLAVE
	/*
	 * Map all memory, incluuding that owned by other core
	 * take into account the RAM remap, so view in this region
	 * is consistent with MASTER
	 */
	pmap_map_chunk(l1_va,
	    GEMINI_ALLMEM_VBASE,
	    GEMINI_ALLMEM_PBASE + ((GEMINI_ALLMEM_SIZE - MEMSIZE) * 1024 * 1024),
	    (GEMINI_ALLMEM_SIZE - MEMSIZE) * 1024 * 1024,
	    VM_PROT_READ|VM_PROT_WRITE, PTE_CACHE);
	pmap_map_chunk(l1_va,
	    GEMINI_ALLMEM_VBASE + GEMINI_BUSBASE * 1024 * 1024,
	    GEMINI_ALLMEM_PBASE,
	    (MEMSIZE * 1024 * 1024),
	    VM_PROT_READ|VM_PROT_WRITE, PTE_CACHE);
#else
	/* Map all memory, incluuding that owned by other core */
	pmap_map_chunk(l1_va, GEMINI_ALLMEM_VBASE, GEMINI_ALLMEM_PBASE,
	    GEMINI_ALLMEM_SIZE * 1024 * 1024, VM_PROT_READ|VM_PROT_WRITE, PTE_CACHE);
#endif	/* GEMINI_SLAVE */
#endif	/* NGEMINIIPM */

	/*
	 * Map integrated peripherals at same address in first level page
	 * table so that we can continue to use console.
	 */
	pmap_devmap_bootstrap(l1_va, devmap);


#ifdef VERBOSE_INIT_ARM
	/* Tell the user about where all the bits and pieces live. */
	printf("%22s       Physical              Virtual        Num\n", " ");
	printf("%22s Starting    Ending    Starting    Ending   Pages\n", " ");

	static const char mem_fmt[] =
	    "%20s: 0x%08lx 0x%08lx 0x%08lx 0x%08lx %d\n";
	static const char mem_fmt_nov[] =
	    "%20s: 0x%08lx 0x%08lx                       %d\n";

	printf(mem_fmt, "SDRAM", physical_start, physical_end-1,
	    KERN_PHYSTOV(physical_start), KERN_PHYSTOV(physical_end-1),
	    physmem);
	printf(mem_fmt, "text section",
	       KERN_VTOPHYS(KERNEL_BASE_virt), KERN_VTOPHYS(etext-1),
	       (vaddr_t)KERNEL_BASE_virt, (vaddr_t)etext-1,
	       (int)(textsize / PAGE_SIZE));
	printf(mem_fmt, "data section",
	       KERN_VTOPHYS(__data_start), KERN_VTOPHYS(_edata),
	       (vaddr_t)__data_start, (vaddr_t)_edata,
	       (int)((round_page((vaddr_t)_edata)
		      - trunc_page((vaddr_t)__data_start)) / PAGE_SIZE));
	printf(mem_fmt, "bss section",
	       KERN_VTOPHYS(__bss_start), KERN_VTOPHYS(__bss_end__),
	       (vaddr_t)__bss_start, (vaddr_t)__bss_end__,
	       (int)((round_page((vaddr_t)__bss_end__)
		      - trunc_page((vaddr_t)__bss_start)) / PAGE_SIZE));
	printf(mem_fmt, "L1 page directory",
	    kernel_l1pt.pv_pa, kernel_l1pt.pv_pa + L1_TABLE_SIZE - 1,
	    kernel_l1pt.pv_va, kernel_l1pt.pv_va + L1_TABLE_SIZE - 1,
	    L1_TABLE_SIZE / PAGE_SIZE);
	printf(mem_fmt, "Exception Vectors",
	    systempage.pv_pa, systempage.pv_pa + PAGE_SIZE - 1,
	    (vaddr_t)ARM_VECTORS_HIGH, (vaddr_t)ARM_VECTORS_HIGH + PAGE_SIZE - 1,
	    1);
	printf(mem_fmt, "FIQ stack",
	    fiqstack.pv_pa, fiqstack.pv_pa + (FIQ_STACK_SIZE * PAGE_SIZE) - 1,
	    fiqstack.pv_va, fiqstack.pv_va + (FIQ_STACK_SIZE * PAGE_SIZE) - 1,
	    FIQ_STACK_SIZE);
	printf(mem_fmt, "IRQ stack",
	    irqstack.pv_pa, irqstack.pv_pa + (IRQ_STACK_SIZE * PAGE_SIZE) - 1,
	    irqstack.pv_va, irqstack.pv_va + (IRQ_STACK_SIZE * PAGE_SIZE) - 1,
	    IRQ_STACK_SIZE);
	printf(mem_fmt, "ABT stack",
	    abtstack.pv_pa, abtstack.pv_pa + (ABT_STACK_SIZE * PAGE_SIZE) - 1,
	    abtstack.pv_va, abtstack.pv_va + (ABT_STACK_SIZE * PAGE_SIZE) - 1,
	    ABT_STACK_SIZE);
	printf(mem_fmt, "UND stack",
	    undstack.pv_pa, undstack.pv_pa + (UND_STACK_SIZE * PAGE_SIZE) - 1,
	    undstack.pv_va, undstack.pv_va + (UND_STACK_SIZE * PAGE_SIZE) - 1,
	    UND_STACK_SIZE);
	printf(mem_fmt, "SVC stack",
	    kernelstack.pv_pa, kernelstack.pv_pa + (UPAGES * PAGE_SIZE) - 1,
	    kernelstack.pv_va, kernelstack.pv_va + (UPAGES * PAGE_SIZE) - 1,
	    UPAGES);
	printf(mem_fmt_nov, "Message Buffer",
	    msgbufphys, msgbufphys + msgbuf_pgs * PAGE_SIZE - 1, msgbuf_pgs);
	printf(mem_fmt, "Free Memory", physical_freestart, physical_freeend-1,
	    KERN_PHYSTOV(physical_freestart), KERN_PHYSTOV(physical_freeend-1),
	    free_pages);
#endif

	/*
	 * Now we have the real page tables in place so we can switch to them.
	 * Once this is done we will be running with the REAL kernel page
	 * tables.
	 */

	/* Switch tables */
#ifdef VERBOSE_INIT_ARM
	printf("switching to new L1 page table  @%#lx...", l1_pa);
#endif

	cpu_domains((DOMAIN_CLIENT << (PMAP_DOMAIN_KERNEL*2)) | DOMAIN_CLIENT);
	cpu_setttb(l1_pa, true);
	cpu_tlb_flushID();
	cpu_domains(DOMAIN_CLIENT << (PMAP_DOMAIN_KERNEL*2));

#ifdef VERBOSE_INIT_ARM
	printf("OK.\n");
#endif
}
