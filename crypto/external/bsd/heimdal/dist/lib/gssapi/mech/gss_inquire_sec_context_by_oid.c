/*	$NetBSD$	*/

/*
 * Copyright (c) 2004, PADL Software Pty Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of PADL Software nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PADL SOFTWARE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL PADL SOFTWARE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "mech_locl.h"

GSSAPI_LIB_FUNCTION OM_uint32 GSSAPI_LIB_CALL
gss_inquire_sec_context_by_oid (OM_uint32 *minor_status,
	                        const gss_ctx_id_t context_handle,
	                        const gss_OID desired_object,
	                        gss_buffer_set_t *data_set)
{
	struct _gss_context	*ctx = (struct _gss_context *) context_handle;
	OM_uint32		major_status;
	gssapi_mech_interface	m;

	*minor_status = 0;
	*data_set = GSS_C_NO_BUFFER_SET;
	if (ctx == NULL)
		return GSS_S_NO_CONTEXT;

	/*
	 * select the approprate underlying mechanism routine and
	 * call it.
	 */

	m = ctx->gc_mech;

	if (m == NULL)
		return GSS_S_BAD_MECH;

	if (m->gm_inquire_sec_context_by_oid != NULL) {
		major_status = m->gm_inquire_sec_context_by_oid(minor_status,
		    ctx->gc_ctx, desired_object, data_set);
		if (major_status != GSS_S_COMPLETE)
			_gss_mg_error(m, major_status, *minor_status);
	} else
		major_status = GSS_S_BAD_MECH;

	return major_status;
}

