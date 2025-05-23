/*	$NetBSD$	*/

/*
 * Copyright (c) 1997-2014 Erez Zadok
 * Copyright (c) 1990 Jan-Simon Pendry
 * Copyright (c) 1990 Imperial College of Science, Technology & Medicine
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Jan-Simon Pendry at Imperial College, London.
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
 *
 * File: am-utils/amq/amq.h
 *
 */

#ifndef _AMQ_H
#define _AMQ_H

/*
 * external definitions for building amq
 */

extern voidp amqproc_null_1(voidp argp, CLIENT *rqstp);
extern amq_mount_tree_p *amqproc_mnttree_1(amq_string *argp, CLIENT *rqstp);
extern voidp amqproc_umnt_1(amq_string *argp, CLIENT *rqstp);
extern amq_sync_umnt *amqproc_sync_umnt_1(amq_string *argp, CLIENT *rqstp);
extern amq_mount_stats *amqproc_stats_1(voidp argp, CLIENT *rqstp);
extern amq_mount_tree_list *amqproc_export_1(voidp argp, CLIENT *rqstp);
extern int *amqproc_setopt_1(amq_setopt *argp, CLIENT *rqstp);
extern amq_mount_info_list *amqproc_getmntfs_1(voidp argp, CLIENT *rqstp);
extern amq_map_info_list *amqproc_getmapinfo_1(voidp argp, CLIENT *rqstp);
extern int *amqproc_mount_1(voidp argp, CLIENT *rqstp);
extern amq_string *amqproc_getvers_1(voidp argp, CLIENT *rqstp);
extern int *amqproc_getpid_1(voidp argp, CLIENT *rqstp);
extern amq_string *amqproc_pawd_1(amq_string *argp, CLIENT *rqstp);

#endif /* not _AMQ_H */
