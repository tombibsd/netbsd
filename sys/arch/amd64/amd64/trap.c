/*	$NetBSD$	*/

/*-
 * Copyright (c) 1998, 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Charles M. Hannum.
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

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the University of Utah, and William Jolitz.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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
 *	@(#)trap.c	7.4 (Berkeley) 5/13/91
 */

/*
 * 386 Trap and System call handling
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include "opt_ddb.h"
#include "opt_kgdb.h"
#include "opt_xen.h"
#include "opt_dtrace.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/acct.h>
#include <sys/kauth.h>
#include <sys/kernel.h>
#include <sys/kmem.h>
#include <sys/ras.h>
#include <sys/signal.h>
#include <sys/syscall.h>
#include <sys/cpu.h>
#include <sys/ucontext.h>

#include <uvm/uvm_extern.h>

#include <machine/cpufunc.h>
#include <x86/fpu.h>
#include <machine/psl.h>
#include <machine/reg.h>
#include <machine/trap.h>
#include <machine/userret.h>
#include <machine/db_machdep.h>

#include <x86/nmi.h>

#ifndef XEN
#include "isa.h"
#endif

#include <sys/kgdb.h>

#ifdef KDTRACE_HOOKS
#include <sys/dtrace_bsd.h>

/*
 * This is a hook which is initialized by the dtrace module
 * to handle traps which might occur during DTrace probe
 * execution.
 */
dtrace_trap_func_t	dtrace_trap_func = NULL;

dtrace_doubletrap_func_t	dtrace_doubletrap_func = NULL;
#endif

void trap(struct trapframe *);
void trap_return_fault_return(struct trapframe *) __dead;

const char * const trap_type[] = {
	"privileged instruction fault",		/*  0 T_PRIVINFLT */
	"breakpoint trap",			/*  1 T_BPTFLT */
	"arithmetic trap",			/*  2 T_ARITHTRAP */
	"asynchronous system trap",		/*  3 T_ASTFLT */
	"protection fault",			/*  4 T_PROTFLT */
	"trace trap",				/*  5 T_TRCTRAP */
	"page fault",				/*  6 T_PAGEFLT */
	"alignment fault",			/*  7 T_ALIGNFLT */
	"integer divide fault",			/*  8 T_DIVIDE */
	"non-maskable interrupt",		/*  9 T_NMI */
	"overflow trap",			/* 10 T_OFLOW */
	"bounds check fault",			/* 11 T_BOUND */
	"FPU not available fault",		/* 12 T_DNA */
	"double fault",				/* 13 T_DOUBLEFLT */
	"FPU operand fetch fault",		/* 14 T_FPOPFLT */
	"invalid TSS fault",			/* 15 T_TSSFLT */
	"segment not present fault",		/* 16 T_SEGNPFLT */
	"stack fault",				/* 17 T_STKFLT */
	"machine check fault",			/* 18 T_MCA */
	"SSE FP exception",			/* 19 T_XMM */
	"reserved trap",			/* 20 T_RESERVED */
};
int	trap_types = __arraycount(trap_type);

#ifdef DEBUG
int	trapdebug = 0;
#endif

#define	IDTVEC(name)	__CONCAT(X, name)

#ifdef TRAP_SIGDEBUG
static void frame_dump(struct trapframe *);
#endif

static void *
onfault_handler(const struct pcb *pcb, const struct trapframe *tf)
{
	struct onfault_table {
		uintptr_t start;
		uintptr_t end;
		void *handler;
	};
	extern const struct onfault_table onfault_table[];
	const struct onfault_table *p;
	uintptr_t pc;

	if (pcb->pcb_onfault != NULL) {
		return pcb->pcb_onfault;
	}

	pc = tf->tf_rip;
	for (p = onfault_table; p->start; p++) {
		if (p->start <= pc && pc < p->end) {
			return p->handler;
		}
	}
	return NULL;
}

static void
trap_print(const struct trapframe *frame, const lwp_t *l)
{
	const int type = frame->tf_trapno;

	if (frame->tf_trapno < trap_types) {
		printf("fatal %s", trap_type[type]);
	} else {
		printf("unknown trap %d", type);
	}
	printf(" in %s mode\n", (type & T_USER) ? "user" : "supervisor");

	printf("trap type %d code %lx rip %lx cs %lx rflags %lx cr2 %lx "
	    "ilevel %x rsp %lx\n",
	    type, frame->tf_err, (u_long)frame->tf_rip, frame->tf_cs,
	    frame->tf_rflags, rcr2(), curcpu()->ci_ilevel, frame->tf_rsp);

	printf("curlwp %p pid %d.%d lowest kstack %p\n",
	    l, l->l_proc->p_pid, l->l_lid, KSTACK_LOWEST_ADDR(l));
}

/*
 * trap(frame): exception, fault, and trap interface to BSD kernel.
 *
 * This common code is called from assembly language IDT gate entry routines
 * that prepare a suitable stack frame, and restore this frame after the
 * exception has been processed. Note that the effect is as if the arguments
 * were passed call by reference.
 *
 * Note that the fpu traps (07 T_DNA, 10 T_ARITHTRAP and 13 T_XMM)
 * jump directly into the code in x86/fpu.c so they get processed
 * without interrupts being enabled.
 */
void
trap(struct trapframe *frame)
{
	struct lwp *l = curlwp;
	struct proc *p;
	struct pcb *pcb;
	extern char fusuintrfailure[], kcopy_fault[];
	extern char IDTVEC(oosyscall)[];
	extern char IDTVEC(osyscall)[];
	extern char IDTVEC(syscall32)[];
#ifndef XEN
	struct trapframe *vframe;
#endif
	ksiginfo_t ksi;
	void *onfault;
	int type, error;
	uint64_t cr2;
	bool pfail;

	if (__predict_true(l != NULL)) {
		pcb = lwp_getpcb(l);
		p = l->l_proc;
	} else {
		/*
		 * this can happen eg. on break points in early on boot.
		 */
		pcb = NULL;
		p = NULL;
	}
	type = frame->tf_trapno;

#ifdef DEBUG
	if (trapdebug) {
		trap_print(frame, l);
	}
#endif
	if (type != T_NMI && !KERNELMODE(frame->tf_cs, frame->tf_rflags)) {
		type |= T_USER;
		l->l_md.md_regs = frame;
		LWP_CACHE_CREDS(l, p);
	}

#ifdef KDTRACE_HOOKS
	/*
	 * A trap can occur while DTrace executes a probe. Before
	 * executing the probe, DTrace blocks re-scheduling and sets
	 * a flag in its per-cpu flags to indicate that it doesn't
	 * want to fault. On returning from the the probe, the no-fault
	 * flag is cleared and finally re-scheduling is enabled.
	 *
	 * If the DTrace kernel module has registered a trap handler,
	 * call it and if it returns non-zero, assume that it has
	 * handled the trap and modified the trap frame so that this
	 * function can return normally.
	 */
	if ((type == T_PROTFLT || type == T_PAGEFLT) &&
	    dtrace_trap_func != NULL) {
		if ((*dtrace_trap_func)(frame, type)) {
			return;
		}
	}
#endif

	switch (type) {

	default:
	we_re_toast:
		trap_print(frame, l);

		if (kdb_trap(type, 0, frame))
			return;
		if (kgdb_trap(type, frame))
			return;
		/*
		 * If this is a breakpoint, don't panic if we're not connected.
		 */
		if (type == T_BPTFLT && kgdb_disconnected()) {
			printf("kgdb: ignored %s\n", trap_type[type]);
			return;
		}
		panic("trap");
		/*NOTREACHED*/

	case T_PROTFLT:
	case T_SEGNPFLT:
	case T_ALIGNFLT:
	case T_TSSFLT:
		if (p == NULL)
			goto we_re_toast;
		/* Check for copyin/copyout fault. */
		onfault = onfault_handler(pcb, frame);
		if (onfault != NULL) {
copyefault:
			error = EFAULT;
copyfault:
			frame->tf_rip = (uintptr_t)onfault;
			frame->tf_rax = error;
			return;
		}

		/*
		 * Check for failure during return to user mode.
		 * This can happen loading invalid values into the segment
		 * registers, or during the 'iret' itself.
		 *
		 * We do this by looking at the instruction we faulted on.
		 * The specific instructions we recognize only happen when
		 * returning from a trap, syscall, or interrupt.
		 */

kernelfault:
#ifdef XEN
		/*
		 * XXX: there has to be an equivalent 'problem'
		 * but I (dsl) don't know exactly what happens!
		 * For now panic the kernel.
		 */
		goto we_re_toast;
#else
		KSI_INIT_TRAP(&ksi);
		ksi.ksi_signo = SIGSEGV;
		ksi.ksi_code = SEGV_ACCERR;
		ksi.ksi_trap = type;

		/* Get %rsp value before fault - there may be a pad word
		 * below the trap frame. */
		vframe = (void *)frame->tf_rsp;
		if (frame->tf_rip == 0) {
			/*
			 * Assume that if we jumped to null we
			 * probably did it via a null function
			 * pointer, so print the return address.
			 */
			printf("kernel jumped to null; return addr was %p\n",
			       *(void **)frame->tf_rsp);
			goto we_re_toast;
		}
		switch (*(uint16_t *)frame->tf_rip) {
		case 0xcf48:	/* iretq */
			/*
			 * The 'iretq' instruction faulted, so we have the
			 * 'user' registers saved after the kernel
			 * %rip:%cs:%fl:%rsp:%ss of the iret, and below that
			 * the user %rip:%cs:%fl:%rsp:%ss the 'iret' was
			 * processing.
			 * We must copy the user register back over the
			 * kernel fault frame to generate a normal stack
			 * frame (eg for sending a SIGSEGV).
			 */
			vframe = (void *)((char *)vframe
			    - offsetof(struct trapframe, tf_rip));
			memmove(vframe, frame,
			    offsetof(struct trapframe, tf_rip));
			/* Set the faulting address to the user %eip */
			ksi.ksi_addr = (void *)vframe->tf_rip;
			break;
		case 0x848e:	/* mov 0xa8(%rsp),%es (8e 84 24 a8 00 00 00) */
		case 0x9c8e:	/* mov 0xb0(%rsp),%ds (8e 9c 24 b0 00 00 00) */
			/*
			 * We faulted loading one if the user segment registers.
			 * The stack frame containing the user registers is
			 * still valid and pointed to by tf_rsp.
			 * Maybe we should check the iretq follows.
			 */
			if (KERNELMODE(vframe->tf_cs, vframe->tf_eflags))
				goto we_re_toast;
			/* There is no valid address for the fault */
			break;

		default:
			goto we_re_toast;
		}

		/* XXX: worry about on-stack trampolines for nested
		 * handlers?? */
		/* Save outer frame for any signal return */
		l->l_md.md_regs = vframe;
		(*p->p_emul->e_trapsignal)(l, &ksi);
		/* Return to user by reloading the user frame */
		trap_return_fault_return(vframe);
		/* NOTREACHED */
#endif

	case T_PROTFLT|T_USER:		/* protection fault */
	case T_TSSFLT|T_USER:
	case T_SEGNPFLT|T_USER:
	case T_STKFLT|T_USER:
	case T_ALIGNFLT|T_USER:
#ifdef TRAP_SIGDEBUG
		printf("pid %d.%d (%s): BUS/SEGV (%x) at rip %lx addr %lx\n",
		    p->p_pid, l->l_lid, p->p_comm, type, frame->tf_rip, rcr2());
		frame_dump(frame);
#endif
		KSI_INIT_TRAP(&ksi);
		ksi.ksi_trap = type & ~T_USER;
		ksi.ksi_addr = (void *)rcr2();
		switch (type) {
		case T_SEGNPFLT|T_USER:
		case T_STKFLT|T_USER:
			ksi.ksi_signo = SIGBUS;
			ksi.ksi_code = BUS_ADRERR;
			break;
		case T_TSSFLT|T_USER:
			ksi.ksi_signo = SIGBUS;
			ksi.ksi_code = BUS_OBJERR;
			break;
		case T_ALIGNFLT|T_USER:
			ksi.ksi_signo = SIGBUS;
			ksi.ksi_code = BUS_ADRALN;
			break;
		case T_PROTFLT|T_USER:
			ksi.ksi_signo = SIGSEGV;
			ksi.ksi_code = SEGV_ACCERR;
			break;
		default:
			KASSERT(0);
			break;
		}
		goto trapsignal;

	case T_PRIVINFLT|T_USER:	/* privileged instruction fault */
	case T_FPOPFLT|T_USER:		/* coprocessor operand fault */
#ifdef TRAP_SIGDEBUG
		printf("pid %d.%d (%s): ILL at rip %lx addr %lx\n",
		    p->p_pid, l->l_lid, p->p_comm, frame->tf_rip, rcr2());
		frame_dump(frame);
#endif
		KSI_INIT_TRAP(&ksi);
		ksi.ksi_signo = SIGILL;
		ksi.ksi_trap = type & ~T_USER;
		ksi.ksi_addr = (void *) frame->tf_rip;
		switch (type) {
		case T_PRIVINFLT|T_USER:
			ksi.ksi_code = ILL_PRVOPC;
			break;
		case T_FPOPFLT|T_USER:
			ksi.ksi_code = ILL_COPROC;
			break;
		default:
			KASSERT(0);
			break;
		}
		goto trapsignal;

	case T_ASTFLT|T_USER:
		/* Allow process switch. */
		//curcpu()->ci_data.cpu_nast++;
		if (l->l_pflag & LP_OWEUPC) {
			l->l_pflag &= ~LP_OWEUPC;
			ADDUPROF(l);
		}
		/* Allow a forced task switch. */
		if (curcpu()->ci_want_resched) {
			preempt();
		}
		goto out;

	case T_BOUND|T_USER:
	case T_OFLOW|T_USER:
	case T_DIVIDE|T_USER:
		KSI_INIT_TRAP(&ksi);
		ksi.ksi_signo = SIGFPE;
		ksi.ksi_trap = type & ~T_USER;
		ksi.ksi_addr = (void *)frame->tf_rip;
		switch (type) {
		case T_BOUND|T_USER:
			ksi.ksi_code = FPE_FLTSUB;
			break;
		case T_OFLOW|T_USER:
			ksi.ksi_code = FPE_INTOVF;
			break;
		case T_DIVIDE|T_USER:
			ksi.ksi_code = FPE_INTDIV;
			break;
		default:
#ifdef DIAGNOSTIC
			panic("unhandled type %x\n", type);
#endif
			break;
		}
		goto trapsignal;

	case T_PAGEFLT:
		/* Allow page faults in kernel mode. */
		if (__predict_false(l == NULL))
			goto we_re_toast;

		/*
		 * fusuintrfailure is used by [fs]uswintr() to prevent
		 * page faulting from inside the profiling interrupt.
		 */
		onfault = pcb->pcb_onfault;
		if (onfault == fusuintrfailure) {
			goto copyefault;
		}
		if (cpu_intr_p() || (l->l_pflag & LP_INTR) != 0) {
			goto we_re_toast;
		}

		cr2 = rcr2();

		if (frame->tf_err & PGEX_X) {
			/* SMEP might have brought us here */
			if (cr2 > VM_MIN_ADDRESS && cr2 <= VM_MAXUSER_ADDRESS)
				panic("prevented execution of %p (SMEP)",
				    (void *)cr2);
		}

		goto faultcommon;

	case T_PAGEFLT|T_USER: {	/* page fault */
		register vaddr_t va;
		register struct vmspace *vm;
		register struct vm_map *map;
		vm_prot_t ftype;
		extern struct vm_map *kernel_map;

		cr2 = rcr2();
		if (p->p_emul->e_usertrap != NULL &&
		    (*p->p_emul->e_usertrap)(l, cr2, frame) != 0)
			return;
faultcommon:
		vm = p->p_vmspace;
		if (__predict_false(vm == NULL)) {
			goto we_re_toast;
		}
		pcb->pcb_cr2 = cr2;
		va = trunc_page((vaddr_t)cr2);
		/*
		 * It is only a kernel address space fault iff:
		 *	1. (type & T_USER) == 0  and
		 *	2. pcb_onfault not set or
		 *	3. pcb_onfault set but supervisor space fault
		 * The last can occur during an exec() copyin where the
		 * argument space is lazy-allocated.
		 */
		if (type == T_PAGEFLT && va >= VM_MIN_KERNEL_ADDRESS)
			map = kernel_map;
		else
			map = &vm->vm_map;
		if (frame->tf_err & PGEX_W)
			ftype = VM_PROT_WRITE;
		else if (frame->tf_err & PGEX_X)
			ftype = VM_PROT_EXECUTE;
		else
			ftype = VM_PROT_READ;

#ifdef DIAGNOSTIC
		if (map == kernel_map && va == 0) {
			printf("trap: bad kernel access at %lx\n", va);
			goto we_re_toast;
		}
#endif
		/* Fault the original page in. */
		onfault = pcb->pcb_onfault;
		pcb->pcb_onfault = NULL;
		error = uvm_fault(map, va, ftype);
		pcb->pcb_onfault = onfault;
		if (error == 0) {
			if (map != kernel_map && (void *)va >= vm->vm_maxsaddr)
				uvm_grow(p, va);

			pfail = false;
			while (type == T_PAGEFLT) {
				/*
				 * we need to switch pmap now if we're in
				 * the middle of copyin/out.
				 *
				 * but we don't need to do so for kcopy as
				 * it never touch userspace.
 				 */
				kpreempt_disable();
				if (curcpu()->ci_want_pmapload) {
					onfault = onfault_handler(pcb, frame);
					if (onfault != kcopy_fault) {
						pmap_load();
					}
				}
				/*
				 * We need to keep the pmap loaded and
				 * so avoid being preempted until back
				 * into the copy functions.  Disable
				 * interrupts at the hardware level before
				 * re-enabling preemption.  Interrupts
				 * will be re-enabled by 'iret' when
				 * returning back out of the trap stub.
				 * They'll only be re-enabled when the
				 * program counter is once again in
				 * the copy functions, and so visible
				 * to cpu_kpreempt_exit().
				 */
#ifndef XEN
				x86_disable_intr();
#endif
				l->l_nopreempt--;
				if (l->l_nopreempt > 0 || !l->l_dopreempt ||
				    pfail) {
					return;
				}
#ifndef XEN
				x86_enable_intr();
#endif
				/*
				 * If preemption fails for some reason,
				 * don't retry it.  The conditions won't
				 * change under our nose.
				 */
				pfail = kpreempt(0);
			}
			goto out;
		}

		if (type == T_PAGEFLT) {
			onfault = onfault_handler(pcb, frame);
			if (onfault != NULL)
				goto copyfault;
			printf("uvm_fault(%p, 0x%lx, %d) -> %x\n",
			    map, va, ftype, error);
			goto kernelfault;
		}

		KSI_INIT_TRAP(&ksi);
		ksi.ksi_trap = type & ~T_USER;
		ksi.ksi_addr = (void *)cr2;
		switch (error) {
		case EINVAL:
			ksi.ksi_signo = SIGBUS;
			ksi.ksi_code = BUS_ADRERR;
			break;
		case EACCES:
			ksi.ksi_signo = SIGSEGV;
			ksi.ksi_code = SEGV_ACCERR;
			error = EFAULT;
			break;
		case ENOMEM:
			ksi.ksi_signo = SIGKILL;
			printf("UVM: pid %d.%d (%s), uid %d killed: "
			    "out of swap\n", p->p_pid, l->l_lid, p->p_comm,
			    l->l_cred ?  kauth_cred_geteuid(l->l_cred) : -1);
			break;
		default:
			ksi.ksi_signo = SIGSEGV;
			ksi.ksi_code = SEGV_MAPERR;
			break;
		}

#ifdef TRAP_SIGDEBUG
		printf("pid %d.%d (%s): signal %d at rip %lx addr %lx "
		    "error %d\n", p->p_pid, l->l_lid, p->p_comm, ksi.ksi_signo,
		    frame->tf_rip, va, error);
		frame_dump(frame);
#endif
		(*p->p_emul->e_trapsignal)(l, &ksi);
		break;
	}

	case T_TRCTRAP:
		/* Check whether they single-stepped into a lcall. */
		if (frame->tf_rip == (uint64_t)IDTVEC(oosyscall) ||
		    frame->tf_rip == (uint64_t)IDTVEC(osyscall) ||
		    frame->tf_rip == (uint64_t)IDTVEC(syscall32)) {
			frame->tf_rflags &= ~PSL_T;
			return;
		}
		goto we_re_toast;

	case T_BPTFLT|T_USER:		/* bpt instruction fault */
	case T_TRCTRAP|T_USER:		/* trace trap */
		/*
		 * Don't go single-stepping into a RAS.
		 */

		if (p->p_raslist == NULL ||
		    (ras_lookup(p, (void *)frame->tf_rip) == (void *)-1)) {
			KSI_INIT_TRAP(&ksi);
			ksi.ksi_signo = SIGTRAP;
			ksi.ksi_trap = type & ~T_USER;
			if (type == (T_BPTFLT|T_USER))
				ksi.ksi_code = TRAP_BRKPT;
			else
				ksi.ksi_code = TRAP_TRACE;
			(*p->p_emul->e_trapsignal)(l, &ksi);
		}
		break;

	case T_NMI:
		if (nmi_dispatch(frame))
			return;
		/* NMI can be hooked up to a pushbutton for debugging */
		if (kgdb_trap(type, frame))
			return;
		if (kdb_trap(type, 0, frame))
			return;
		/* machine/parity/power fail/"kitchen sink" faults */

		x86_nmi();
		return;
	}

	if ((type & T_USER) == 0)
		return;
out:
	userret(l);
	return;
trapsignal:
	(*p->p_emul->e_trapsignal)(l, &ksi);
	userret(l);
}

/* 
 * startlwp: start of a new LWP.
 */
void
startlwp(void *arg)
{
	ucontext_t *uc = arg;
	lwp_t *l = curlwp;
	int error __diagused;

	error = cpu_setmcontext(l, &uc->uc_mcontext, uc->uc_flags);
	KASSERT(error == 0);

	kmem_free(uc, sizeof(ucontext_t));
	userret(l);
}

#ifdef TRAP_SIGDEBUG
static void
frame_dump(struct trapframe *tf)
{
	int i;
	unsigned long *p;

	printf("rip %p  rsp %p  rfl %p\n",
	    (void *)tf->tf_rip, (void *)tf->tf_rsp, (void *)tf->tf_rflags);
	printf("rdi %p  rsi %p  rdx %p\n",
	    (void *)tf->tf_rdi, (void *)tf->tf_rsi, (void *)tf->tf_rdx);
	printf("rcx %p  r8  %p  r9  %p\n",
	    (void *)tf->tf_rcx, (void *)tf->tf_r8, (void *)tf->tf_r9);
	printf("r10 %p  r11 %p  r12 %p\n",
	    (void *)tf->tf_r10, (void *)tf->tf_r11, (void *)tf->tf_r12);
	printf("r13 %p  r14 %p  r15 %p\n",
	    (void *)tf->tf_r13, (void *)tf->tf_r14, (void *)tf->tf_r15);
	printf("rbp %p  rbx %p  rax %p\n",
	    (void *)tf->tf_rbp, (void *)tf->tf_rbx, (void *)tf->tf_rax);
	printf("cs %lx  ds %lx  es %lx  fs %lx  gs %lx  ss %lx\n",
		tf->tf_cs & 0xffff, tf->tf_ds & 0xffff, tf->tf_es & 0xffff,
		tf->tf_fs & 0xffff, tf->tf_gs & 0xffff, tf->tf_ss & 0xffff);

	printf("\n");
	printf("Stack dump:\n");
	for (i = 0, p = (unsigned long *) tf; i < 20; i ++, p += 4)
		printf("   0x%.16lx  0x%.16lx  0x%.16lx 0x%.16lx\n", *p, p[1], p[2], p[3]);
	printf("\n");
}
#endif
