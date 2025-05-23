/*	$NetBSD$ */

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Paul Kranenburg.
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

struct lsi64854_softc {
	device_t		sc_dev;		/* base device */
	bus_space_tag_t		sc_bustag;	/* bus tags */
	bus_dma_tag_t		sc_dmatag;

	bus_space_handle_t	sc_regs;	/* the registers */
	u_int			sc_rev;		/* revision */
	int			sc_burst;	/* max suported burst size */

	int			sc_channel;
#define L64854_CHANNEL_SCSI	1
#define L64854_CHANNEL_ENET	2
#define L64854_CHANNEL_PP	3
	void			*sc_client;

	int			sc_active;	/* DMA active ? */
	bus_dmamap_t		sc_dmamap;	/* DMA map for bus_dma_* */
	void *			sc_dvmaaddr;	/* DVMA cookie */
	size_t			sc_dmasize;
	uint8_t			**sc_dmaaddr;
	size_t			*sc_dmalen;

	void	(*reset)(struct lsi64854_softc *);/* reset routine */
	int	(*setup)(struct lsi64854_softc *, uint8_t **, size_t *,
			      int, size_t *);	/* DMA setup */
	int	(*intr)(void *);		/* interrupt handler */

	int	(*sc_intrchain)(void *);	/* next handler in intr chain */
	void	*sc_intrchainarg;		/* arg for next intr handler */

	u_int 			sc_dmactl;
};

#define L64854_GCSR(sc)	\
	(bus_space_read_4((sc)->sc_bustag, (sc)->sc_regs, L64854_REG_CSR))

#define L64854_SCSR(sc, csr)	\
	bus_space_write_4((sc)->sc_bustag, (sc)->sc_regs, L64854_REG_CSR, csr)


/*
 * DMA engine interface functions.
 */
#define DMA_RESET(sc)			(((sc)->reset)(sc))
#define DMA_INTR(sc)			(((sc)->intr)(sc))
#define DMA_SETUP(sc, a, l, d, s)	(((sc)->setup)(sc, a, l, d, s))

#define DMA_ISACTIVE(sc)		((sc)->sc_active)

#define DMA_ENINTR(sc) do {			\
	uint32_t _csr = L64854_GCSR(sc);	\
	_csr |= L64854_INT_EN;			\
	L64854_SCSR(sc, _csr);			\
} while (/*CONSTCOND*/0)

#define DMA_ISINTR(sc)	(L64854_GCSR(sc) & (D_INT_PEND|D_ERR_PEND))

#define DMA_GO(sc) do {				\
	uint32_t _csr = L64854_GCSR(sc);	\
	_csr |= D_EN_DMA;			\
	L64854_SCSR(sc, _csr);			\
	sc->sc_active = 1;			\
} while (0)


void	lsi64854_attach(struct lsi64854_softc *);
int	lsi64854_scsi_intr(void *);
int	lsi64854_enet_intr(void *);
int	lsi64854_pp_intr(void *);
