/*	$NetBSD$	*/

/*
 * Copyright 2013 Red Hat Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Ben Skeggs
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include "nv40.h"

/*******************************************************************************
 * Perfmon object classes
 ******************************************************************************/

/*******************************************************************************
 * PPM context
 ******************************************************************************/

/*******************************************************************************
 * PPM engine/subdev functions
 ******************************************************************************/

static const struct nouveau_specdom
nv50_perfmon[] = {
	{ 0x040, (const struct nouveau_specsig[]) {
			{}
		}, &nv40_perfctr_func },
	{ 0x100, (const struct nouveau_specsig[]) {
			{ 0xc8, "gr_idle" },
			{}
		}, &nv40_perfctr_func },
	{ 0x100, (const struct nouveau_specsig[]) {
			{}
		}, &nv40_perfctr_func },
	{ 0x020, (const struct nouveau_specsig[]) {
			{}
		}, &nv40_perfctr_func },
	{ 0x040, (const struct nouveau_specsig[]) {
			{}
		}, &nv40_perfctr_func },
	{}
};

struct nouveau_oclass *
nv50_perfmon_oclass = &(struct nv40_perfmon_oclass) {
	.base.handle = NV_ENGINE(PERFMON, 0x50),
	.base.ofuncs = &(struct nouveau_ofuncs) {
		.ctor = nv40_perfmon_ctor,
		.dtor = _nouveau_perfmon_dtor,
		.init = _nouveau_perfmon_init,
		.fini = _nouveau_perfmon_fini,
	},
	.doms = nv50_perfmon,
}.base;
