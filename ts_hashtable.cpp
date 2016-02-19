#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ts_hashtable.h"

ts_hashtable * new_hashtable(int capacity) {
	ts_hashtable * table = (ts_hashtable *)malloc(sizeof(ts_hashtable));
	table -> capacity = capacity;
	table -> nodes = (ts_hashnode_t*)malloc(sizeof(ts_hashnode_t) * capacity);
	memset(table -> nodes, 0, sizeof(ts_hashnode_t) * capacity);
	return table;
}

// return hastable_has
bool hashtable_set(ts_hashtable * table, hash_data_t data) {
	int index = data % table -> capacity;
	ts_hashnode_t* hash_table = table -> nodes;

	if(!hash_table[index].valid && 
		__sync_bool_compare_and_swap(&hash_table[index].valid, false, true)) {
		hash_table[index].data = data;
		hash_table[index].next = NULL;
		return false;
	}
	else {
		ts_hashnode_t* node = hash_table + index;
		ts_hashnode_t* new_node = (ts_hashnode_t*)malloc(sizeof(ts_hashnode_t));
		new_node -> next = NULL;
		new_node -> data = data;
		while(true) {
			if(node -> data == data) {
				free(new_node);
				return true;
			}
			if(node -> next == NULL && 
				__sync_bool_compare_and_swap(&node -> next, NULL, new_node)) {
				return false;
			}
			node = node -> next;
		}		
	}
}

bool hashtable_has(ts_hashtable * table, hash_data_t data) {
	int index = data % table -> capacity;
	ts_hashnode_t* hash_table = table -> nodes;
	if(!hash_table[index].valid) {
		return false;
	}
	else {
		ts_hashnode_t* node = hash_table + index;
		while(node != NULL) {
			if(node -> data == data) {
				return true;
			}
			node = node -> next;		
		}
		return false;
	}
}

void hashtable_free(ts_hashtable * table) {
	free(table -> nodes);
	free(table);
}