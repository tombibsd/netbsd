/*	$NetBSD$	*/

/* mtest.c - memory-mapped database tester/toy */
/*
 * Copyright 2011 Howard Chu, Symas Corp.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in the file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */
#define _XOPEN_SOURCE 500		/* srandom(), random() */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "lmdb.h"

int main(int argc,char * argv[])
{
	int i = 0, j = 0, rc;
	MDB_env *env;
	MDB_dbi dbi;
	MDB_val key, data;
	MDB_txn *txn;
	MDB_stat mst;
	MDB_cursor *cursor, *cur2;
	int count;
	int *values;
	char sval[32] = "";

	srandom(time(NULL));

	    count = (random()%384) + 64;
	    values = (int *)malloc(count*sizeof(int));

	    for(i = 0;i<count;i++) {
			values[i] = random()%1024;
	    }
    
		rc = mdb_env_create(&env);
		rc = mdb_env_set_mapsize(env, 10485760);
		rc = mdb_env_open(env, "./testdb", MDB_FIXEDMAP /*|MDB_NOSYNC*/, 0664);
		rc = mdb_txn_begin(env, NULL, 0, &txn);
		rc = mdb_open(txn, NULL, 0, &dbi);
   
		key.mv_size = sizeof(int);
		key.mv_data = sval;
		data.mv_size = sizeof(sval);
		data.mv_data = sval;

		printf("Adding %d values\n", count);
	    for (i=0;i<count;i++) {	
			sprintf(sval, "%03x %d foo bar", values[i], values[i]);
			rc = mdb_put(txn, dbi, &key, &data, MDB_NOOVERWRITE);
			if (rc) {
				j++;
				data.mv_size = sizeof(sval);
				data.mv_data = sval;
			}
	    }
		if (j) printf("%d duplicates skipped\n", j);
		rc = mdb_txn_commit(txn);
		rc = mdb_env_stat(env, &mst);

		rc = mdb_txn_begin(env, NULL, 1, &txn);
		rc = mdb_cursor_open(txn, dbi, &cursor);
		while ((rc = mdb_cursor_get(cursor, &key, &data, MDB_NEXT)) == 0) {
			printf("key: %p %.*s, data: %p %.*s\n",
				key.mv_data,  (int) key.mv_size,  (char *) key.mv_data,
				data.mv_data, (int) data.mv_size, (char *) data.mv_data);
		}
		mdb_cursor_close(cursor);
		mdb_txn_abort(txn);

		j=0;
		key.mv_data = sval;
	    for (i= count - 1; i > -1; i-= (random()%5)) {	
			j++;
			txn=NULL;
			rc = mdb_txn_begin(env, NULL, 0, &txn);
			sprintf(sval, "%03x ", values[i]);
			rc = mdb_del(txn, dbi, &key, NULL);
			if (rc) {
				j--;
				mdb_txn_abort(txn);
			} else {
				rc = mdb_txn_commit(txn);
			}
	    }
	    free(values);
		printf("Deleted %d values\n", j);

		rc = mdb_env_stat(env, &mst);
		rc = mdb_txn_begin(env, NULL, 1, &txn);
		rc = mdb_cursor_open(txn, dbi, &cursor);
		printf("Cursor next\n");
		while ((rc = mdb_cursor_get(cursor, &key, &data, MDB_NEXT)) == 0) {
			printf("key: %.*s, data: %.*s\n",
				(int) key.mv_size,  (char *) key.mv_data,
				(int) data.mv_size, (char *) data.mv_data);
		}
		printf("Cursor last\n");
		rc = mdb_cursor_get(cursor, &key, &data, MDB_LAST);
		printf("key: %.*s, data: %.*s\n",
			(int) key.mv_size,  (char *) key.mv_data,
			(int) data.mv_size, (char *) data.mv_data);
		printf("Cursor prev\n");
		while ((rc = mdb_cursor_get(cursor, &key, &data, MDB_PREV)) == 0) {
			printf("key: %.*s, data: %.*s\n",
				(int) key.mv_size,  (char *) key.mv_data,
				(int) data.mv_size, (char *) data.mv_data);
		}
		printf("Cursor last/prev\n");
		rc = mdb_cursor_get(cursor, &key, &data, MDB_LAST);
			printf("key: %.*s, data: %.*s\n",
				(int) key.mv_size,  (char *) key.mv_data,
				(int) data.mv_size, (char *) data.mv_data);
		rc = mdb_cursor_get(cursor, &key, &data, MDB_PREV);
			printf("key: %.*s, data: %.*s\n",
				(int) key.mv_size,  (char *) key.mv_data,
				(int) data.mv_size, (char *) data.mv_data);

		mdb_txn_abort(txn);

		printf("Deleting with cursor\n");
		rc = mdb_txn_begin(env, NULL, 0, &txn);
		rc = mdb_cursor_open(txn, dbi, &cur2);
		for (i=0; i<50; i++) {
			rc = mdb_cursor_get(cur2, &key, &data, MDB_NEXT);
			if (rc)
				break;
			printf("key: %p %.*s, data: %p %.*s\n",
				key.mv_data,  (int) key.mv_size,  (char *) key.mv_data,
				data.mv_data, (int) data.mv_size, (char *) data.mv_data);
			rc = mdb_del(txn, dbi, &key, NULL);
		}

		printf("Restarting cursor in txn\n");
		rc = mdb_cursor_get(cur2, &key, &data, MDB_FIRST);
		printf("key: %p %.*s, data: %p %.*s\n",
			key.mv_data,  (int) key.mv_size,  (char *) key.mv_data,
			data.mv_data, (int) data.mv_size, (char *) data.mv_data);
		for (i=0; i<32; i++) {
			rc = mdb_cursor_get(cur2, &key, &data, MDB_NEXT);
			if (rc) break;
			printf("key: %p %.*s, data: %p %.*s\n",
				key.mv_data,  (int) key.mv_size,  (char *) key.mv_data,
				data.mv_data, (int) data.mv_size, (char *) data.mv_data);
		}
		mdb_cursor_close(cur2);
		rc = mdb_txn_commit(txn);

		printf("Restarting cursor outside txn\n");
		rc = mdb_txn_begin(env, NULL, 0, &txn);
		rc = mdb_cursor_open(txn, dbi, &cursor);
		rc = mdb_cursor_get(cursor, &key, &data, MDB_FIRST);
		printf("key: %p %.*s, data: %p %.*s\n",
			key.mv_data,  (int) key.mv_size,  (char *) key.mv_data,
			data.mv_data, (int) data.mv_size, (char *) data.mv_data);
		for (i=0; i<32; i++) {
			rc = mdb_cursor_get(cursor, &key, &data, MDB_NEXT);
			if (rc) break;
			printf("key: %p %.*s, data: %p %.*s\n",
				key.mv_data,  (int) key.mv_size,  (char *) key.mv_data,
				data.mv_data, (int) data.mv_size, (char *) data.mv_data);
		}
		mdb_cursor_close(cursor);
		mdb_close(env, dbi);

		mdb_txn_abort(txn);
		mdb_env_close(env);

	return 0;
}
