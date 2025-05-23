/*	$NetBSD$	*/

/*-
 * Copyright (c) 2006 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe.
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

/*
 * Utility routines to make it more convenient to work with values
 * stored in array.
 *
 * Note: There is no special magic going on here.  We use the standard
 * proplib(3) APIs to do all of this work.  Any application could do
 * exactly what we're doing here.
 */

#include "prop_object_impl.h" /* hide kernel vs. not-kernel vs. standalone */
#include <prop/proplib.h>

bool
prop_array_get_bool(prop_array_t array,
			 unsigned int indx,
			 bool *valp)
{
	prop_bool_t b;

	b = prop_array_get(array, indx);
	if (prop_object_type(b) != PROP_TYPE_BOOL)
		return (false);
	
	*valp = prop_bool_true(b);

	return (true);
}

bool
prop_array_set_bool(prop_array_t array,
			 unsigned int indx,
			 bool val)
{
	prop_bool_t b;
	int rv;

	b = prop_bool_create(val);
	if (b == NULL)
		return (false);
	rv = prop_array_set(array, indx, b);
	prop_object_release(b);

	return (rv);
}

#define	TEMPLATE(size)							\
bool							                \
prop_array_get_int ## size (prop_array_t array,				\
				 unsigned int indx,			\
				 int ## size ## _t *valp)		\
{									\
	prop_number_t num;						\
									\
	num = prop_array_get(array, indx);				\
	if (prop_object_type(num) != PROP_TYPE_NUMBER)			\
		return (false);						\
									\
	if (prop_number_unsigned(num) &&				\
	    prop_number_unsigned_integer_value(num) >			\
	   /*CONSTCOND*/((size) ==  8 ?  INT8_MAX :			\
			 (size) == 16 ? INT16_MAX :			\
			 (size) == 32 ? INT32_MAX : INT64_MAX)) {	\
		return (false);						\
	}								\
									\
	if (prop_number_size(num) > (size))				\
		return (false);						\
									\
	*valp = (int ## size ## _t) prop_number_integer_value(num);	\
									\
	return (true);							\
}									\
									\
bool								        \
prop_array_get_uint ## size (prop_array_t array,		        \
				  unsigned int indx,			\
				  uint ## size ## _t *valp)		\
{									\
	prop_number_t num;						\
									\
	num = prop_array_get(array, indx);				\
	if (prop_object_type(num) != PROP_TYPE_NUMBER)			\
		return (false);						\
									\
	if (prop_number_unsigned(num) == false &&			\
	    prop_number_integer_value(num) < 0) {			\
		return (false);						\
	}								\
									\
	if (prop_number_size(num) > (size))				\
		return (false);						\
									\
	*valp = (uint ## size ## _t)					\
	    prop_number_unsigned_integer_value(num);			\
									\
	return (true);							\
}									\
									\
bool									\
 prop_array_set_int ## size (prop_array_t array,			\
				 unsigned int indx,			\
				 int ## size ## _t val)			\
{									\
	prop_number_t num;						\
	int rv;								\
									\
	num = prop_number_create_integer((int64_t) val);		\
	if (num == NULL)						\
		return (false);						\
	rv = prop_array_set(array, indx, num);				\
	prop_object_release(num);					\
									\
	return (rv);							\
}									\
									\
bool									\
prop_array_set_uint ## size (prop_array_t array,			\
				  unsigned int indx,			\
				  uint ## size ## _t val)		\
{									\
	prop_number_t num;						\
	int rv;								\
									\
	num = prop_number_create_unsigned_integer((uint64_t) val);	\
	if (num == NULL)						\
		return (false);						\
	rv = prop_array_set(array, indx, num);				\
	prop_object_release(num);					\
									\
	return (rv);							\
}									\
									\
bool									\
prop_array_add_int ## size (prop_array_t array,				\
				 int ## size ## _t val)			\
{									\
	prop_number_t num;						\
	int rv;								\
									\
	num = prop_number_create_integer((int64_t) val);		\
	if (num == NULL)						\
		return (false);						\
	rv = prop_array_add(array, num);				\
	prop_object_release(num);					\
									\
	return (rv);							\
}									\
									\
bool									\
prop_array_add_uint ## size (prop_array_t array,			\
				  uint ## size ## _t val)		\
{									\
	prop_number_t num;						\
	int rv;								\
									\
	num = prop_number_create_integer((int64_t) val);		\
	if (num == NULL)						\
		return (false);						\
	rv = prop_array_add(array, num);				\
	prop_object_release(num);					\
									\
	return (rv);							\
}

TEMPLATE(8)
TEMPLATE(16)
TEMPLATE(32)
TEMPLATE(64)

#undef TEMPLATE

#define	TEMPLATE(variant, qualifier)					\
bool									\
prop_array_add_cstring ## variant (prop_array_t array,			\
					const char *cp)			\
{									\
	prop_string_t str;						\
	bool rv;							\
									\
	str = prop_string_create_cstring ## variant (cp);		\
	if (str == NULL)						\
		return false;						\
	rv = prop_array_add(array, str);				\
	prop_object_release(str);					\
	return rv;							\
}									\
									\
bool								        \
prop_array_get_cstring ## variant (prop_array_t array,		        \
					unsigned int indx,		\
					qualifier char **cpp)		\
{									\
	prop_string_t str;						\
									\
	str = prop_array_get(array, indx);				\
	if (prop_object_type(str) != PROP_TYPE_STRING)			\
		return (false);						\
									\
	*cpp = prop_string_cstring ## variant (str);			\
									\
	return (*cpp == NULL ? false : true);				\
}									\
									\
bool									\
prop_array_set_cstring ## variant (prop_array_t array,			\
					unsigned int indx,		\
					const char *cp)			\
{									\
	prop_string_t str;						\
	int rv;								\
									\
	str = prop_string_create_cstring ## variant (cp);		\
	if (str == NULL)						\
		return (false);						\
	rv = prop_array_set(array, indx, str);				\
	prop_object_release(str);					\
									\
	return (rv);							\
}

TEMPLATE(,)
TEMPLATE(_nocopy,const)

#undef TEMPLATE

bool
prop_array_add_and_rel(prop_array_t array, prop_object_t po)
{
	bool ret;
	if (po == NULL)
		return false;
	ret = prop_array_add(array, po);
	prop_object_release(po);
	return ret;
}
