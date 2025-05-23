/*	$NetBSD$	*/

/*
 * Copyright (C) 2002 Stichting NLnet, Netherlands, stichting@nlnet.nl.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND STICHTING NLNET
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * STICHTING NLNET BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
 * USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * The development of Dynamically Loadable Zones (DLZ) for Bind 9 was
 * conceived and contributed by Rob Butler.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ROB BUTLER
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * ROB BUTLER BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
 * USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Copyright (C) 2013  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2001  Internet Software Consortium.
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

/*
 * This provides the externally loadable filesystem DLZ module, without
 * update support
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/stat.h>

#include "dlz_minimal.h"
#include "dlz_list.h"
#include "dir.h"

typedef struct config_data {
	char		*basedir;
	int		basedirsize;
	char		*datadir;
	int		datadirsize;
	char		*xfrdir;
	int		xfrdirsize;
	int		splitcnt;
	char		separator;
	char		pathsep;

	/* Helper functions from the dlz_dlopen driver */
	log_t *log;
	dns_sdlz_putrr_t *putrr;
	dns_sdlz_putnamedrr_t *putnamedrr;
	dns_dlz_writeablezone_t *writeable_zone;
} config_data_t;

typedef struct dir_entry dir_entry_t;

struct dir_entry {
	char dirpath[DIR_PATHMAX];
	DLZ_LINK(dir_entry_t)	link;
};

typedef DLZ_LIST(dir_entry_t) dlist_t;

/* forward reference */

static void
b9_add_helper(struct config_data *cd, const char *helper_name, void *ptr);

/*
 * Private methods
 */
static isc_boolean_t
is_safe(const char *input) {
	unsigned int i;
	unsigned int len = strlen(input);

        /* check that only allowed characters are in the domain name */
	for (i = 0; i < len; i++) {
		/* '.' is allowed, but has special requirements */
		if (input[i] == '.') {
			/* '.' is not allowed as first char */
			if (i == 0)
				return (ISC_FALSE);
			/* '..', two dots together is not allowed. */
			else if (input[i-1] == '.')
				return (ISC_FALSE);
			/* '.' is not allowed as last char */
			if (i == len)
				return (ISC_FALSE);
			/* only 1 dot in ok location, continue at next char */
			continue;
		}
		/* '-' is allowed, continue at next char */
		if (input[i] == '-')
			continue;
		/* 0-9 is allowed, continue at next char */
		if (input[i] >= '0' && input[i] <= '9')
			continue;
		/* A-Z uppercase is allowed, continue at next char */
		if (input[i] >= 'A' && input[i] <= 'Z')
			continue;
		/* a-z lowercase is allowed, continue at next char */
		if (input[i] >= 'a' && input[i] <= 'z')
			continue;

		/*
		 * colon needs to be allowed for IPV6 client
		 * addresses.  Not dangerous in domain names, as not a
		 * special char.
		 */
		if (input[i] == ':')
			continue;

		/*
		 * '@' needs to be allowed for in zone data.  Not
		 * dangerous in domain names, as not a special char.
		 */
		if (input[i] == '@')
			continue;

		/*
		 * if we reach this point we have encountered a
		 * disallowed char!
		 */
		return (ISC_FALSE);
	}
        /* everything ok. */
	return (ISC_TRUE);
}

static isc_result_t
create_path_helper(char *out, const char *in, config_data_t *cd) {
	char *tmpString;
	char *tmpPtr;
	int i;

	tmpString = strdup(in);
	if (tmpString == NULL)
		return (ISC_R_NOMEMORY);

	/*
	 * don't forget is_safe guarantees '.' will NOT be the
	 * first/last char
	 */
	while ((tmpPtr = strrchr(tmpString, '.')) != NULL) {
		i = 0;
		while (tmpPtr[i+1] != '\0') {
			if (cd->splitcnt < 1)
				strcat(out, (char *) &tmpPtr[i+1]);
			else
				strncat(out, (char *) &tmpPtr[i+1],
					cd->splitcnt);
			strncat(out, (char *) &cd->pathsep, 1);
			if (cd->splitcnt == 0)
				break;
			if (strlen((char *) &tmpPtr[i+1]) <=
			    (unsigned int) cd->splitcnt)
				break;
			i += cd->splitcnt;
		}
		tmpPtr[0] = '\0';
	}

	/* handle the "first" label properly */
	i=0;
	tmpPtr = tmpString;
	while (tmpPtr[i] != '\0') {
		if (cd->splitcnt < 1)
			strcat(out, (char *) &tmpPtr[i]);
		else
			strncat(out, (char *) &tmpPtr[i], cd->splitcnt);
		strncat(out, (char *) &cd->pathsep, 1);
		if (cd->splitcnt == 0)
			break;
		if (strlen((char *) &tmpPtr[i]) <=
		    (unsigned int) cd->splitcnt)
			break;
		i += cd->splitcnt;
	}

	free(tmpString);
	return (ISC_R_SUCCESS);
}

/*%
 * Checks to make sure zone and host are safe.  If safe, then
 * hashes zone and host strings to build a path.  If zone / host
 * are not safe an error is returned.
 */

static isc_result_t
create_path(const char *zone, const char *host, const char *client,
	    config_data_t *cd, char **path)
{

	char *tmpPath;
	int pathsize;
	int len;
	isc_result_t result;
	isc_boolean_t isroot = ISC_FALSE;

	/* special case for root zone */
	if (strcmp(zone, ".") == 0)
		isroot = ISC_TRUE;

	/* if the requested zone is "unsafe", return error */
	if (!isroot && !is_safe(zone))
		return (ISC_R_FAILURE);

	/* if host was passed, verify that it is safe */
	if (host != NULL && !is_safe(host))
		return (ISC_R_FAILURE);

	/* if client was passed, verify that it is safe */
	if (client != NULL && !is_safe(client))
		return (ISC_R_FAILURE);

	/* Determine how much memory the split up string will require */
	if (host != NULL)
		len = strlen(zone) + strlen(host);
	else if (client != NULL)
		len = strlen(zone) + strlen(client);
	else
		len = strlen(zone);

	/*
	 * even though datadir and xfrdir will never be in the same
	 * string we only waste a few bytes by allocating for both,
	 * and then we are safe from buffer overruns.
	 */
	pathsize = len + cd->basedirsize +
		   cd->datadirsize + cd->xfrdirsize + 4;

	/* if we are splitting names, we will need extra space. */
	if (cd->splitcnt > 0)
		pathsize += len/cd->splitcnt;

	tmpPath = malloc(pathsize * sizeof(char));
	if (tmpPath == NULL) {
		/* write error message */
		cd->log(ISC_LOG_ERROR,
			"Filesystem driver unable to "
			"allocate memory in create_path().");
		result = ISC_R_NOMEMORY;
		goto cleanup_mem;
	}

	/*
	 * build path string.
	 * start out with base directory.
	 */
	strcpy(tmpPath, cd->basedir);

	/* add zone name - parsed properly */
	if (!isroot) {
		result = create_path_helper(tmpPath, zone, cd);
		if (result != ISC_R_SUCCESS)
			goto cleanup_mem;
	}

	/*
	 * When neither client or host is passed we are building a
	 * path to see if a zone is supported.  We require that a zone
	 * path have the "data dir" directory contained within it so
	 * that we know this zone is really supported.  Otherwise,
	 * this zone may not really be supported because we are
	 * supporting a delagated sub zone.
	 *
	 * Example:
	 *
	 * We are supporting long.domain.com and using a splitcnt of
	 * 0.  the base dir is "/base-dir/" and the data dir is
	 * "/.datadir" We want to see if we are authoritative for
	 * domain.com.  Path /base-dir/com/domain/.datadir since
	 * /base-dir/com/domain/.datadir does not exist, we are not
	 * authoritative for the domain "domain.com".  However we are
	 * authoritative for the domain "long.domain.com" because the
	 * path /base-dir/com/domain/long/.datadir does exist!
	 */

	/* if client is passed append xfr dir, otherwise append data dir */
	if (client != NULL) {
		strcat(tmpPath, cd->xfrdir);
		strncat(tmpPath, (char *) &cd->pathsep, 1);
		strcat(tmpPath, client);
	} else
		strcat(tmpPath, cd->datadir);

	/* if host not null, add it. */
	if (host != NULL) {
		strncat(tmpPath, (char *) &cd->pathsep, 1);
		result = create_path_helper(tmpPath, host, cd);
		if (result != ISC_R_SUCCESS)
			goto cleanup_mem;
	}

	/* return the path we built. */
	*path = tmpPath;

	/* return success */
	result = ISC_R_SUCCESS;

 cleanup_mem:
	/* cleanup memory */

	/* free tmpPath memory */
	if (tmpPath != NULL && result != ISC_R_SUCCESS)
		free(tmpPath);

	return (result);
}

static isc_result_t
process_dir(dir_t *dir, void *passback, config_data_t *cd,
	    dlist_t *dir_list, unsigned int basedirlen)
{

	char tmp[DIR_PATHMAX + DIR_NAMEMAX];
	int astPos;
	struct stat	sb;
	isc_result_t result = ISC_R_FAILURE;
	char *endp;
	char *type;
	char *ttlStr;
	char *data;
	char host[DIR_NAMEMAX];
	char *tmpString;
	char *tmpPtr;
	int ttl;
	int i;
	int len;
	dir_entry_t *direntry;
	isc_boolean_t foundHost;

	tmp[0] = '\0'; /* set 1st byte to '\0' so strcpy works right. */
	host[0] = '\0';
	foundHost = ISC_FALSE;

	/* copy base directory name to tmp. */
	strcpy(tmp, dir->dirname);

	/* dir->dirname will always have '*' as the last char. */
	astPos = strlen(dir->dirname) - 1;

	/* if dir_list != NULL, were are performing a zone xfr */
	if (dir_list != NULL) {
		/* if splitcnt == 0, determine host from path. */
		if (cd->splitcnt == 0) {
			if (strlen(tmp) - 3 > basedirlen) {
				tmp[astPos-1] = '\0';
				tmpString = (char *) &tmp[basedirlen+1];
				/* handle filesystem's special wildcard "-"  */
				if (strcmp(tmpString, "-") == 0) {
					strcpy(host, "*");
				} else {
					/*
					 * not special wildcard -- normal name
					 */
					while ((tmpPtr = strrchr(tmpString,
								 cd->pathsep))
					       != NULL)
					{
						if ((strlen(host) +
						     strlen(tmpPtr + 1) + 2)
						    > DIR_NAMEMAX)
							continue;
						strcat(host, tmpPtr + 1);
						strcat(host, ".");
						tmpPtr[0] = '\0';
					}
					if ((strlen(host) +
					     strlen(tmpString) + 1)
					    <= DIR_NAMEMAX)
						strcat(host, tmpString);
				}

				foundHost = ISC_TRUE;
				/* set tmp again for use later */
				strcpy(tmp, dir->dirname);
			}
		} else {
			/*
			 * if splitcnt != 0 determine host from
			 * ".host" directory entry
			 */
			while (dir_read(dir) == ISC_R_SUCCESS) {
				if (strncasecmp(".host",
						dir->entry.name, 5) == 0) {
					/*
					 * handle filesystem's special
					 * wildcard "-"
					 */
					if (strcmp((char *) &dir->entry.name[6],
						   "-") == 0)
						strcpy(host, "*");
					else {
						strncpy(host,
						   (char *) &dir->entry.name[6],
						   sizeof(host) - 1);
						host[255] = '\0';
					}
					foundHost = ISC_TRUE;
					break;
				}
			}
			/* reset dir list for use later */
			dir_reset(dir);
		} /* end of else */
	}

	while (dir_read(dir) == ISC_R_SUCCESS) {
		cd->log(ISC_LOG_DEBUG(1),
			"Filesystem driver Dir name:"
			" '%s' Dir entry: '%s'\n",
			dir->dirname, dir->entry.name);

		/* skip any entries starting with "." */
		if (dir->entry.name[0] == '.')
			continue;

		/*
		 * get rid of '*', set to NULL.  Effectively trims
		 * string from previous loop to base directory only
		 * while still leaving memory for concat to be
		 * performed next.
		 */

		tmp[astPos] = '\0';

		/* add name to base directory name. */
		strcat(tmp, dir->entry.name);

		/* make sure we can stat entry */
		if (stat(tmp, &sb) == 0 ) {
			/* if entry is a directory */
			if ((sb.st_mode & S_IFDIR) != 0) {
				/*
				 * if dir list is NOT NULL, add dir to
				 * dir list
				 */
				if (dir_list != NULL) {
					direntry = malloc(sizeof(dir_entry_t));
					if (direntry == NULL)
						return (ISC_R_NOMEMORY);
					strcpy(direntry->dirpath, tmp);
					DLZ_LINK_INIT(direntry, link);
					DLZ_LIST_APPEND(*dir_list, direntry,
							link);
					result = ISC_R_SUCCESS;
				}
				continue;

				/*
				 * if entry is a file be sure we do
				 * not add entry to DNS results if we
				 * are performing a zone xfr and we
				 * could not find a host entry.
				 */

			} else if (dir_list != NULL &&
				   foundHost == ISC_FALSE) {
				continue;
			}
		} else /* if we cannot stat entry, skip it. */
			continue;

		type = dir->entry.name;
		ttlStr = strchr(type,  cd->separator);
		if (ttlStr == NULL) {
			cd->log(ISC_LOG_ERROR,
				"Filesystem driver: "
				"%s could not be parsed properly", tmp);
			return (ISC_R_FAILURE);
		}

		/* replace separator char with NULL to split string */
		ttlStr[0] = '\0';
		/* start string after NULL of previous string */
		ttlStr = (char *) &ttlStr[1];

		data = strchr(ttlStr, cd->separator);
		if (data == NULL) {
			cd->log(ISC_LOG_ERROR,
				"Filesystem driver: "
				"%s could not be parsed properly", tmp);
			return (ISC_R_FAILURE);
		}

		/* replace separator char with NULL to split string */
		data[0] = '\0';

		/* start string after NULL of previous string */
		data = (char *) &data[1];

		/* replace all cd->separator chars with a space. */
		len = strlen(data);

		for (i=0; i < len; i++) {
			if (data[i] == cd->separator)
				data[i] = ' ';
		}

		/* convert text to int, make sure it worked right */
		ttl = strtol(ttlStr, &endp, 10);
		if (*endp != '\0' || ttl < 0)
			cd->log(ISC_LOG_ERROR,
				"Filesystem driver "
				"ttl must be a postive number");

		/* pass data back to Bind */
		if (dir_list == NULL)
			result = cd->putrr((dns_sdlzlookup_t *) passback,
					   type, ttl, data);
		else
			result = cd->putnamedrr((dns_sdlzallnodes_t *) passback,
						(char *) host,
						type, ttl, data);

		/* if error, return error right away */
		if (result != ISC_R_SUCCESS)
			return (result);
	} /* end of while loop */

	return (result);
}

/*
 * DLZ methods
 */
isc_result_t
dlz_allowzonexfr(void *dbdata, const char *name, const char *client) {
	isc_result_t result;
	char *path;
	struct stat	sb;
	config_data_t *cd;
	path = NULL;

	cd = (config_data_t *) dbdata;

	if (create_path(name, NULL, client, cd, &path) != ISC_R_SUCCESS) {
		return (ISC_R_NOTFOUND);
	}

	if (stat(path, &sb) != 0) {
		result = ISC_R_NOTFOUND;
		goto complete_AXFR;
	}

	if ((sb.st_mode & S_IFREG) != 0) {
		result = ISC_R_SUCCESS;
		goto complete_AXFR;
	}

	result = ISC_R_NOTFOUND;

 complete_AXFR:
	free(path);
	return (result);
}

isc_result_t
dlz_allnodes(const char *zone, void *dbdata, dns_sdlzallnodes_t *allnodes) {
	isc_result_t result;
	dlist_t *dir_list;
	config_data_t *cd = (config_data_t *) dbdata;
	char *basepath;
	unsigned int basepathlen;
	struct stat	sb;
	dir_t dir;
	dir_entry_t *dir_entry;
	dir_entry_t *next_de;

	basepath = NULL;
	dir_list = NULL;

	/* allocate memory for list */
	dir_list = malloc(sizeof(dlist_t));
	if (dir_list == NULL) {
		result = ISC_R_NOTFOUND;
		goto complete_allnds;
	}

	/* initialize list */
	DLZ_LIST_INIT(*dir_list);

	if (create_path(zone, NULL, NULL, cd, &basepath) != ISC_R_SUCCESS) {
		result = ISC_R_NOTFOUND;
		goto complete_allnds;
	}

	/* remove path separator at end of path so stat works properly */
	basepathlen = strlen(basepath);

	if (stat(basepath, &sb) != 0) {
		result = ISC_R_NOTFOUND;
		goto complete_allnds;
	}

	if ((sb.st_mode & S_IFDIR) == 0) {
		result = ISC_R_NOTFOUND;
		goto complete_allnds;
	}

	/* initialize and open directory */
	dir_init(&dir);
	result = dir_open(&dir, basepath);

	/* if directory open failed, return error. */
	if (result != ISC_R_SUCCESS) {
		cd->log(ISC_LOG_ERROR,
			"Unable to open %s directory to read entries.",
			basepath);
		result = ISC_R_FAILURE;
		goto complete_allnds;
	}

	/* process the directory */
	result = process_dir(&dir, allnodes, cd, dir_list, basepathlen);

	/* close the directory */
	dir_close(&dir);

	if (result != ISC_R_SUCCESS)
		goto complete_allnds;

	/* get first dir entry from list. */
	dir_entry = DLZ_LIST_HEAD(*dir_list);
	while (dir_entry != NULL) {
		result = dir_open(&dir, dir_entry->dirpath);
		/* if directory open failed, return error. */
		if (result != ISC_R_SUCCESS) {
			cd->log(ISC_LOG_ERROR,
				"Unable to open %s "
				"directory to read entries.", basepath);
			result = ISC_R_FAILURE;
			goto complete_allnds;
		}

		/* process the directory */
		result = process_dir(&dir, allnodes, cd, dir_list, basepathlen);

		/* close the directory */
		dir_close(&dir);

		if (result != ISC_R_SUCCESS)
			goto complete_allnds;

		dir_entry = DLZ_LIST_NEXT(dir_entry, link);
	} /* end while */

 complete_allnds:
	if (dir_list != NULL) {
		/* clean up entries from list. */
		dir_entry = DLZ_LIST_HEAD(*dir_list);
		while (dir_entry != NULL) {
			next_de = DLZ_LIST_NEXT(dir_entry, link);
			free(dir_entry);
			dir_entry = next_de;
		} /* end while */
		free(dir_list);
	}

	if (basepath != NULL)
		free(basepath);

	return (result);
}

#if DLZ_DLOPEN_VERSION < 3
isc_result_t
dlz_findzonedb(void *dbdata, const char *name)
#else
isc_result_t
dlz_findzonedb(void *dbdata, const char *name,
	       dns_clientinfomethods_t *methods,
	       dns_clientinfo_t *clientinfo)
#endif
{

	isc_result_t result;
	config_data_t *cd = (config_data_t *) dbdata;
	char *path;
	struct stat sb;
	path = NULL;

#if DLZ_DLOPEN_VERSION >= 3
	UNUSED(methods);
	UNUSED(clientinfo);
#endif

	if (create_path(name, NULL, NULL, cd, &path) != ISC_R_SUCCESS)
		return (ISC_R_NOTFOUND);

	cd->log(ISC_LOG_DEBUG(1),
		"Filesystem driver Findzone() Checking for path: '%s'\n", path);

	if (stat(path, &sb) != 0) {
		result = ISC_R_NOTFOUND;
		goto complete_FZ;
	}

	if ((sb.st_mode & S_IFDIR) != 0) {
		result = ISC_R_SUCCESS;
		goto complete_FZ;
	}

	result = ISC_R_NOTFOUND;

 complete_FZ:

	free(path);
	return (result);
}

#if DLZ_DLOPEN_VERSION == 1
isc_result_t
dlz_lookup(const char *zone, const char *name,
	   void *dbdata, dns_sdlzlookup_t *lookup)
#else
isc_result_t
dlz_lookup(const char *zone, const char *name,
	   void *dbdata, dns_sdlzlookup_t *lookup,
	   dns_clientinfomethods_t *methods,
	   dns_clientinfo_t *clientinfo)
#endif
{
	isc_result_t result = ISC_R_NOTFOUND;
	config_data_t *cd = (config_data_t *) dbdata;
	char *path;
	struct stat sb;
	dir_t dir;
	path = NULL;

	UNUSED(lookup);
#if DLZ_DLOPEN_VERSION >= 2
	UNUSED(methods);
	UNUSED(clientinfo);
#endif

	if (strcmp(name, "*") == 0)
		/*
		 * handle filesystem's special wildcard "-"
		 */
		result = create_path(zone, "-", NULL, cd, &path);
	else
		result = create_path(zone, name, NULL, cd, &path);

	if (result != ISC_R_SUCCESS)
		return (ISC_R_NOTFOUND);

	/* remove path separator at end of path so stat works properly */
	path[strlen(path)-1] = '\0';

	cd->log(ISC_LOG_DEBUG(1),
		"Filesystem driver lookup() Checking for path: '%s'\n", path);

	if (stat(path, &sb) != 0) {
		result = ISC_R_NOTFOUND;
		goto complete_lkup;
	}

	if ((sb.st_mode & S_IFDIR) == 0) {
		result = ISC_R_NOTFOUND;
		goto complete_lkup;
	}

	/* initialize and open directory */
	dir_init(&dir);
	result = dir_open(&dir, path);

	/* if directory open failed, return error. */
	if (result != ISC_R_SUCCESS) {
		cd->log(ISC_LOG_ERROR,
			"Unable to open %s directory to read entries.", path);
		result = ISC_R_FAILURE;
		goto complete_lkup;
	}

	/* process any records in the directory */
	result = process_dir(&dir, lookup, cd, NULL, 0);

	/* close the directory */
	dir_close(&dir);

 complete_lkup:

	free(path);
	return (result);
}

isc_result_t
dlz_create(const char *dlzname, unsigned int argc, char *argv[],
	   void **dbdata, ...)
{
	config_data_t *cd;
	char *endp;
	int len;
	char pathsep;
	const char *helper_name;
	va_list ap;

	UNUSED(dlzname);

	/* allocate memory for our config data and helper functions */
	cd = calloc(1, sizeof(config_data_t));
	if (cd == NULL)
		goto no_mem;

	/* zero the memory */
	memset(cd, 0, sizeof(config_data_t));

	/* Fill in the helper functions */
	va_start(ap, dbdata);
	while ((helper_name = va_arg(ap, const char*)) != NULL)
		b9_add_helper(cd, helper_name, va_arg(ap, void*));
	va_end(ap);

	/* we require 5 command line args. */
	if (argc != 6) {
		cd->log(ISC_LOG_ERROR,
			"Filesystem driver requires "
			"6 command line args.");
		result = ISC_R_FAILURE;
		goto free_cd;
	}

	if (strlen(argv[5]) > 1) {
		cd->log(ISC_LOG_ERROR,
			"Filesystem driver can only "
			"accept a single character for separator.");
		result = ISC_R_FAILURE;
		goto free_cd;
	}

	/* verify base dir ends with '/' or '\' */
	len = strlen(argv[1]);
	if (argv[1][len-1] != '\\' && argv[1][len-1] != '/') {
		cd->log(ISC_LOG_ERROR,
			"Base dir parameter for filesystem driver "
			"should end with %s",
			"either '/' or '\\' ");
		result = ISC_R_FAILURE;
		goto free_cd;
	}

	/* determine and save path separator for later */
	if (argv[1][len-1] == '\\')
		pathsep = '\\';
	else
		pathsep = '/';

	cd->pathsep = pathsep;

	/* get and store our base directory */
	cd->basedir = strdup(argv[1]);
	if (cd->basedir == NULL)
		goto no_mem;
	cd->basedirsize = strlen(cd->basedir);

	/* get and store our data sub-dir */
	cd->datadir = strdup(argv[2]);
	if (cd->datadir == NULL)
		goto no_mem;
	cd->datadirsize = strlen(cd->datadir);

	/* get and store our zone xfr sub-dir */
	cd->xfrdir = strdup(argv[3]);
	if (cd->xfrdir == NULL)
		goto no_mem;
	cd->xfrdirsize = strlen(cd->xfrdir);

	/* get and store our directory split count */
	cd->splitcnt = strtol(argv[4], &endp, 10);
	if (*endp != '\0' || cd->splitcnt < 0)
		cd->log(ISC_LOG_ERROR,
			"Directory split count must be zero (0) "
			"or a postive number");

	/* get and store our separator character */
	cd->separator = *argv[5];

	/* pass back config data */
	*dbdata = cd;

	/* return success */
	return (ISC_R_SUCCESS);

	/* handle no memory error */
 no_mem:

	/* write error message */
	if (cd != NULL && cd->log != NULL)
		cd->log(ISC_LOG_ERROR,
			"filesystem_dynamic: Filesystem driver unable to "
			"allocate memory for config data.");

 free_cd:
	/* if we allocated a config data object clean it up */
	if (cd != NULL)
		dlz_destroy(cd);

	/* return error */
	return (ISC_R_NOMEMORY);
}

void
dlz_destroy(void *dbdata) {
	config_data_t *cd;

	cd = (config_data_t *) dbdata;

	/*
	 * free memory for each section of config data that was
	 * allocated
	 */
	if (cd->basedir != NULL)
		free(cd->basedir);

	if (cd->datadir != NULL)
		free(cd->datadir);

	if (cd->xfrdir != NULL)
		free(cd->xfrdir);

	/* free config data memory */
	free(cd);
}

/*
 * Return the version of the API
 */
int
dlz_version(unsigned int *flags) {
	UNUSED(flags);
	return (DLZ_DLOPEN_VERSION);
}

/*
 * Register a helper function from the bind9 dlz_dlopen driver
 */
static void
b9_add_helper(struct config_data *cd, const char *helper_name, void *ptr) {
	if (strcmp(helper_name, "log") == 0)
		cd->log = (log_t *)ptr;
	if (strcmp(helper_name, "putrr") == 0)
		cd->putrr = (dns_sdlz_putrr_t *)ptr;
	if (strcmp(helper_name, "putnamedrr") == 0)
		cd->putnamedrr = (dns_sdlz_putnamedrr_t *)ptr;
	if (strcmp(helper_name, "writeable_zone") == 0)
		cd->writeable_zone = (dns_dlz_writeablezone_t *)ptr;
}
