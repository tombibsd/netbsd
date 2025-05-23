/* $NetBSD$ */

#include <mips/pmap.h>

/* uncached accesses are bad; all accesses should be cached (and coherent) */
#undef PMAP_PAGEIDLEZERO
#define	PMAP_PAGEIDLEZERO(pa)   (pmap_zero_page(pa), true)

int sbmips_cca_for_pa(paddr_t);

#undef PMAP_CCA_FOR_PA
#define	PMAP_CCA_FOR_PA(pa)	sbmips_cca_for_pa(pa)
