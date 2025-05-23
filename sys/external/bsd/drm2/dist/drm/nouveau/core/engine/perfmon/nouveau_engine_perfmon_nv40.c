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

static void
nv40_perfctr_init(struct nouveau_perfmon *ppm, struct nouveau_perfdom *dom,
		  struct nouveau_perfctr *ctr)
{
	struct nv40_perfmon_priv *priv = (void *)ppm;
	struct nv40_perfmon_cntr *cntr = (void *)ctr;
	u32 log = ctr->logic_op;
	u32 src = 0x00000000;
	int i;

	for (i = 0; i < 4 && ctr->signal[i]; i++)
		src |= (ctr->signal[i] - dom->signal) << (i * 8);

	nv_wr32(priv, 0x00a7c0 + dom->addr, 0x00000001);
	nv_wr32(priv, 0x00a400 + dom->addr + (cntr->base.slot * 0x40), src);
	nv_wr32(priv, 0x00a420 + dom->addr + (cntr->base.slot * 0x40), log);
}

static void
nv40_perfctr_read(struct nouveau_perfmon *ppm, struct nouveau_perfdom *dom,
		  struct nouveau_perfctr *ctr)
{
	struct nv40_perfmon_priv *priv = (void *)ppm;
	struct nv40_perfmon_cntr *cntr = (void *)ctr;

	switch (cntr->base.slot) {
	case 0: cntr->base.ctr = nv_rd32(priv, 0x00a700 + dom->addr); break;
	case 1: cntr->base.ctr = nv_rd32(priv, 0x00a6c0 + dom->addr); break;
	case 2: cntr->base.ctr = nv_rd32(priv, 0x00a680 + dom->addr); break;
	case 3: cntr->base.ctr = nv_rd32(priv, 0x00a740 + dom->addr); break;
	}
	cntr->base.clk = nv_rd32(priv, 0x00a600 + dom->addr);
}

static void
nv40_perfctr_next(struct nouveau_perfmon *ppm, struct nouveau_perfdom *dom)
{
	struct nv40_perfmon_priv *priv = (void *)ppm;
	if (priv->sequence != ppm->sequence) {
		nv_wr32(priv, 0x400084, 0x00000020);
		priv->sequence = ppm->sequence;
	}
}

const struct nouveau_funcdom
nv40_perfctr_func = {
	.init = nv40_perfctr_init,
	.read = nv40_perfctr_read,
	.next = nv40_perfctr_next,
};

static const struct nouveau_specdom
nv40_perfmon[] = {
	{ 0x20, (const struct nouveau_specsig[]) {
			{}
		}, &nv40_perfctr_func },
	{ 0x20, (const struct nouveau_specsig[]) {
			{}
		}, &nv40_perfctr_func },
	{ 0x20, (const struct nouveau_specsig[]) {
			{}
		}, &nv40_perfctr_func },
	{ 0x20, (const struct nouveau_specsig[]) {
			{}
		}, &nv40_perfctr_func },
	{ 0x20, (const struct nouveau_specsig[]) {
			{}
		}, &nv40_perfctr_func },
	{}
};

int
nv40_perfmon_ctor(struct nouveau_object *parent, struct nouveau_object *engine,
		  struct nouveau_oclass *oclass, void *data, u32 size,
		  struct nouveau_object **pobject)
{
	struct nv40_perfmon_oclass *mclass = (void *)oclass;
	struct nv40_perfmon_priv *priv;
	int ret;

	ret = nouveau_perfmon_create(parent, engine, oclass, &priv);
	*pobject = nv_object(priv);
	if (ret)
		return ret;

	ret = nouveau_perfdom_new(&priv->base, "pm", 0, 0, 0, 4, mclass->doms);
	if (ret)
		return ret;

	nv_engine(priv)->cclass = &nouveau_perfmon_cclass;
	nv_engine(priv)->sclass =  nouveau_perfmon_sclass;
	return 0;
}

struct nouveau_oclass *
nv40_perfmon_oclass = &(struct nv40_perfmon_oclass) {
	.base.handle = NV_ENGINE(PERFMON, 0x40),
	.base.ofuncs = &(struct nouveau_ofuncs) {
		.ctor = nv40_perfmon_ctor,
		.dtor = _nouveau_perfmon_dtor,
		.init = _nouveau_perfmon_init,
		.fini = _nouveau_perfmon_fini,
	},
	.doms = nv40_perfmon,
}.base;
