/*	$NetBSD$	*/

/*
 * Copyright (C) 2004-2016  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2002  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* Id */

#include <config.h>

#include <isc/mem.h>
#include <isc/string.h>		/* Required for HP/UX (and others?) */
#include <isc/util.h>

#include <isccfg/namedconf.h>
#include <isccfg/aclconf.h>

#include <dns/acl.h>
#include <dns/iptable.h>
#include <dns/fixedname.h>
#include <dns/log.h>

#ifdef HAVE_GEOIP
#include <stdlib.h>
#include <math.h>
#endif /* HAVE_GEOIP */

#define LOOP_MAGIC ISC_MAGIC('L','O','O','P')

isc_result_t
cfg_aclconfctx_create(isc_mem_t *mctx, cfg_aclconfctx_t **ret) {
	isc_result_t result;
	cfg_aclconfctx_t *actx;

	REQUIRE(mctx != NULL);
	REQUIRE(ret != NULL && *ret == NULL);

	actx = isc_mem_get(mctx, sizeof(*actx));
	if (actx == NULL)
		return (ISC_R_NOMEMORY);

	result = isc_refcount_init(&actx->references, 1);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	actx->mctx = NULL;
	isc_mem_attach(mctx, &actx->mctx);
	ISC_LIST_INIT(actx->named_acl_cache);

#ifdef HAVE_GEOIP
	actx->geoip = NULL;
#endif

	*ret = actx;
	return (ISC_R_SUCCESS);

 cleanup:
	isc_mem_put(mctx, actx, sizeof(*actx));
	return (result);
}

void
cfg_aclconfctx_attach(cfg_aclconfctx_t *src, cfg_aclconfctx_t **dest) {
	REQUIRE(src != NULL);
	REQUIRE(dest != NULL && *dest == NULL);

	isc_refcount_increment(&src->references, NULL);
	*dest = src;
}

void
cfg_aclconfctx_detach(cfg_aclconfctx_t **actxp) {
	cfg_aclconfctx_t *actx;
	dns_acl_t *dacl, *next;
	unsigned int refs;

	REQUIRE(actxp != NULL && *actxp != NULL);

	actx = *actxp;

	isc_refcount_decrement(&actx->references, &refs);
	if (refs == 0) {
		for (dacl = ISC_LIST_HEAD(actx->named_acl_cache);
		     dacl != NULL;
		     dacl = next)
		{
			next = ISC_LIST_NEXT(dacl, nextincache);
			ISC_LIST_UNLINK(actx->named_acl_cache, dacl,
					nextincache);
			dns_acl_detach(&dacl);
		}
		isc_mem_putanddetach(&actx->mctx, actx, sizeof(*actx));
	}

	*actxp = NULL;
}

/*
 * Find the definition of the named acl whose name is "name".
 */
static isc_result_t
get_acl_def(const cfg_obj_t *cctx, const char *name, const cfg_obj_t **ret) {
	isc_result_t result;
	const cfg_obj_t *acls = NULL;
	const cfg_listelt_t *elt;

	result = cfg_map_get(cctx, "acl", &acls);
	if (result != ISC_R_SUCCESS)
		return (result);
	for (elt = cfg_list_first(acls);
	     elt != NULL;
	     elt = cfg_list_next(elt)) {
		const cfg_obj_t *acl = cfg_listelt_value(elt);
		const char *aclname = cfg_obj_asstring(cfg_tuple_get(acl, "name"));
		if (strcasecmp(aclname, name) == 0) {
			if (ret != NULL) {
				*ret = cfg_tuple_get(acl, "value");
			}
			return (ISC_R_SUCCESS);
		}
	}
	return (ISC_R_NOTFOUND);
}

static isc_result_t
convert_named_acl(const cfg_obj_t *nameobj, const cfg_obj_t *cctx,
		  isc_log_t *lctx, cfg_aclconfctx_t *ctx,
		  isc_mem_t *mctx, unsigned int nest_level,
		  dns_acl_t **target)
{
	isc_result_t result;
	const cfg_obj_t *cacl = NULL;
	dns_acl_t *dacl;
	dns_acl_t loop;
	const char *aclname = cfg_obj_asstring(nameobj);

	/* Look for an already-converted version. */
	for (dacl = ISC_LIST_HEAD(ctx->named_acl_cache);
	     dacl != NULL;
	     dacl = ISC_LIST_NEXT(dacl, nextincache))
	{
		if (strcasecmp(aclname, dacl->name) == 0) {
			if (ISC_MAGIC_VALID(dacl, LOOP_MAGIC)) {
				cfg_obj_log(nameobj, lctx, ISC_LOG_ERROR,
					    "acl loop detected: %s", aclname);
				return (ISC_R_FAILURE);
			}
			dns_acl_attach(dacl, target);
			return (ISC_R_SUCCESS);
		}
	}
	/* Not yet converted.  Convert now. */
	result = get_acl_def(cctx, aclname, &cacl);
	if (result != ISC_R_SUCCESS) {
		cfg_obj_log(nameobj, lctx, ISC_LOG_WARNING,
			    "undefined ACL '%s'", aclname);
		return (result);
	}
	/*
	 * Add a loop detection element.
	 */
	memset(&loop, 0, sizeof(loop));
	ISC_LINK_INIT(&loop, nextincache);
	DE_CONST(aclname, loop.name);
	loop.magic = LOOP_MAGIC;
	ISC_LIST_APPEND(ctx->named_acl_cache, &loop, nextincache);
	result = cfg_acl_fromconfig(cacl, cctx, lctx, ctx, mctx,
				    nest_level, &dacl);
	ISC_LIST_UNLINK(ctx->named_acl_cache, &loop, nextincache);
	loop.magic = 0;
	loop.name = NULL;
	if (result != ISC_R_SUCCESS)
		return (result);
	dacl->name = isc_mem_strdup(dacl->mctx, aclname);
	if (dacl->name == NULL)
		return (ISC_R_NOMEMORY);
	ISC_LIST_APPEND(ctx->named_acl_cache, dacl, nextincache);
	dns_acl_attach(dacl, target);
	return (ISC_R_SUCCESS);
}

static isc_result_t
convert_keyname(const cfg_obj_t *keyobj, isc_log_t *lctx, isc_mem_t *mctx,
		dns_name_t *dnsname)
{
	isc_result_t result;
	isc_buffer_t buf;
	dns_fixedname_t fixname;
	unsigned int keylen;
	const char *txtname = cfg_obj_asstring(keyobj);

	keylen = strlen(txtname);
	isc_buffer_constinit(&buf, txtname, keylen);
	isc_buffer_add(&buf, keylen);
	dns_fixedname_init(&fixname);
	result = dns_name_fromtext(dns_fixedname_name(&fixname), &buf,
				   dns_rootname, 0, NULL);
	if (result != ISC_R_SUCCESS) {
		cfg_obj_log(keyobj, lctx, ISC_LOG_WARNING,
			    "key name '%s' is not a valid domain name",
			    txtname);
		return (result);
	}
	return (dns_name_dup(dns_fixedname_name(&fixname), mctx, dnsname));
}

/*
 * Recursively pre-parse an ACL definition to find the total number
 * of non-IP-prefix elements (localhost, localnets, key) in all nested
 * ACLs, so that the parent will have enough space allocated for the
 * elements table after all the nested ACLs have been merged in to the
 * parent.
 */
static isc_result_t
count_acl_elements(const cfg_obj_t *caml, const cfg_obj_t *cctx,
		   isc_log_t *lctx, cfg_aclconfctx_t *ctx, isc_mem_t *mctx,
		   isc_uint32_t *count, isc_boolean_t *has_negative)
{
	const cfg_listelt_t *elt;
	isc_result_t result;
	isc_uint32_t n = 0;

	REQUIRE(count != NULL);

	if (has_negative != NULL)
		*has_negative = ISC_FALSE;

	for (elt = cfg_list_first(caml);
	     elt != NULL;
	     elt = cfg_list_next(elt)) {
		const cfg_obj_t *ce = cfg_listelt_value(elt);

		/* might be a negated element, in which case get the value. */
		if (cfg_obj_istuple(ce)) {
			const cfg_obj_t *negated =
				cfg_tuple_get(ce, "negated");
			if (! cfg_obj_isvoid(negated)) {
				ce = negated;
				if (has_negative != NULL)
					*has_negative = ISC_TRUE;
			}
		}

		if (cfg_obj_istype(ce, &cfg_type_keyref)) {
			n++;
		} else if (cfg_obj_islist(ce)) {
			isc_boolean_t negative;
			isc_uint32_t sub;
			result = count_acl_elements(ce, cctx, lctx, ctx, mctx,
						    &sub, &negative);
			if (result != ISC_R_SUCCESS)
				return (result);
			n += sub;
			if (negative)
				n++;
#ifdef HAVE_GEOIP
		} else if (cfg_obj_istuple(ce) &&
			   cfg_obj_isvoid(cfg_tuple_get(ce, "negated")))
		{
			n++;
#endif /* HAVE_GEOIP */
		} else if (cfg_obj_isstring(ce)) {
			const char *name = cfg_obj_asstring(ce);
			if (strcasecmp(name, "localhost") == 0 ||
			    strcasecmp(name, "localnets") == 0 ||
			    strcasecmp(name, "none") == 0)
			{
				n++;
			} else if (strcasecmp(name, "any") != 0) {
				dns_acl_t *inneracl = NULL;
				/*
				 * Convert any named acls we reference now if
				 * they have not already been converted.
				 */
				result = convert_named_acl(ce, cctx, lctx, ctx,
							   mctx, 0, &inneracl);
				if (result == ISC_R_SUCCESS) {
					if (inneracl->has_negatives)
						n++;
					else
						n += inneracl->length;
					dns_acl_detach(&inneracl);
				} else
					return (result);
			}
		}
	}

	*count = n;
	return (ISC_R_SUCCESS);
}

#ifdef HAVE_GEOIP
static dns_geoip_subtype_t
get_subtype(const cfg_obj_t *obj, isc_log_t *lctx,
	    dns_geoip_subtype_t subtype, const char *dbname)
{
	if (dbname == NULL)
		return (subtype);

	switch (subtype) {
	case dns_geoip_countrycode:
		if (strcasecmp(dbname, "city") == 0)
			return (dns_geoip_city_countrycode);
		else if (strcasecmp(dbname, "region") == 0)
			return (dns_geoip_region_countrycode);
		else if (strcasecmp(dbname, "country") == 0)
			return (dns_geoip_country_code);
		cfg_obj_log(obj, lctx, ISC_LOG_ERROR,
			    "invalid GeoIP DB specified for "
			    "country search: ignored");
		return (subtype);
	case dns_geoip_countrycode3:
		if (strcasecmp(dbname, "city") == 0)
			return (dns_geoip_city_countrycode3);
		else if (strcasecmp(dbname, "country") == 0)
			return (dns_geoip_country_code3);
		cfg_obj_log(obj, lctx, ISC_LOG_ERROR,
			    "invalid GeoIP DB specified for "
			    "country search: ignored");
		return (subtype);
	case dns_geoip_countryname:
		if (strcasecmp(dbname, "city") == 0)
			return (dns_geoip_city_countryname);
		else if (strcasecmp(dbname, "country") == 0)
			return (dns_geoip_country_name);
		cfg_obj_log(obj, lctx, ISC_LOG_ERROR,
			    "invalid GeoIP DB specified for "
			    "country search: ignored");
		return (subtype);
	case dns_geoip_region:
		if (strcasecmp(dbname, "city") == 0)
			return (dns_geoip_city_region);
		else if (strcasecmp(dbname, "region") == 0)
			return (dns_geoip_region_code);
		cfg_obj_log(obj, lctx, ISC_LOG_ERROR,
			    "invalid GeoIP DB specified for "
			    "region search: ignored");
		return (subtype);
	case dns_geoip_regionname:
		if (strcasecmp(dbname, "city") == 0)
			return (dns_geoip_city_region);
		else if (strcasecmp(dbname, "region") == 0)
			return (dns_geoip_region_name);
		cfg_obj_log(obj, lctx, ISC_LOG_ERROR,
			    "invalid GeoIP DB specified for "
			    "region search: ignored");
		return (subtype);

	/*
	 * Log a warning if the wrong database was specified
	 * on an unambiguous query
	 */
	case dns_geoip_city_name:
	case dns_geoip_city_postalcode:
	case dns_geoip_city_metrocode:
	case dns_geoip_city_areacode:
	case dns_geoip_city_continentcode:
	case dns_geoip_city_timezonecode:
		if (strcasecmp(dbname, "city") != 0)
			cfg_obj_log(obj, lctx, ISC_LOG_WARNING,
				    "invalid GeoIP DB specified for "
				    "a 'city'-only search type: ignoring");
		return (subtype);
	case dns_geoip_isp_name:
		if (strcasecmp(dbname, "isp") != 0)
			cfg_obj_log(obj, lctx, ISC_LOG_WARNING,
				    "invalid GeoIP DB specified for "
				    "an 'isp' search: ignoring");
		return (subtype);
	case dns_geoip_org_name:
		if (strcasecmp(dbname, "org") != 0)
			cfg_obj_log(obj, lctx, ISC_LOG_WARNING,
				    "invalid GeoIP DB specified for "
				    "an 'org' search: ignoring");
		return (subtype);
	case dns_geoip_as_asnum:
		if (strcasecmp(dbname, "asnum") != 0)
			cfg_obj_log(obj, lctx, ISC_LOG_WARNING,
				    "invalid GeoIP DB specified for "
				    "an 'asnum' search: ignoring");
		return (subtype);
	case dns_geoip_domain_name:
		if (strcasecmp(dbname, "domain") != 0)
			cfg_obj_log(obj, lctx, ISC_LOG_WARNING,
				    "invalid GeoIP DB specified for "
				    "a 'domain' search: ignoring");
		return (subtype);
	case dns_geoip_netspeed_id:
		if (strcasecmp(dbname, "netspeed") != 0)
			cfg_obj_log(obj, lctx, ISC_LOG_WARNING,
				    "invalid GeoIP DB specified for "
				    "a 'netspeed' search: ignoring");
		return (subtype);
	default:
		INSIST(0);
	}
}

static isc_boolean_t
geoip_can_answer(dns_aclelement_t *elt, cfg_aclconfctx_t *ctx) {
	if (ctx->geoip == NULL)
		return (ISC_TRUE);

	switch (elt->geoip_elem.subtype) {
	case dns_geoip_countrycode:
	case dns_geoip_countrycode3:
	case dns_geoip_countryname:
		if (ctx->geoip->city_v4 != NULL ||
		    ctx->geoip->city_v6 != NULL ||
		    ctx->geoip->country_v4 != NULL ||
		    ctx->geoip->country_v6 != NULL ||
		    ctx->geoip->region != NULL)
			return (ISC_TRUE);
	case dns_geoip_region:
	case dns_geoip_regionname:
		if (ctx->geoip->city_v4 != NULL ||
		    ctx->geoip->city_v6 != NULL ||
		    ctx->geoip->region != NULL)
			return (ISC_TRUE);
	case dns_geoip_country_code:
	case dns_geoip_country_code3:
	case dns_geoip_country_name:
		if (ctx->geoip->country_v4 != NULL ||
		    ctx->geoip->country_v6 != NULL)
			return (ISC_TRUE);
	case dns_geoip_region_countrycode:
	case dns_geoip_region_code:
	case dns_geoip_region_name:
		if (ctx->geoip->region != NULL)
			return (ISC_TRUE);
	case dns_geoip_city_countrycode:
	case dns_geoip_city_countrycode3:
	case dns_geoip_city_countryname:
	case dns_geoip_city_region:
	case dns_geoip_city_regionname:
	case dns_geoip_city_name:
	case dns_geoip_city_postalcode:
	case dns_geoip_city_metrocode:
	case dns_geoip_city_areacode:
	case dns_geoip_city_continentcode:
	case dns_geoip_city_timezonecode:
		if (ctx->geoip->city_v4 != NULL ||
		    ctx->geoip->city_v6 != NULL)
			return (ISC_TRUE);
	case dns_geoip_isp_name:
		if (ctx->geoip->isp != NULL)
			return (ISC_TRUE);
	case dns_geoip_org_name:
		if (ctx->geoip->org != NULL)
			return (ISC_TRUE);
	case dns_geoip_as_asnum:
		if (ctx->geoip->as != NULL)
			return (ISC_TRUE);
	case dns_geoip_domain_name:
		if (ctx->geoip->domain != NULL)
			return (ISC_TRUE);
	case dns_geoip_netspeed_id:
		if (ctx->geoip->netspeed != NULL)
			return (ISC_TRUE);
	}

	return (ISC_FALSE);
}

static isc_result_t
parse_geoip_element(const cfg_obj_t *obj, isc_log_t *lctx,
		    cfg_aclconfctx_t *ctx, dns_aclelement_t *dep)
{
	const cfg_obj_t *ge;
	const char *dbname = NULL;
	const char *stype, *search;
	dns_geoip_subtype_t subtype;
	dns_aclelement_t de;
	size_t len;

	REQUIRE(dep != NULL);

	de = *dep;

	ge = cfg_tuple_get(obj, "db");
	if (!cfg_obj_isvoid(ge))
		dbname = cfg_obj_asstring(ge);

	stype = cfg_obj_asstring(cfg_tuple_get(obj, "subtype"));
	search = cfg_obj_asstring(cfg_tuple_get(obj, "search"));
	len = strlen(search);

	if (len == 0) {
		cfg_obj_log(obj, lctx, ISC_LOG_ERROR,
			    "zero-length geoip search field");
		return (ISC_R_FAILURE);
	}

	if (strcasecmp(stype, "country") == 0 && len == 2) {
		/* Two-letter country code */
		subtype = dns_geoip_countrycode;
		strlcpy(de.geoip_elem.as_string, search,
			sizeof(de.geoip_elem.as_string));
	} else if (strcasecmp(stype, "country") == 0 && len == 3) {
		/* Three-letter country code */
		subtype = dns_geoip_countrycode3;
		strlcpy(de.geoip_elem.as_string, search,
			sizeof(de.geoip_elem.as_string));
	} else if (strcasecmp(stype, "country") == 0) {
		/* Country name */
		subtype = dns_geoip_countryname;
		strlcpy(de.geoip_elem.as_string, search,
			sizeof(de.geoip_elem.as_string));
	} else if (strcasecmp(stype, "region") == 0 && len == 2) {
		/* Two-letter region code */
		subtype = dns_geoip_region;
		strlcpy(de.geoip_elem.as_string, search,
			sizeof(de.geoip_elem.as_string));
	} else if (strcasecmp(stype, "region") == 0) {
		/* Region name */
		subtype = dns_geoip_regionname;
		strlcpy(de.geoip_elem.as_string, search,
			sizeof(de.geoip_elem.as_string));
	} else if (strcasecmp(stype, "city") == 0) {
		/* City name */
		subtype = dns_geoip_city_name;
		strlcpy(de.geoip_elem.as_string, search,
			sizeof(de.geoip_elem.as_string));
	} else if (strcasecmp(stype, "postal") == 0 ||
		   strcasecmp(stype, "postalcode") == 0)
	{
		if (len < 7) {
			subtype = dns_geoip_city_postalcode;
			strlcpy(de.geoip_elem.as_string, search,
				sizeof(de.geoip_elem.as_string));
		} else {
			cfg_obj_log(obj, lctx, ISC_LOG_ERROR,
				    "geoiop postal code (%s) too long",
				    search);
			return (ISC_R_FAILURE);
		}
	} else if (strcasecmp(stype, "metro") == 0 ||
		   strcasecmp(stype, "metrocode") == 0)
	{
		subtype = dns_geoip_city_metrocode;
		de.geoip_elem.as_int = atoi(search);
	} else if (strcasecmp(stype, "area") == 0 ||
		   strcasecmp(stype, "areacode") == 0)
	{
		subtype = dns_geoip_city_areacode;
		de.geoip_elem.as_int = atoi(search);
	} else if (strcasecmp(stype, "tz") == 0 ||
		   strcasecmp(stype, "timezone") == 0)
	{
		subtype = dns_geoip_city_timezonecode;
		strlcpy(de.geoip_elem.as_string, search,
			sizeof(de.geoip_elem.as_string));
	} else if (strcasecmp(stype, "continent") == 0 && len == 2) {
		/* Two-letter continent code */
		subtype = dns_geoip_city_continentcode;
		strlcpy(de.geoip_elem.as_string, search,
			sizeof(de.geoip_elem.as_string));
	} else if (strcasecmp(stype, "continent") == 0) {
		cfg_obj_log(obj, lctx, ISC_LOG_ERROR,
			    "geoiop continent code (%s) too long", search);
		return (ISC_R_FAILURE);
	} else if (strcasecmp(stype, "isp") == 0) {
		subtype = dns_geoip_isp_name;
		strlcpy(de.geoip_elem.as_string, search,
			sizeof(de.geoip_elem.as_string));
	} else if (strcasecmp(stype, "asnum") == 0) {
		subtype = dns_geoip_as_asnum;
		strlcpy(de.geoip_elem.as_string, search,
			sizeof(de.geoip_elem.as_string));
	} else if (strcasecmp(stype, "org") == 0) {
		subtype = dns_geoip_org_name;
		strlcpy(de.geoip_elem.as_string, search,
			sizeof(de.geoip_elem.as_string));
	} else if (strcasecmp(stype, "domain") == 0) {
		subtype = dns_geoip_domain_name;
		strlcpy(de.geoip_elem.as_string, search,
			sizeof(de.geoip_elem.as_string));
	} else if (strcasecmp(stype, "netspeed") == 0) {
		subtype = dns_geoip_netspeed_id;
		de.geoip_elem.as_int = atoi(search);
	} else
		INSIST(0);

	de.geoip_elem.subtype = get_subtype(obj, lctx, subtype, dbname);

	if (! geoip_can_answer(&de, ctx)) {
		cfg_obj_log(obj, lctx, ISC_LOG_ERROR,
			    "no GeoIP database installed which can answer "
			    "queries of type '%s'", stype);
		return (ISC_R_FAILURE);
	}

	*dep = de;

	return (ISC_R_SUCCESS);
}
#endif

isc_result_t
cfg_acl_fromconfig(const cfg_obj_t *caml, const cfg_obj_t *cctx,
		   isc_log_t *lctx, cfg_aclconfctx_t *ctx,
		   isc_mem_t *mctx, unsigned int nest_level,
		   dns_acl_t **target)
{
	return (cfg_acl_fromconfig2(caml, cctx, lctx, ctx, mctx,
				    nest_level, 0, target));
}

isc_result_t
cfg_acl_fromconfig2(const cfg_obj_t *caml, const cfg_obj_t *cctx,
		   isc_log_t *lctx, cfg_aclconfctx_t *ctx,
		   isc_mem_t *mctx, unsigned int nest_level,
		   isc_uint16_t family, dns_acl_t **target)
{
	isc_result_t result;
	dns_acl_t *dacl = NULL, *inneracl = NULL;
	dns_aclelement_t *de;
	const cfg_listelt_t *elt;
	dns_iptable_t *iptab;
	int new_nest_level = 0;

	if (nest_level != 0)
		new_nest_level = nest_level - 1;

	REQUIRE(target != NULL);
	REQUIRE(*target == NULL || DNS_ACL_VALID(*target));

	if (*target != NULL) {
		/*
		 * If target already points to an ACL, then we're being
		 * called recursively to configure a nested ACL.  The
		 * nested ACL's contents should just be absorbed into its
		 * parent ACL.
		 */
		dns_acl_attach(*target, &dacl);
		dns_acl_detach(target);
	} else {
		/*
		 * Need to allocate a new ACL structure.  Count the items
		 * in the ACL definition that will require space in the
		 * elements table.  (Note that if nest_level is nonzero,
		 * *everything* goes in the elements table.)
		 */
		isc_uint32_t nelem;

		if (nest_level == 0) {
			result = count_acl_elements(caml, cctx, lctx, ctx,
						    mctx, &nelem, NULL);
			if (result != ISC_R_SUCCESS)
				return (result);
		} else
			nelem = cfg_list_length(caml, ISC_FALSE);

		result = dns_acl_create(mctx, nelem, &dacl);
		if (result != ISC_R_SUCCESS)
			return (result);
	}

	de = dacl->elements;
	for (elt = cfg_list_first(caml);
	     elt != NULL;
	     elt = cfg_list_next(elt)) {
		const cfg_obj_t *ce = cfg_listelt_value(elt);
		isc_boolean_t neg = ISC_FALSE;

		INSIST(dacl->length <= dacl->alloc);

		if (cfg_obj_istuple(ce)) {
			/* Might be a negated element */
			const cfg_obj_t *negated =
				cfg_tuple_get(ce, "negated");
			if (! cfg_obj_isvoid(negated)) {
				neg = ISC_TRUE;
				dacl->has_negatives = ISC_TRUE;
				ce = negated;
			}
		}

		/*
		 * If nest_level is nonzero, then every element is
		 * to be stored as a separate, nested ACL rather than
		 * merged into the main iptable.
		 */
		iptab = dacl->iptable;

		if (nest_level != 0) {
			result = dns_acl_create(mctx,
						cfg_list_length(ce, ISC_FALSE),
						&de->nestedacl);
			if (result != ISC_R_SUCCESS)
				goto cleanup;
			iptab = de->nestedacl->iptable;
		}

		if (cfg_obj_isnetprefix(ce)) {
			/* Network prefix */
			isc_netaddr_t	addr;
			unsigned int	bitlen;

			cfg_obj_asnetprefix(ce, &addr, &bitlen);
			if (family != 0 && family != addr.family) {
				char buf[ISC_NETADDR_FORMATSIZE + 1];
				isc_netaddr_format(&addr, buf, sizeof(buf));
				cfg_obj_log(ce, lctx, ISC_LOG_WARNING,
					    "'%s': incorrect address family; "
					    "ignoring", buf);
				if (nest_level != 0)
					dns_acl_detach(&de->nestedacl);
				continue;
			}

			/*
			 * If nesting ACLs (nest_level != 0), we negate
			 * the nestedacl element, not the iptable entry.
			 */
			result = dns_iptable_addprefix(iptab, &addr, bitlen,
					      ISC_TF(nest_level != 0 || !neg));
			if (result != ISC_R_SUCCESS)
				goto cleanup;

			if (nest_level > 0) {
				INSIST(dacl->length < dacl->alloc);
				de->type = dns_aclelementtype_nestedacl;
				de->negative = neg;
			} else
				continue;
		} else if (cfg_obj_islist(ce)) {
			/*
			 * If we're nesting ACLs, put the nested
			 * ACL onto the elements list; otherwise
			 * merge it into *this* ACL.  We nest ACLs
			 * in two cases: 1) sortlist, 2) if the
			 * nested ACL contains negated members.
			 */
			if (inneracl != NULL)
				dns_acl_detach(&inneracl);
			result = cfg_acl_fromconfig(ce, cctx, lctx,
						    ctx, mctx, new_nest_level,
						    &inneracl);
			if (result != ISC_R_SUCCESS)
				goto cleanup;
nested_acl:
			if (nest_level > 0 || inneracl->has_negatives) {
				INSIST(dacl->length < dacl->alloc);
				de->type = dns_aclelementtype_nestedacl;
				de->negative = neg;
				if (de->nestedacl != NULL)
					dns_acl_detach(&de->nestedacl);
				dns_acl_attach(inneracl,
					       &de->nestedacl);
				dns_acl_detach(&inneracl);
				/* Fall through. */
			} else {
				INSIST(dacl->length + inneracl->length
				       <= dacl->alloc);
				dns_acl_merge(dacl, inneracl,
					      ISC_TF(!neg));
				de += inneracl->length;  /* elements added */
				dns_acl_detach(&inneracl);
				INSIST(dacl->length <= dacl->alloc);
				continue;
			}
		} else if (cfg_obj_istype(ce, &cfg_type_keyref)) {
			/* Key name. */
			INSIST(dacl->length < dacl->alloc);
			de->type = dns_aclelementtype_keyname;
			de->negative = neg;
			dns_name_init(&de->keyname, NULL);
			result = convert_keyname(ce, lctx, mctx,
						 &de->keyname);
			if (result != ISC_R_SUCCESS)
				goto cleanup;
#ifdef HAVE_GEOIP
		} else if (cfg_obj_istuple(ce) &&
			   cfg_obj_isvoid(cfg_tuple_get(ce, "negated")))
		{
			INSIST(dacl->length < dacl->alloc);
			result = parse_geoip_element(ce, lctx, ctx, de);
			if (result != ISC_R_SUCCESS)
				goto cleanup;
			de->type = dns_aclelementtype_geoip;
			de->negative = neg;
#endif /* HAVE_GEOIP */
		} else if (cfg_obj_isstring(ce)) {
			/* ACL name. */
			const char *name = cfg_obj_asstring(ce);
			if (strcasecmp(name, "any") == 0) {
				/* Iptable entry with zero bit length. */
				result = dns_iptable_addprefix(iptab, NULL, 0,
					      ISC_TF(nest_level != 0 || !neg));
				if (result != ISC_R_SUCCESS)
					goto cleanup;

				if (nest_level != 0) {
					INSIST(dacl->length < dacl->alloc);
					de->type = dns_aclelementtype_nestedacl;
					de->negative = neg;
				} else
					continue;
			} else if (strcasecmp(name, "none") == 0) {
				/* none == !any */
				/*
				 * We don't unconditional set
				 * dacl->has_negatives and
				 * de->negative to true so we can handle
				 * "!none;".
				 */
				result = dns_iptable_addprefix(iptab, NULL, 0,
					      ISC_TF(nest_level != 0 || neg));
				if (result != ISC_R_SUCCESS)
					goto cleanup;

				if (!neg)
					dacl->has_negatives = !neg;

				if (nest_level != 0) {
					INSIST(dacl->length < dacl->alloc);
					de->type = dns_aclelementtype_nestedacl;
					de->negative = !neg;
				} else
					continue;
			} else if (strcasecmp(name, "localhost") == 0) {
				INSIST(dacl->length < dacl->alloc);
				de->type = dns_aclelementtype_localhost;
				de->negative = neg;
			} else if (strcasecmp(name, "localnets") == 0) {
				INSIST(dacl->length < dacl->alloc);
				de->type = dns_aclelementtype_localnets;
				de->negative = neg;
			} else {
				if (inneracl != NULL)
					dns_acl_detach(&inneracl);
				/*
				 * This call should just find the cached
				 * of the named acl.
				 */
				result = convert_named_acl(ce, cctx, lctx, ctx,
							   mctx, new_nest_level,
							   &inneracl);
				if (result != ISC_R_SUCCESS)
					goto cleanup;

				goto nested_acl;
			}
		} else {
			cfg_obj_log(ce, lctx, ISC_LOG_WARNING,
				    "address match list contains "
				    "unsupported element type");
			result = ISC_R_FAILURE;
			goto cleanup;
		}

		/*
		 * This should only be reached for localhost, localnets
		 * and keyname elements, and nested ACLs if nest_level is
		 * nonzero (i.e., in sortlists).
		 */
		if (de->nestedacl != NULL &&
		    de->type != dns_aclelementtype_nestedacl)
			dns_acl_detach(&de->nestedacl);

		dacl->node_count++;
		de->node_num = dacl->node_count;

		dacl->length++;
		de++;
		INSIST(dacl->length <= dacl->alloc);
	}

	dns_acl_attach(dacl, target);
	result = ISC_R_SUCCESS;

 cleanup:
	if (inneracl != NULL)
		dns_acl_detach(&inneracl);
	dns_acl_detach(&dacl);
	return (result);
}
