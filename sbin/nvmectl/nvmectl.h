/*	$NetBSD$	*/

/*-
 * Copyright (C) 2012-2013 Intel Corporation
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
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: head/sbin/nvmecontrol/nvmecontrol.h 295087 2016-01-30 22:48:06Z imp $
 */

#ifndef __NVMECTL_H__
#define __NVMECTL_H__

#include <sys/ioctl.h>

#include <dev/ic/nvmeio.h>
#include "nvme.h"

#define NVME_CTRLR_PREFIX	"nvme"
#define NVME_NS_PREFIX		"ns"

#define DEVLIST_USAGE							       \
"       nvmectl devlist\n"

#define IDENTIFY_USAGE							       \
"       nvmectl identify [-x [-v]] <controller id|namespace id>\n"

#if 0
#define PERFTEST_USAGE							       \
"       nvmectl perftest <-n num_threads> <-o read|write>\n"	               \
"                        <-s size_in_bytes> <-t time_in_seconds>\n"	       \
"                        <-i intr|wait> [-f refthread] [-p]\n"	               \
"                        <namespace id>\n"
#endif

#if 0
#define RESET_USAGE							       \
"       nvmectl reset <controller id>\n"
#endif

#define LOGPAGE_USAGE							       \
"       nvmectl logpage <-p page_id> [-x] <controller id|namespace id>\n"  \

#if 0
#define FIRMWARE_USAGE							       \
"       nvmectl firmware [-s slot] [-f path_to_firmware] [-a] <controller id>\n"
#endif

#define POWER_USAGE							       \
"       nvmectl power [-l] [-p new-state [-w workload-hint]] <controller id>\n"

void devlist(int, char *[]) __dead;
void identify(int, char *[]) __dead;
#ifdef PERFTEST_USAGE
void perftest(int, char *[]);
#endif
#ifdef RESET_USAGE
void reset(int, char *[]);
#endif
void logpage(int, char *[]) __dead;
#ifdef FIRMWARE_USAGE
void firmware(int, char *[]);
#endif
void power(int, char *[]) __dead;

int open_dev(const char *, int *, int, int);
void parse_ns_str(const char *, char *, int *);
void read_controller_data(int, struct nvm_identify_controller *);
void read_namespace_data(int, int, struct nvm_identify_namespace *);
void print_hex(void *, uint32_t);
void read_logpage(int, uint8_t, int, void *, uint32_t);
void nvme_strvis(uint8_t *, int, const uint8_t *, int);

#endif	/* __NVMECTL_H__ */
