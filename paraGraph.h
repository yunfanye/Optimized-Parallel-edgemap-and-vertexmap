#ifndef __PARAGRAPH_H__
#define __PARAGRAPH_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "vertex_set.h"
#include "graph.h"

#include "mic.h"
#include "ts_hashtable.h"
/*
 * edgeMap --
 * 
 * Students will implement this function.
 * 
 * The input argument f is a class with the following methods defined:
 *   bool update(Vertex src, Vertex dst)
 *   bool cond(Vertex v)
 *
 * See apps/bfs.cpp for an example of such a class definition.
 * 
 * When the argument removeDuplicates is false, the implementation of
 * edgeMap need not remove duplicate vertices from the VertexSet it
 * creates when iterating over edges.  This is a performance
 * optimization when the application knows (and can tell ParaGraph)
 * that f.update() guarantees that duplicate vertices cannot appear in
 * the output vertex set.
 * 
 * Further notes: the implementation of edgeMap is templated on the
 * type of this object, which allows for higher performance code
 * generation as these methods will be inlined.
 */
template <class F>
VertexSet *edgeMap(Graph g, VertexSet *u, F &f, bool removeDuplicates=true)
{
  // outputSubset = {}

  // foreach u in U: (in parallel)
  //    for each outgoing edge (u,v) from u: (in parallel)
  //        if (C(v) && F(u,v))
  //            outputSubset.append(v)

  // remove_duplicates(outputSubset)
  // return outputSubset
	int size = u -> size;
	int total_num = num_nodes(g);
	Vertex * vertices = u -> vertices;
	int capacity = 1000;
	for (int i = 0; i < size; i++) {
		int diff = outgoing_size(g, vertices[i]);
		capacity += diff;
	}
	
	VertexSet* ret;
	if(u -> type == SPARSE) {
		// top down approach
		ret = newVertexSet(SPARSE, capacity, total_num);
		#pragma omp parallel for 
		for (int i = 0; i < size; i++) {
			const Vertex* start = outgoing_begin(g, vertices[i]);
			const Vertex* end = outgoing_end(g, vertices[i]);
			for (const Vertex* k = start; k != end; k++) {
				if (f.cond(*k) && f.update(vertices[i], *k)) {
					addVertex(ret, *k);
				}
			}
		}
		// remove duplicates, optimized for no dup case
		int count = ret -> size;
		Vertex * ret_vertices = ret -> vertices;
		ts_hashtable * hash_table = new_hashtable(count | 1); //odd number capacity
		bool has_conflicts = false;
		#pragma omp parallel for
		for (int i = 0; i < count; i++) {
			if(hashtable_set(hash_table, ret_vertices[i])) {
				has_conflicts = true;
			}
		}
		if(has_conflicts) {
			hashtable_reset(hash_table);
			VertexSet* no_dup_set = newVertexSet(SPARSE, capacity, total_num);
			#pragma omp parallel for
			for (int i = 0; i < count; i++) {
				if(!hashtable_set(hash_table, ret_vertices[i])) {
					addVertex(no_dup_set, ret_vertices[i]);
				}
			}
			freeVertexSet(ret);
			ret = no_dup_set;
		}
		hashtable_free(hash_table);
	}
	else {
		// buttom up approach
		ret = newVertexSet(DENSE, capacity, total_num);
		// Vertex is typedef'ed as int
		#pragma omp parallel for 
		for(Vertex i = 0; i < total_num; i++) {
			const Vertex* start = outgoing_begin(g, i);
			const Vertex* end = outgoing_end(g, i);
			for (const Vertex* k = start; k != end; k++) {
				if (hasVertex(u, i) && f.cond(i) && f.update(*k, i)) {
					addVertex(ret, i);
					break;
				}
			}
		}
	}
	return ret;
}



/*
 * vertexMap -- 
 * 
 * Students will implement this function.
 *
 * The input argument f is a class with the following methods defined:
 *   bool operator()(Vertex v)
 *
 * See apps/kBFS.cpp for an example implementation.
 * 
 * Note that you'll call the function on a vertex as follows:
 *    Vertex v;
 *    bool result = f(v)
 *
 * If returnSet is false, then the implementation of vertexMap should
 * return NULL (it need not build and create a vertex set)
 */
template <class F>
VertexSet *vertexMap(VertexSet *u, F &f, bool returnSet=true)
{
	// 1. apply F to all vertices in U
	// 2. return a new vertex subset containing all vertices u in U
	//      for which F(u) == true
	int size = u -> size;
	Vertex * vertices = u -> vertices;
	if (returnSet) {
		VertexSet* ret = newVertexSet(SPARSE, size, u -> numNodes);		
		#pragma omp parallel for 
		for (int i = 0; i < size; i++) {
			if (f(vertices[i])) {
				addVertex(ret, vertices[i]);
			}
		}
		return ret;
	}
	else {
		#pragma omp parallel for 
		for (int i = 0; i < size; i++) {
			f(vertices[i]);
		}
		return NULL;
	}
}

#endif /* __PARAGRAPH_H__ */
