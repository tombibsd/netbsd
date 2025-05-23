/*	$NetBSD$ */

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce this list of conditions
 *    and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL ANY
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ARM_OMAP_OMAP_REG_H_
#define _ARM_OMAP_OMAP_REG_H_

#include "opt_omap.h"

#ifndef MEMSIZE
#error Specify the amount of SDRAM in megabytes with the MEMSIZE option.
#endif
#define MEMSIZE_BYTES (MEMSIZE * 1024 * 1024)

#ifndef OMAP_TIPB_PBASE
#error Specify the TIPB physical address with the OMAP_TIPB_PBASE option.
#endif
#ifndef OMAP_TIPB_SIZE
#error Specify the TIPB size with the OMAP_TIPB_SIZE option.
#endif

#endif /* _ARM_OMAP_OMAP_REG_H_ */
