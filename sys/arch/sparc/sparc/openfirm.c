/*	$NetBSD$	*/

/*
 * Copyright (C) 1995, 1996 Wolfgang Solfrank.
 * Copyright (C) 1995, 1996 TooLs GmbH.
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
 *	This product includes software developed by TooLs GmbH.
 * 4. The name of TooLs GmbH may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY TOOLS GMBH ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TOOLS GMBH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include <sys/param.h>
#include <sys/systm.h>
#include <machine/lock.h>
#include <machine/psl.h>
#include <machine/promlib.h>
#include <lib/libkern/libkern.h>

#ifndef _KERNEL
#include <sys/stdarg.h>
#endif

#ifdef SUN4V
#ifdef __arch64__
#define OFBOUNCE_MAXSIZE 1024
/* 
 * Sun4v OpenBoot is not always happy with 64-bit addresses - an example is the
 * addr parameter in the OF_write() call which can be truncated to a 32-bit
 * value.
 * Avoid this behaviour by using a static buffer which is assumed to be mapped
 * in on a 32-bit address.
 * Use a mutex to protect access to the buffer from multiple threads.
 * 
 */
static __cpu_simple_lock_t ofcall_lock;
static char ofbounce[OFBOUNCE_MAXSIZE];
#endif
#endif

void
OF_init(void)
{
#ifdef SUN4V
#ifdef __arch64__
  KASSERT(((uint64_t)&ofbounce & 0xffffffffUL)==(uint64_t)&ofbounce);
  __cpu_simple_lock_init(&ofcall_lock);
#endif	
#endif
}

int
OF_peer(int phandle)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t phandle;
		cell_t sibling;
	} args;

	args.name = ADR2CELL("peer");
	args.nargs = 1;
	args.nreturns = 1;
	args.phandle = HDL2CELL(phandle);
	if (openfirmware(&args) == -1)
		return 0;
	return args.sibling;
}

int
OF_child(int phandle)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t phandle;
		cell_t child;
	} args;

	args.name = ADR2CELL("child");
	args.nargs = 1;
	args.nreturns = 1;
	args.phandle = HDL2CELL(phandle);
	if (openfirmware(&args) == -1)
		return 0;
	return args.child;
}

int
OF_parent(int phandle)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t phandle;
		cell_t parent;
	} args;

	args.name = ADR2CELL("parent");
	args.nargs = 1;
	args.nreturns = 1;
	args.phandle = HDL2CELL(phandle);
	if (openfirmware(&args) == -1)
		return 0;
	return args.parent;
}

int
OF_instance_to_package(int ihandle)
{
	static struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t ihandle;
		cell_t phandle;
	} args;

	args.name = ADR2CELL("instance-to-package");
	args.nargs = 1;
	args.nreturns = 1;
	args.ihandle = HDL2CELL(ihandle);
	if (openfirmware(&args) == -1)
		return -1;
	return args.phandle;
}

/* Should really return a `long' */
int
OF_getproplen(int handle, const char *prop)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t phandle;
		cell_t prop;
		cell_t size;
	} args;

	args.name = ADR2CELL("getproplen");
	args.nargs = 2;
	args.nreturns = 1;
	args.phandle = HDL2CELL(handle);
	args.prop = ADR2CELL(prop);
	if (openfirmware(&args) == -1)
		return -1;
	return args.size;
}

int
OF_getprop(int handle, const char *prop, void *buf, int buflen)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t phandle;
		cell_t prop;
		cell_t buf;
		cell_t buflen;
		cell_t size;
	} args;

	if (buflen > NBPG)
		return -1;
	args.name = ADR2CELL("getprop");
	args.nargs = 4;
	args.nreturns = 1;
	args.phandle = HDL2CELL(handle);
	args.prop = ADR2CELL(prop);
	args.buf = ADR2CELL(buf);
	args.buflen = buflen;
	if (openfirmware(&args) == -1)
		return -1;
	return args.size;
}

int
OF_setprop(int handle, const char *prop, const void *buf, int buflen)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t phandle;
		cell_t prop;
		cell_t buf;
		cell_t buflen;
		cell_t size;
	} args;

	if (buflen > NBPG)
		return -1;
	args.name = ADR2CELL("setprop");
	args.nargs = 4;
	args.nreturns = 1;
	args.phandle = HDL2CELL(handle);
	args.prop = ADR2CELL(prop);
	args.buf = ADR2CELL(buf);
	args.buflen = buflen;
	if (openfirmware(&args) == -1)
		return -1;
	return args.size;
}

int
OF_nextprop(int handle, const char *prop, void *buf)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t phandle;
		cell_t prev;
		cell_t buf;
		cell_t next;
	} args;

	args.name = ADR2CELL("nextprop");
	args.nargs = 3;
	args.nreturns = 1;
	args.phandle = HDL2CELL(handle);
	args.prev = ADR2CELL(prop);
	args.buf = ADR2CELL(buf);
	if (openfirmware(&args) == -1)
		return -1;
	return args.next;
}

int
OF_finddevice(const char *name)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t device;
		cell_t phandle;
	} args;

	args.name = ADR2CELL("finddevice");
	args.nargs = 1;
	args.nreturns = 1;
	args.device = ADR2CELL(name);
	if (openfirmware(&args) == -1)
		return -1;
	return args.phandle;
}

int
OF_instance_to_path(int ihandle, char *buf, int buflen)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t ihandle;
		cell_t buf;
		cell_t buflen;
		cell_t length;
	} args;

	if (buflen > NBPG)
		return -1;
	args.name = ADR2CELL("instance-to-path");
	args.nargs = 3;
	args.nreturns = 1;
	args.ihandle = HDL2CELL(ihandle);
	args.buf = ADR2CELL(buf);
	args.buflen = buflen;
	if (openfirmware(&args) < 0)
		return -1;
	return args.length;
}

int
OF_package_to_path(int phandle, char *buf, int buflen)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t phandle;
		cell_t buf;
		cell_t buflen;
		cell_t length;
	} args;

	if (buflen > NBPG)
		return -1;
	args.name = ADR2CELL("package-to-path");
	args.nargs = 3;
	args.nreturns = 1;
	args.phandle = HDL2CELL(phandle);
	args.buf = ADR2CELL(buf);
	args.buflen = buflen;
	if (openfirmware(&args) < 0)
		return -1;
	return args.length;
}

/*
 * The following two functions may need to be re-worked to be 64-bit clean.
 */
int
OF_call_method(const char *method, int ihandle, int nargs, int nreturns, ...)
{
	va_list ap;
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t method;
		cell_t ihandle;
		cell_t args_n_results[12];
	} args;
	long *ip, n;

	if (nargs > 6)
		return -1;
	args.name = ADR2CELL("call-method");
	args.nargs = nargs + 2;
	args.nreturns = nreturns + 1;
	args.method = ADR2CELL(method);
	args.ihandle = HDL2CELL(ihandle);
	va_start(ap, nreturns);
	for (ip = (long*)(args.args_n_results + (n = nargs)); --n >= 0;)
		*--ip = va_arg(ap, unsigned long);
	if (openfirmware(&args) == -1) {
		va_end(ap);
		return -1;
	}
	if (args.args_n_results[nargs]) {
		va_end(ap);
		return args.args_n_results[nargs];
	}
	for (ip = (long*)(args.args_n_results + nargs + (n = args.nreturns)); --n > 0;)
		*va_arg(ap, unsigned long *) = *--ip;
	va_end(ap);
	return 0;
}

int
OF_call_method_1(const char *method, int ihandle, int nargs, ...)
{
	va_list ap;
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t method;
		cell_t ihandle;
		cell_t args_n_results[16];
	} args;
	long *ip, n;

	if (nargs > 6)
		return -1;
	args.name = ADR2CELL("call-method");
	args.nargs = nargs + 2;
	args.nreturns = 1;
	args.method = ADR2CELL(method);
	args.ihandle = HDL2CELL(ihandle);
	va_start(ap, nargs);
	for (ip = (long*)(args.args_n_results + (n = nargs)); --n >= 0;)
		*--ip = va_arg(ap, unsigned long);
	va_end(ap);
	if (openfirmware(&args) == -1)
		return -1;
	if (args.args_n_results[nargs])
		return -1;
	return args.args_n_results[nargs + 1];
}

int
OF_open(const char *dname)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t dname;
		cell_t handle;
	} args;
	int l;

	if ((l = strlen(dname)) >= NBPG)
		return -1;
	args.name = ADR2CELL("open");
	args.nargs = 1;
	args.nreturns = 1;
	args.dname = ADR2CELL(dname);
	if (openfirmware(&args) == -1)
		return -1;
	return args.handle;
}

void
OF_close(int handle)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t handle;
	} args;

	args.name = ADR2CELL("close");
	args.nargs = 1;
	args.nreturns = 0;
	args.handle = HDL2CELL(handle);
	openfirmware(&args);
}

int
OF_test(const char* service)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t service;
		cell_t status;
	} args;

	args.name = ADR2CELL("test");
	args.nargs = 1;
	args.nreturns = 1;
	args.service = ADR2CELL(service);
	if (openfirmware(&args) == -1)
		return -1;
	return args.status;
}

int
OF_test_method(int service, const char* method)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t service;
		cell_t method;
		cell_t status;
	} args;

	args.name = ADR2CELL("test-method");
	args.nargs = 2;
	args.nreturns = 1;
	args.service = HDL2CELL(service);
	args.method = ADR2CELL(method);
	if (openfirmware(&args) == -1)
		return -1;
	return args.status;
}


/*
 * This assumes that character devices don't read in multiples of NBPG.
 */
int
OF_read(int handle, void *addr, int len)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t ihandle;
		cell_t addr;
		cell_t len;
		cell_t actual;
	} args;
	int l, act = 0;

	args.name = ADR2CELL("read");
	args.nargs = 3;
	args.nreturns = 1;
	args.ihandle = HDL2CELL(handle);
	args.addr = ADR2CELL(addr);
	for (; len > 0; len -= l) {
		l = MIN(NBPG, len);
		args.len = l;
		if (openfirmware(&args) == -1)
			return -1;
		if (args.actual > 0) {
			act += args.actual;
		}
		if (args.actual < l) {
			if (act)
				return act;
			else
				return args.actual;
		}
	}
	return act;
}

int
OF_write(int handle, const void *addr, int len)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t ihandle;
		cell_t addr;
		cell_t len;
		cell_t actual;
	} args;
	int l, act = 0;

	if (len > 1024) {
		panic("OF_write(len = %d)\n", len);
	}
#ifdef SUN4V
#if __arch64__
	__cpu_simple_lock(&ofcall_lock);
	if (len > OFBOUNCE_MAXSIZE) 
		panic("OF_write(len = %d) exceedes bounce buffer\n", len);
	memcpy(ofbounce, addr, len);
	addr = ofbounce;
#endif	
#endif
	args.name = ADR2CELL("write");
	args.nargs = 3;
	args.nreturns = 1;
	args.ihandle = HDL2CELL(handle);
	args.addr = ADR2CELL(addr);
	for (; len > 0; len -= l) {
		l = MIN(NBPG, len);
		args.len = l;
		if (openfirmware(&args) == -1)
			return -1;
		l = args.actual;
		act += l;
	}
#ifdef SUN4V
#if __arch64__
	__cpu_simple_unlock(&ofcall_lock);
#endif
#endif
	return act;
}


int
OF_seek(int handle, u_quad_t pos)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t handle;
		cell_t poshi;
		cell_t poslo;
		cell_t status;
	} args;

	args.name = ADR2CELL("seek");
	args.nargs = 3;
	args.nreturns = 1;
	args.handle = HDL2CELL(handle);
	args.poshi = HDQ2CELL_HI(pos);
	args.poslo = HDQ2CELL_LO(pos);
	if (openfirmware(&args) == -1)
		return -1;
	return args.status;
}

void
OF_boot(const char *bspec)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t bootspec;
	} args;
	int l;

	if ((l = strlen(bspec)) >= NBPG)
		panic("OF_boot");
	args.name = ADR2CELL("boot");
	args.nargs = 1;
	args.nreturns = 0;
	args.bootspec = ADR2CELL(bspec);
	openfirmware(&args);
	panic("OF_boot failed");
}

void
OF_enter(void)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
	} args;

	args.name = ADR2CELL("enter");
	args.nargs = 0;
	args.nreturns = 0;
	openfirmware(&args);
}

void
OF_exit(void)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
	} args;

	args.name = ADR2CELL("exit");
	args.nargs = 0;
	args.nreturns = 0;
	openfirmware(&args);
	panic("OF_exit failed");
}

void
OF_poweroff(void)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
	} args;

	args.name = ADR2CELL("SUNW,power-off");
	args.nargs = 0;
	args.nreturns = 0;
	openfirmware(&args);
	panic("OF_poweroff failed");
}

void
(*OF_set_callback(void (*newfunc)(void *)))(void *)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t newfunc;
		cell_t oldfunc;
	} args;

	args.name = ADR2CELL("set-callback");
	args.nargs = 1;
	args.nreturns = 1;
	args.newfunc = ADR2CELL(newfunc);
	if (openfirmware(&args) == -1)
		return 0;
	return (void*)(long)args.oldfunc;
}

void
OF_set_symbol_lookup(void (*s2v)(void *), void (*v2s)(void *))
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t sym2val;
		cell_t val2sym;
	} args;

	args.name = ADR2CELL("set-symbol-lookup");
	args.nargs = 2;
	args.nreturns = 0;
	args.sym2val = ADR2CELL(s2v);
	args.val2sym = ADR2CELL(v2s);

	(void)openfirmware(&args);
}

int
OF_interpret(const char *cmd, int nargs, int nreturns, ...)
{
	va_list ap;
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t slot[16];
	} args;
	cell_t status;
	int i = 0;

	args.name = ADR2CELL("interpret");
	args.nargs = ++nargs;
	args.nreturns = ++nreturns;
	args.slot[i++] = ADR2CELL(cmd);
	va_start(ap, nreturns);
	while (i < nargs) {
		args.slot[i++] = va_arg(ap, cell_t);
	}
	if (openfirmware(&args) == -1) {
		va_end(ap);
		return (-1);
	}
	status = args.slot[i++];
	while (i < nargs+nreturns) {
		*va_arg(ap, cell_t *) = args.slot[i++];
	}
	va_end(ap);
	return (status);
}

int
OF_milliseconds(void)
{
	struct {
		cell_t name;
		cell_t nargs;
		cell_t nreturns;
		cell_t ticks;
	} args;

	args.name = ADR2CELL("milliseconds");
	args.nargs = 0;
	args.nreturns = 1;
	if (openfirmware(&args) == -1)
		return -1;
	return (args.ticks);
}

/* Claim an area of memory. */
void *
OF_claim(void *virt, u_int size, u_int align)
{
	static struct {
		cell_t	name;
		cell_t	nargs;
		cell_t	nreturns;
		cell_t	virt;
		cell_t	size;
		cell_t	align;
		cell_t	baseaddr;
	} args;

	args.name = ADR2CELL("claim");
	args.nargs = 3;
	args.nreturns = 1;
	args.virt = ADR2CELL(virt);
	args.size = size;
	args.align = align;
	if (openfirmware(&args) == -1)
		return (void *)-1;
	return (void *)(long)args.baseaddr;
}

#if defined(_KERNEL_OPT)
#include "opt_ddb.h"
#endif

#ifdef DDB
#include <machine/db_machdep.h>
#include <ddb/db_sym.h>
#include <ddb/db_extern.h>

int obp_symbol_debug = 0;

void
OF_sym2val(void *cells)
{
	struct args {
		cell_t service;
		cell_t nargs;
		cell_t nreturns;
		cell_t symbol;
		cell_t result;
		cell_t value;
	} *args = (struct args*)cells;
	char *symbol;
	db_expr_t value;

	/* Set data segment pointer */
	__asm volatile("clr %%g4" : :);

	/* No args?  Nothing to do. */
	if (args->nargs == 0 || args->nreturns == 0)
		return;

	/* Do we have a place for the value? */
	if (args->nreturns != 2) {
		args->nreturns = 1;
		args->result = -1;
		return;
	}

	symbol = (char *)(u_long)args->symbol;
	if (obp_symbol_debug)
		prom_printf("looking up symbol %s\n", symbol);
	args->result = (db_value_of_name(symbol, &value) == true) ? 0 : -1;
	if (obp_symbol_debug)
		prom_printf("%s is %lx\n", symbol, value);
	args->value = ADR2CELL(value);
}

void
OF_val2sym(void *cells)
{
	struct args {
		cell_t service;
		cell_t nargs;
		cell_t nreturns;
		cell_t value;
		cell_t offset;
		cell_t symbol;
	} *args = (struct args*)cells;
	db_sym_t symbol;
	db_expr_t value;
	db_expr_t offset;

	/* Set data segment pointer */
	__asm volatile("clr %%g4" : :);

	if (obp_symbol_debug)
		prom_printf("OF_val2sym: nargs %lx nreturns %lx\n",
			args->nargs, args->nreturns);

	/* No args?  Nothing to do. */
	if (args->nargs == 0 || args->nreturns == 0)
		return;

	/* Do we have a place for the value? */
	if (args->nreturns != 2) {
		args->nreturns = 1;
		args->offset = -1;
		return;
	}

	value = args->value;
	if (obp_symbol_debug)
		prom_printf("looking up value %ld\n", value);
	symbol = db_search_symbol(value, 0, &offset);
	if (symbol == DB_SYM_NULL) {
		if (obp_symbol_debug)
			prom_printf("OF_val2sym: not found\n");
		args->nreturns = 1;
		args->offset = -1;
		return;
	}
	args->offset = offset;
	args->symbol = ADR2CELL(symbol);
}
#endif /* DDB */
