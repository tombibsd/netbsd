/*	$NetBSD$	*/
/*-
 * Copyright (c) 2011 Abhinav Upadhyay <er.abhinav.upadhyay@gmail.com>
 * All rights reserved.
 *
 * This code was developed as part of Google's Summer of Code 2011 program.
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

#include <sys/queue.h>
#include <sys/stat.h>

#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>
#include <zlib.h>
#include <term.h>
#undef tab	// XXX: manconf.h

#include "apropos-utils.h"
#include "manconf.h"

typedef struct orig_callback_data {
	void *data;
	int (*callback) (void *, const char *, const char *, const char *,
		const char *, size_t);
} orig_callback_data;

typedef struct inverse_document_frequency {
	double value;
	int status;
} inverse_document_frequency;

/* weights for individual columns */
static const double col_weights[] = {
	2.0,	// NAME
	2.00,	// Name-description
	0.55,	// DESCRIPTION
	0.10,	// LIBRARY
	0.001,	//RETURN VALUES
	0.20,	//ENVIRONMENT
	0.01,	//FILES
	0.001,	//EXIT STATUS
	2.00,	//DIAGNOSTICS
	0.05,	//ERRORS
	0.00,	//md5_hash
	1.00	//machine
};

/*
 * lower --
 *  Converts the string str to lower case
 */
char *
lower(char *str)
{
	assert(str);
	int i = 0;
	char c;
	while (str[i] != '\0') {
		c = tolower((unsigned char) str[i]);
		str[i++] = c;
	}
	return str;
}

/*
* concat--
*  Utility function. Concatenates together: dst, a space character and src.
* dst + " " + src
*/
void
concat(char **dst, const char *src)
{
	concat2(dst, src, strlen(src));
}

void
concat2(char **dst, const char *src, size_t srclen)
{
	size_t total_len, dst_len;
	assert(src != NULL);

	/*
	 * If destination buffer dst is NULL, then simply
	 * strdup the source buffer
	 */
	if (*dst == NULL) {
		*dst = estrdup(src);
		return;
	}

	dst_len = strlen(*dst);
	/*
	 * NUL Byte and separator space
	 */
	total_len = dst_len + srclen + 2;

	*dst = erealloc(*dst, total_len);

	/* Append a space at the end of dst */
	(*dst)[dst_len++] = ' ';

	/* Now, copy src at the end of dst */
	memcpy(*dst + dst_len, src, srclen + 1);
}

void
close_db(sqlite3 *db)
{
	sqlite3_close(db);
	sqlite3_shutdown();
}

/*
 * create_db --
 *  Creates the database schema.
 */
static int
create_db(sqlite3 *db)
{
	const char *sqlstr = NULL;
	char *schemasql;
	char *errmsg = NULL;

/*------------------------ Create the tables------------------------------*/

#if NOTYET
	sqlite3_exec(db, "PRAGMA journal_mode = WAL", NULL, NULL, NULL);
#else
	sqlite3_exec(db, "PRAGMA journal_mode = DELETE", NULL, NULL, NULL);
#endif

	schemasql = sqlite3_mprintf("PRAGMA user_version = %d",
	    APROPOS_SCHEMA_VERSION);
	sqlite3_exec(db, schemasql, NULL, NULL, &errmsg);
	if (errmsg != NULL)
		goto out;
	sqlite3_free(schemasql);

	sqlstr =
	    //mandb
	    "CREATE VIRTUAL TABLE mandb USING fts4(section, name, "
		"name_desc, desc, lib, return_vals, env, files, "
		"exit_status, diagnostics, errors, md5_hash UNIQUE, machine, "
		"compress=zip, uncompress=unzip, tokenize=porter); "
	    //mandb_meta
	    "CREATE TABLE IF NOT EXISTS mandb_meta(device, inode, mtime, "
		"file UNIQUE, md5_hash UNIQUE, id  INTEGER PRIMARY KEY); "
	    //mandb_links
	    "CREATE TABLE IF NOT EXISTS mandb_links(link, target, section, "
		"machine, md5_hash); ";

	sqlite3_exec(db, sqlstr, NULL, NULL, &errmsg);
	if (errmsg != NULL)
		goto out;

	sqlstr =
	    "CREATE INDEX IF NOT EXISTS index_mandb_links ON mandb_links "
		"(link); "
	    "CREATE INDEX IF NOT EXISTS index_mandb_meta_dev ON mandb_meta "
		"(device, inode); "
	    "CREATE INDEX IF NOT EXISTS index_mandb_links_md5 ON mandb_links "
		"(md5_hash);";
	sqlite3_exec(db, sqlstr, NULL, NULL, &errmsg);
	if (errmsg != NULL)
		goto out;
	return 0;

out:
	warnx("%s", errmsg);
	free(errmsg);
	sqlite3_close(db);
	sqlite3_shutdown();
	return -1;
}

/*
 * zip --
 *  User defined Sqlite function to compress the FTS table
 */
static void
zip(sqlite3_context *pctx, int nval, sqlite3_value **apval)
{
	int nin;
	long int nout;
	const unsigned char * inbuf;
	unsigned char *outbuf;

	assert(nval == 1);
	nin = sqlite3_value_bytes(apval[0]);
	inbuf = (const unsigned char *) sqlite3_value_blob(apval[0]);
	nout = nin + 13 + (nin + 999) / 1000;
	outbuf = emalloc(nout);
	compress(outbuf, (unsigned long *) &nout, inbuf, nin);
	sqlite3_result_blob(pctx, outbuf, nout, free);
}

/*
 * unzip --
 *  User defined Sqlite function to uncompress the FTS table.
 */
static void
unzip(sqlite3_context *pctx, int nval, sqlite3_value **apval)
{
	unsigned int rc;
	unsigned char *outbuf;
	z_stream stream;

	assert(nval == 1);
	stream.next_in = __UNCONST(sqlite3_value_blob(apval[0]));
	stream.avail_in = sqlite3_value_bytes(apval[0]);
	stream.avail_out = stream.avail_in * 2 + 100;
	stream.next_out = outbuf = emalloc(stream.avail_out);
	stream.zalloc = NULL;
	stream.zfree = NULL;

	if (inflateInit(&stream) != Z_OK) {
		free(outbuf);
		return;
	}

	while ((rc = inflate(&stream, Z_SYNC_FLUSH)) != Z_STREAM_END) {
		if (rc != Z_OK ||
		    (stream.avail_out != 0 && stream.avail_in == 0)) {
			free(outbuf);
			return;
		}
		outbuf = erealloc(outbuf, stream.total_out * 2);
		stream.next_out = outbuf + stream.total_out;
		stream.avail_out = stream.total_out;
	}
	if (inflateEnd(&stream) != Z_OK) {
		free(outbuf);
		return;
	}
	outbuf = erealloc(outbuf, stream.total_out);
	sqlite3_result_text(pctx, (const char *)outbuf, stream.total_out, free);
}

/*
 * get_dbpath --
 *   Read the path of the database from man.conf and return.
 */
char *
get_dbpath(const char *manconf)
{
	TAG *tp;
	char *dbpath;

	config(manconf);
	tp = gettag("_mandb", 1);
	if (!tp)
		return NULL;

	if (TAILQ_EMPTY(&tp->entrylist))
		return NULL;

	dbpath = TAILQ_LAST(&tp->entrylist, tqh)->s;
	return dbpath;
}

/* init_db --
 *   Prepare the database. Register the compress/uncompress functions and the
 *   stopword tokenizer.
 *	 db_flag specifies the mode in which to open the database. 3 options are
 *   available:
 *   	1. DB_READONLY: Open in READONLY mode. An error if db does not exist.
 *  	2. DB_READWRITE: Open in read-write mode. An error if db does not exist.
 *  	3. DB_CREATE: Open in read-write mode. It will try to create the db if
 *			it does not exist already.
 *  RETURN VALUES:
 *		The function will return NULL in case the db does not exist
 *		and DB_CREATE
 *  	was not specified. And in case DB_CREATE was specified and yet NULL is
 *  	returned, then there was some other error.
 *  	In normal cases the function should return a handle to the db.
 */
sqlite3 *
init_db(mandb_access_mode db_flag, const char *manconf)
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt;
	struct stat sb;
	int rc;
	int create_db_flag = 0;

	char *dbpath = get_dbpath(manconf);
	if (dbpath == NULL)
		errx(EXIT_FAILURE, "_mandb entry not found in man.conf");

	if (!(stat(dbpath, &sb) == 0 && S_ISREG(sb.st_mode))) {
		/* Database does not exist, check if DB_CREATE was specified,
		 * and set flag to create the database schema
		 */
		if (db_flag != (MANDB_CREATE)) {
			warnx("Missing apropos database. "
			      "Please run makemandb to create it.");
			return NULL;
		}
		create_db_flag = 1;
	} else {
		/*
		 * Database exists. Check if we have the permissions
		 * to read/write the files
		 */
		int access_mode = R_OK;
		switch (db_flag) {
		case MANDB_CREATE:
		case MANDB_WRITE:
			access_mode |= W_OK;
			break;
		default:
			break;
		}
		if ((access(dbpath, access_mode)) != 0) {
			warnx("Unable to access the database, please check"
			    " permissions for `%s'", dbpath);
			return NULL;
		}
	}

	sqlite3_initialize();
	rc = sqlite3_open_v2(dbpath, &db, db_flag, NULL);

	if (rc != SQLITE_OK) {
		warnx("%s", sqlite3_errmsg(db));
		goto error;
	}

	if (create_db_flag && create_db(db) < 0) {
		warnx("%s", "Unable to create database schema");
		goto error;
	}

	rc = sqlite3_prepare_v2(db, "PRAGMA user_version", -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		warnx("Unable to query schema version: %s",
		    sqlite3_errmsg(db));
		goto error;
	}
	if (sqlite3_step(stmt) != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		warnx("Unable to query schema version: %s",
		    sqlite3_errmsg(db));
		goto error;
	}
	if (sqlite3_column_int(stmt, 0) != APROPOS_SCHEMA_VERSION) {
		sqlite3_finalize(stmt);
		warnx("Incorrect schema version found. "
		      "Please run makemandb -f.");
		goto error;
	}
	sqlite3_finalize(stmt);

	sqlite3_extended_result_codes(db, 1);

	/* Register the zip and unzip functions for FTS compression */
	rc = sqlite3_create_function(db, "zip", 1, SQLITE_ANY, NULL, zip,
	    NULL, NULL);
	if (rc != SQLITE_OK) {
		warnx("Unable to register function: compress: %s",
		    sqlite3_errmsg(db));
		goto error;
	}

	rc = sqlite3_create_function(db, "unzip", 1, SQLITE_ANY, NULL,
                                 unzip, NULL, NULL);
	if (rc != SQLITE_OK) {
		warnx("Unable to register function: uncompress: %s",
		    sqlite3_errmsg(db));
		goto error;
	}
	return db;

error:
	close_db(db);
	return NULL;
}

/*
 * rank_func --
 *  Sqlite user defined function for ranking the documents.
 *  For each phrase of the query, it computes the tf and idf and adds them over.
 *  It computes the final rank, by multiplying tf and idf together.
 *  Weight of term t for document d = (term frequency of t in d *
 *                                      inverse document frequency of t)
 *
 *  Term Frequency of term t in document d = Number of times t occurs in d /
 *	Number of times t appears in all documents
 *
 *  Inverse document frequency of t = log(Total number of documents /
 *										Number of documents in which t occurs)
 */
static void
rank_func(sqlite3_context *pctx, int nval, sqlite3_value **apval)
{
	inverse_document_frequency *idf = sqlite3_user_data(pctx);
	double tf = 0.0;
	const unsigned int *matchinfo;
	int ncol;
	int nphrase;
	int iphrase;
	int ndoc;
	int doclen = 0;
	const double k = 3.75;
	/*
	 * Check that the number of arguments passed to this
	 * function is correct.
	 */
	assert(nval == 1);

	matchinfo = (const unsigned int *) sqlite3_value_blob(apval[0]);
	nphrase = matchinfo[0];
	ncol = matchinfo[1];
	ndoc = matchinfo[2 + 3 * ncol * nphrase + ncol];
	for (iphrase = 0; iphrase < nphrase; iphrase++) {
		int icol;
		const unsigned int *phraseinfo =
		    &matchinfo[2 + ncol + iphrase * ncol * 3];
		for(icol = 1; icol < ncol; icol++) {

			/* nhitcount: number of times the current phrase occurs
			 * 	in the current column in the current document.
			 * nglobalhitcount: number of times current phrase
			 *	occurs in the current column in all documents.
			 * ndocshitcount: number of documents in which the
			 *	current phrase occurs in the current column at
			 *	least once.
			 */
  			int nhitcount = phraseinfo[3 * icol];
			int nglobalhitcount = phraseinfo[3 * icol + 1];
			int ndocshitcount = phraseinfo[3 * icol + 2];
			doclen = matchinfo[2 + icol ];
			double weight = col_weights[icol - 1];
			if (idf->status == 0 && ndocshitcount)
				idf->value +=
				    log(((double)ndoc / ndocshitcount))* weight;

			/*
			 * Dividing the tf by document length to normalize
			 * the effect of longer documents.
			 */
			if (nglobalhitcount > 0 && nhitcount)
				tf += (((double)nhitcount  * weight)
				    / (nglobalhitcount * doclen));
		}
	}
	idf->status = 1;

	/*
	 * Final score: Dividing by k + tf further normalizes the weight
	 * leading to better results. The value of k is experimental
	 */
	double score = (tf * idf->value) / (k + tf);
	sqlite3_result_double(pctx, score);
	return;
}

/*
 * generates sql query for matching the user entered query
 */
static char *
generate_search_query(query_args *args, const char *snippet_args[3])
{
	const char *default_snippet_args[3];
	char *section_clause = NULL;
	char *limit_clause = NULL;
	char *machine_clause = NULL;
	char *query;

	if (args->machine)
		easprintf(&machine_clause, "AND machine = \'%s\' ",
		    args->machine);


	/* We want to build a query of the form: "select x,y,z from mandb where
	 * mandb match :query [AND (section LIKE '1' OR section LIKE '2' OR...)]
	 * ORDER BY rank DESC..."
	 * NOTES:
	 *   1. The portion in square brackets is optional, it will be there
	 *      only if the user has specified an option on the command line
	 *      to search in one or more specific sections.
	 */
	char *sections_str = args->sec_nums;
	char *temp;
	if (sections_str) {
		while (*sections_str) {
			size_t len = strcspn(sections_str, " ");
			char *sec = sections_str;
			if (sections_str[len] == 0) {
				sections_str += len;
			} else {
				sections_str[len] = 0;
				sections_str += len + 1;
			}
			easprintf(&temp, "\'%s\',", sec);

			if (section_clause) {
				concat(&section_clause, temp);
				free(temp);
			} else {
				section_clause = temp;
			}
		}
		if (section_clause) {
			/*
			 * At least one section requested, add glue for query.
			 * Before doing that, remove the comma at the end of
			 * section_clause
			 */
			size_t section_clause_len = strlen(section_clause);
			if (section_clause[section_clause_len - 1] == ',')
				section_clause[section_clause_len - 1] = 0;
			temp = section_clause;
			easprintf(&section_clause, " AND section IN (%s)", temp);
			free(temp);
		}
	}

	if (args->nrec >= 0) {
		/* Use the provided number of records and offset */
		easprintf(&limit_clause, " LIMIT %d OFFSET %d",
		    args->nrec, args->offset);
	}

	if (snippet_args == NULL) {
		default_snippet_args[0] = "";
		default_snippet_args[1] = "";
		default_snippet_args[2] = "...";
		snippet_args = default_snippet_args;
	}

	if (args->legacy) {
	    char *wild;
	    easprintf(&wild, "%%%s%%", args->search_str);
	    query = sqlite3_mprintf("SELECT section, name, name_desc, machine"
		" FROM mandb"
		" WHERE name LIKE %Q OR name_desc LIKE %Q "
		"%s"
		"%s",
		wild, wild,
		section_clause ? section_clause : "",
		limit_clause ? limit_clause : "");
		free(wild);
	} else {
	    query = sqlite3_mprintf("SELECT section, name, name_desc, machine,"
		" snippet(mandb, %Q, %Q, %Q, -1, 40 ),"
		" rank_func(matchinfo(mandb, \"pclxn\")) AS rank"
		" FROM mandb"
		" WHERE mandb MATCH %Q %s "
		"%s"
		" ORDER BY rank DESC"
		"%s",
		snippet_args[0], snippet_args[1], snippet_args[2],
		args->search_str, machine_clause ? machine_clause : "",
		section_clause ? section_clause : "",
		limit_clause ? limit_clause : "");
	}

	free(machine_clause);
	free(section_clause);
	free(limit_clause);
	return query;
}

/*
 * Execute the full text search query and return the number of results
 * obtained.
 */
static unsigned int
execute_search_query(sqlite3 *db, char *query, query_args *args)
{
	sqlite3_stmt *stmt;
	const char *section;
	char *name;
	char *slash_ptr;
	const char *name_desc;
	const char *machine;
	const char *snippet = "";
	const char *name_temp;
	char *m = NULL;
	int rc;
	inverse_document_frequency idf = {0, 0};

	if (!args->legacy) {
		/* Register the rank function */
		rc = sqlite3_create_function(db, "rank_func", 1, SQLITE_ANY,
		    (void *) &idf, rank_func, NULL, NULL);
		if (rc != SQLITE_OK) {
			warnx("Unable to register the ranking function: %s",
			    sqlite3_errmsg(db));
			sqlite3_close(db);
			sqlite3_shutdown();
			exit(EXIT_FAILURE);
		}
	}

	rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (rc == SQLITE_IOERR) {
		warnx("Corrupt database. Please rerun makemandb");
		return -1;
	} else if (rc != SQLITE_OK) {
		warnx("%s", sqlite3_errmsg(db));
		return -1;
	}

	unsigned int nresults = 0;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		nresults++;
		section = (const char *) sqlite3_column_text(stmt, 0);
		name_temp = (const char *) sqlite3_column_text(stmt, 1);
		name_desc = (const char *) sqlite3_column_text(stmt, 2);
		machine = (const char *) sqlite3_column_text(stmt, 3);
		if (!args->legacy)
			snippet = (const char *) sqlite3_column_text(stmt, 4);
		if ((slash_ptr = strrchr(name_temp, '/')) != NULL)
			name_temp = slash_ptr + 1;
		if (machine && machine[0]) {
			m = estrdup(machine);
			easprintf(&name, "%s/%s", lower(m), name_temp);
			free(m);
		} else {
			name = estrdup((const char *)
			    sqlite3_column_text(stmt, 1));
		}

		(args->callback)(args->callback_data, section, name,
		    name_desc, snippet, args->legacy? 0: strlen(snippet));
		free(name);
	}
	sqlite3_finalize(stmt);
	return nresults;
}


/*
 *  run_query_internal --
 *  Performs the searches for the keywords entered by the user.
 *  The 2nd param: snippet_args is an array of strings providing values for the
 *  last three parameters to the snippet function of sqlite. (Look at the docs).
 *  The 3rd param: args contains rest of the search parameters. Look at
 *  arpopos-utils.h for the description of individual fields.
 *
 */
static int
run_query_internal(sqlite3 *db, const char *snippet_args[3], query_args *args)
{
	char *query;
	query = generate_search_query(args, snippet_args);
	if (query == NULL) {
		*args->errmsg = estrdup("malloc failed");
		return -1;
	}

	execute_search_query(db, query, args);
	sqlite3_free(query);
	return *(args->errmsg) == NULL ? 0 : -1;
}

static char *
get_escaped_html_string(const char *src, size_t *slen)
{
	static const char trouble[] = "<>\"&\002\003";
	/*
	 * First scan the src to find out the number of occurrences
	 * of {'>', '<' '"', '&'}.  Then allocate a new buffer with
	 * sufficient space to be able to store the quoted versions
	 * of the special characters {&gt;, &lt;, &quot;, &amp;}.
	 * Copy over the characters from the original src into
	 * this buffer while replacing the special characters with
	 * their quoted versions.
	 */
	char *dst, *ddst;
	size_t count;
	const char *ssrc;

	for (count = 0, ssrc = src; *src; count++) {
		size_t sz = strcspn(src, trouble);
		src += sz + 1;
	}


#define append(a)				\
    do {					\
	memcpy(dst, (a), sizeof(a) - 1);	\
	dst += sizeof(a) - 1; 			\
    } while (/*CONSTCOND*/0)


	ddst = dst = emalloc(*slen + count * 5 + 1);
	for (src = ssrc; *src; src++) {
		switch (*src) {
		case '<':
			append("&lt;");
			break;
		case '>':
			append("&gt;");
			break;
		case '\"':
			append("&quot;");
			break;
		case '&':
			/*
			 * Don't perform the quoting if this & is part of
			 * an mdoc escape sequence, e.g. \&
			 */
			if (src != ssrc && src[-1] != '\\')
				append("&amp;");
			else
				append("&");
			break;
		case '\002':
			append("<b>");
			break;
		case '\003':
			append("</b>");
			break;
		default:
			*dst++ = *src;
			break;
		}
	}
	*dst = '\0';
	*slen = dst - ddst;
	return ddst;
}


/*
 * callback_html --
 *  Callback function for run_query_html. It builds the html output and then
 *  calls the actual user supplied callback function.
 */
static int
callback_html(void *data, const char *section, const char *name,
    const char *name_desc, const char *snippet, size_t snippet_length)
{
	struct orig_callback_data *orig_data = data;
	int (*callback)(void *, const char *, const char *, const char *,
	    const char *, size_t) = orig_data->callback;
	size_t length = snippet_length;
	size_t name_description_length = strlen(name_desc);
	char *qsnippet = get_escaped_html_string(snippet, &length);
	char *qname_description = get_escaped_html_string(name_desc,
	    &name_description_length);

	(*callback)(orig_data->data, section, name, qname_description,
	    qsnippet, length);
	free(qsnippet);
	free(qname_description);
	return 0;
}

/*
 * run_query_html --
 *  Utility function to output query result in HTML format.
 *  It internally calls run_query only, but it first passes the output to its
 *  own custom callback function, which preprocess the snippet for quoting
 *  inline HTML fragments.
 *  After that it delegates the call the actual user supplied callback function.
 */
static int
run_query_html(sqlite3 *db, query_args *args)
{
	struct orig_callback_data orig_data;
	orig_data.callback = args->callback;
	orig_data.data = args->callback_data;
	const char *snippet_args[] = {"\002", "\003", "..."};
	args->callback = &callback_html;
	args->callback_data = (void *) &orig_data;
	return run_query_internal(db, snippet_args, args);
}

/*
 * underline a string, pager style.
 */
static char *
ul_pager(int ul, const char *s)
{
	size_t len;
	char *dst, *d;

	if (!ul)
		return estrdup(s);

	// a -> _\ba
	len = strlen(s) * 3 + 1;

	d = dst = emalloc(len);
	while (*s) {
		*d++ = '_';
		*d++ = '\b';
		*d++ = *s++;
	}
	*d = '\0';
	return dst;
}

/*
 * callback_pager --
 *  A callback similar to callback_html. It overstrikes the matching text in
 *  the snippet so that it appears emboldened when viewed using a pager like
 *  more or less.
 */
static int
callback_pager(void *data, const char *section, const char *name,
	const char *name_desc, const char *snippet, size_t snippet_length)
{
	struct orig_callback_data *orig_data = data;
	char *psnippet;
	const char *temp = snippet;
	int count = 0;
	int i = 0, did;
	size_t sz = 0;
	size_t psnippet_length;

	/* Count the number of bytes of matching text. For each of these
	 * bytes we will use 2 extra bytes to overstrike it so that it
	 * appears bold when viewed using a pager.
	 */
	while (*temp) {
		sz = strcspn(temp, "\002\003");
		temp += sz;
		if (*temp == '\003') {
			count += 2 * (sz);
		}
		temp++;
	}

	psnippet_length = snippet_length + count;
	psnippet = emalloc(psnippet_length + 1);

	/* Copy the bytes from snippet to psnippet:
	 * 1. Copy the bytes before \002 as it is.
	 * 2. The bytes after \002 need to be overstriked till we
	 *    encounter \003.
	 * 3. To overstrike a byte 'A' we need to write 'A\bA'
	 */
	did = 0;
	while (*snippet) {
		sz = strcspn(snippet, "\002");
		memcpy(&psnippet[i], snippet, sz);
		snippet += sz;
		i += sz;

		/* Don't change this. Advancing the pointer without reading the byte
		 * is causing strange behavior.
		 */
		if (*snippet == '\002')
			snippet++;
		while (*snippet && *snippet != '\003') {
			did = 1;
			psnippet[i++] = *snippet;
			psnippet[i++] = '\b';
			psnippet[i++] = *snippet++;
		}
		if (*snippet)
			snippet++;
	}

	psnippet[i] = 0;
	char *ul_section = ul_pager(did, section);
	char *ul_name = ul_pager(did, name);
	char *ul_name_desc = ul_pager(did, name_desc);
	(orig_data->callback)(orig_data->data, ul_section, ul_name,
	    ul_name_desc, psnippet, psnippet_length);
	free(ul_section);
	free(ul_name);
	free(ul_name_desc);
	free(psnippet);
	return 0;
}

struct term_args {
	struct orig_callback_data *orig_data;
	const char *smul;
	const char *rmul;
};

/*
 * underline a string, pager style.
 */
static char *
ul_term(const char *s, const struct term_args *ta)
{
	char *dst;

	easprintf(&dst, "%s%s%s", ta->smul, s, ta->rmul);
	return dst;
}

/*
 * callback_term --
 *  A callback similar to callback_html. It overstrikes the matching text in
 *  the snippet so that it appears emboldened when viewed using a pager like
 *  more or less.
 */
static int
callback_term(void *data, const char *section, const char *name,
	const char *name_desc, const char *snippet, size_t snippet_length)
{
	struct term_args *ta = data;
	struct orig_callback_data *orig_data = ta->orig_data;

	char *ul_section = ul_term(section, ta);
	char *ul_name = ul_term(name, ta);
	char *ul_name_desc = ul_term(name_desc, ta);
	(orig_data->callback)(orig_data->data, ul_section, ul_name,
	    ul_name_desc, snippet, snippet_length);
	free(ul_section);
	free(ul_name);
	free(ul_name_desc);
	return 0;
}

/*
 * run_query_pager --
 *  Utility function similar to run_query_html. This function tries to
 *  pre-process the result assuming it will be piped to a pager.
 *  For this purpose it first calls its own callback function callback_pager
 *  which then delegates the call to the user supplied callback.
 */
static int
run_query_pager(sqlite3 *db, query_args *args)
{
	struct orig_callback_data orig_data;
	orig_data.callback = args->callback;
	orig_data.data = args->callback_data;
	const char *snippet_args[3] = { "\002", "\003", "..." };
	args->callback = &callback_pager;
	args->callback_data = (void *) &orig_data;
	return run_query_internal(db, snippet_args, args);
}

struct nv {
	char *s;
	size_t l;
};

static int
term_putc(int c, void *p)
{
	struct nv *nv = p;
	nv->s[nv->l++] = c;
	return 0;
}

static char *
term_fix_seq(TERMINAL *ti, const char *seq)
{
	char *res = estrdup(seq);
	struct nv nv;

	if (ti == NULL)
	    return res;

	nv.s = res;
	nv.l = 0;
	ti_puts(ti, seq, 1, term_putc, &nv);
	nv.s[nv.l] = '\0';

	return res;
}

static void
term_init(int fd, const char *sa[5])
{
	TERMINAL *ti;
	int error;
	const char *bold, *sgr0, *smso, *rmso, *smul, *rmul;

	if (ti_setupterm(&ti, NULL, fd, &error) == -1) {
		bold = sgr0 = NULL;
		smso = rmso = smul = rmul = "";
		ti = NULL;
	} else {
		bold = ti_getstr(ti, "bold");
		sgr0 = ti_getstr(ti, "sgr0");
		if (bold == NULL || sgr0 == NULL) {
			smso = ti_getstr(ti, "smso");

			if (smso == NULL ||
			    (rmso = ti_getstr(ti, "rmso")) == NULL)
				smso = rmso = "";
			bold = sgr0 = NULL;
		} else
			smso = rmso = "";

		smul = ti_getstr(ti, "smul");
		if (smul == NULL || (rmul = ti_getstr(ti, "rmul")) == NULL)
			smul = rmul = "";
	}

	sa[0] = term_fix_seq(ti, bold ? bold : smso);
	sa[1] = term_fix_seq(ti, sgr0 ? sgr0 : rmso);
	sa[2] = estrdup("...");
	sa[3] = term_fix_seq(ti, smul);
	sa[4] = term_fix_seq(ti, rmul);

	if (ti)
		del_curterm(ti);
}

/*
 * run_query_term --
 *  Utility function similar to run_query_html. This function tries to
 *  pre-process the result assuming it will be displayed on a terminal
 *  For this purpose it first calls its own callback function callback_pager
 *  which then delegates the call to the user supplied callback.
 */
static int
run_query_term(sqlite3 *db, query_args *args)
{
	struct orig_callback_data orig_data;
	struct term_args ta;
	orig_data.callback = args->callback;
	orig_data.data = args->callback_data;
	const char *snippet_args[5];

	term_init(STDOUT_FILENO, snippet_args);
	ta.smul = snippet_args[3];
	ta.rmul = snippet_args[4];
	ta.orig_data = (void *) &orig_data;

	args->callback = &callback_term;
	args->callback_data = &ta;
	return run_query_internal(db, snippet_args, args);
}

static int
run_query_none(sqlite3 *db, query_args *args)
{
	struct orig_callback_data orig_data;
	orig_data.callback = args->callback;
	orig_data.data = args->callback_data;
	const char *snippet_args[3] = { "", "", "..." };
	args->callback = &callback_pager;
	args->callback_data = (void *) &orig_data;
	return run_query_internal(db, snippet_args, args);
}

int
run_query(sqlite3 *db, query_format fmt, query_args *args)
{
	switch (fmt) {
	case APROPOS_NONE:
		return run_query_none(db, args);
	case APROPOS_HTML:
		return run_query_html(db, args);
	case APROPOS_TERM:
		return run_query_term(db, args);
	case APROPOS_PAGER:
		return run_query_pager(db, args);
	default:
		warnx("Unknown query format %d", (int)fmt);
		return -1;
	}
}
