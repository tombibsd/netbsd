/*	$NetBSD$ */
/*-
 * Copyright (c) 2013 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Emmanuel Dreyfus
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__RCSID("$NetBSD$");

#include <sys/resource.h>
#include <atf-c.h>
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>

#define DSO TESTDIR "/h_pthread_dlopen.so"

void *
routine(void *arg)
{
	ATF_REQUIRE((intptr_t)arg == 0xcafe);
	return NULL;
}

ATF_TC(dso_pthread_create_dso);

ATF_TC_HEAD(dso_pthread_create_dso, tc)
{
	atf_tc_set_md_var(tc, "descr",
	    "Test if non -lpthread main can call pthread_create() "
	    "in -lpthread DSO");
}

ATF_TC_BODY(dso_pthread_create_dso, tc)
{
	int ret;
	pthread_t thread;
	void *arg = (void *)0xcafe;
	void *handle;
	int (*testf_dso_pthread_create)(pthread_t *, pthread_attr_t *, 
	    void *(*)(void *), void *);
	struct rlimit rl;

	atf_tc_expect_signal(6,
	    "calling pthread_create() requires -lpthread main");
	
	rl.rlim_max = rl.rlim_cur = 0;
	ATF_REQUIRE_EQ(setrlimit(RLIMIT_CORE, &rl), 0);

	handle = dlopen(DSO, RTLD_NOW | RTLD_LOCAL);
	ATF_REQUIRE_MSG(handle != NULL, "dlopen fails: %s", dlerror());

	testf_dso_pthread_create = dlsym(handle, "testf_dso_pthread_create");
	ATF_REQUIRE_MSG(testf_dso_pthread_create != NULL, 
	    "dlsym fails: %s", dlerror());

	ret = testf_dso_pthread_create(&thread, NULL, routine, arg);
	ATF_REQUIRE(ret == 0);

	ATF_REQUIRE(dlclose(handle) == 0);

}

ATF_TP_ADD_TCS(tp)
{
	ATF_TP_ADD_TC(tp, dso_pthread_create_dso);

	return atf_no_error();
}
