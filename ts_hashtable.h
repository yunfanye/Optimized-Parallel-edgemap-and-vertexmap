#ifndef __TS_HASHTABLE_H__
#define __TS_HASHTABLE_H__

#include "graph.h"

typedef Vertex hash_data_t;

typedef struct ts_hashnode_t {
	hash_data_t data;
	bool valid;
	struct ts_hashnode_t * next;
} ts_hashnode;

// thread safe hash table implemented by atomic builtins
typedef struct ts_hashtable_t {
	int capacity;
	ts_hashnode * nodes;
} ts_hashtable;

ts_hashtable * new_hashtable(int capacity);

bool hashtable_set(ts_hashtable * table, hash_data_t data);

bool hashtable_has(ts_hashtable * table, hash_data_t data);

void hashtable_free(ts_hashtable * table);

#endif