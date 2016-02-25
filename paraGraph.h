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
#include "util.h"

#include <time.h>
#include <immintrin.h>

#define CHUNK_SIZE 32
#define MAX_THREAD_NUM 256

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
static VertexSet *edgeMap(Graph g, VertexSet *u, F &f,
    bool removeDuplicates=true)
{
  // outputSubset = {}

  // foreach u in U: (in parallel)
  //    for each outgoing edge (u,v) from u: (in parallel)
  //        if (C(v) && F(u,v))
  //            outputSubset.append(v)

  // remove_duplicates(outputSubset)
  // return outputSubset
	static int edge_counts[MAX_THREAD_NUM];
	static int edge_sizes[MAX_THREAD_NUM];
	int size = u -> size;
	int total_num = num_nodes(g);	
	VertexSet* ret;
	bool need_free = false;	
	if(size < total_num / 100) {		
		// ensure u is SPARSE
		if(u -> type != SPARSE) {
			u = ConvertDenseToSparse(u);
			need_free = true;
		}
		ts_hashtable * hash_table;
		int max_threads = omp_get_max_threads();	
		Vertex * vertices = u -> vertices;
		#pragma omp parallel
		{
			int numThreads = omp_get_num_threads();
			int blockSize = (size + numThreads - 1)/ numThreads;
			int tid = omp_get_thread_num();
			int start = blockSize * tid;
			int end = start + blockSize;
			end = end > size ? size : end;
			int localCount = 0;
			for (int i = start; i < end; i++) {
				int diff = outgoing_size(g, vertices[i]);
				localCount += diff;
			}
			#pragma vector nontemporal(edge_counts)
			edge_counts[tid] = localCount;
		}

		int numNextPow2 = nextPow2(max_threads + 1);
		exclusive_scan(edge_counts, numNextPow2);	
		int capacity = edge_counts[max_threads];
		int * edges = (int *)malloc(sizeof(int) * (capacity + 1));
		// top down approach
		if(removeDuplicates)
			hash_table = new_hashtable(capacity | 1); //odd number capacity
		#pragma omp parallel
		{
			int numThreads = omp_get_num_threads();
			int blockSize = (size + numThreads - 1)/ numThreads;
			int tid = omp_get_thread_num();
			int start = blockSize * tid;
			int end = start + blockSize;
			end = end > size ? size : end;
			int localSize = 0;
			int localOffset;
			#pragma vector nontemporal(edge_counts)
			localOffset = edge_counts[tid];
			for (int i = start; i < end; i++) {
				const Vertex v_i = vertices[i];
				const Vertex* start = outgoing_begin(g, v_i);
				const Vertex* end = outgoing_end(g, v_i);
				for (const Vertex* k = start; k != end; k++) {
					if (f.cond(*k) && f.update(v_i, *k) && 
						(!removeDuplicates || !hashtable_set(hash_table, *k))) {
						edges[localOffset + localSize] = *k;
						localSize++;
					}
				}
			}
			#pragma vector nontemporal(edge_sizes)
			edge_sizes[tid] = localSize;
		}
		exclusive_scan(edge_sizes, numNextPow2);
		capacity = edge_sizes[max_threads];
		ret = newVertexSet(SPARSE, capacity, total_num);
		#pragma omp parallel for schedule(static)
		for(int i = 0; i < max_threads; i++) {
			int edge_sizestart = edge_sizes[i];
			int edgeStart = edge_counts[i];
			int length = edge_sizes[i + 1] - edge_sizes[i];
			if(length > 0)
				memcpy(&ret -> vertices[edge_sizestart], &edges[edgeStart], sizeof(int) * length);
		}
		setSize(ret, capacity);
		free(edges);
		if(removeDuplicates)
			hashtable_free(hash_table);
	}
	else {
		// ensure u is DENSE
		if(u -> type != DENSE) {
			u = ConvertSparseToDense(u);
			need_free = true;
		}
		// buttom up approach
		ret = newVertexSet(DENSE, size, total_num);
		// Vertex is typedef'ed as int 
		int total_size = 0;
		#pragma omp parallel for schedule(dynamic, 32) reduction(+:total_size)
		for(Vertex chunk = 0; chunk < total_num; chunk+=CHUNK_SIZE) {
			int mapValue = 0;
			for(int i = chunk; i < (chunk + CHUNK_SIZE) && i < total_num; i++) {
				bool hasAdded = false;
				const Vertex* k = incoming_begin(g, i);
				const Vertex* end = incoming_end(g, i);				
				while(f.cond(i) && k != end) {
					if ((u -> size == u -> numNodes || DenseHasVertex(u, *k)) 
						&& f.update(*k, i) && !hasAdded) {
						hasAdded = true;
						mapValue |= 1 << (i - chunk);
						total_size += 1;
					}
					k++;
				}
			}
			DenseSetMapValue(ret, chunk / CHUNK_SIZE, mapValue);
		}
		setSize(ret, total_size);
	}
	if(need_free)
		freeVertexSet(u);

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
static VertexSet *vertexMap(VertexSet *u, F &f, bool returnSet=true)
{
	// 1. apply F to all vertices in U
	// 2. return a new vertex subset containing all vertices u in U
	//      for which F(u) == true
	int size = u -> size;
	int numNodes = u -> numNodes;
	if(u -> type == SPARSE) {
		Vertex * vertices = u -> vertices;
		if (returnSet) {
			VertexSet* ret = newVertexSet(SPARSE, size, numNodes);		
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
	else {
		if (returnSet) {
			int total_size = 0;
			VertexSet* ret = newVertexSet(DENSE, size, numNodes);		
			#pragma omp parallel for schedule(static) reduction(+:total_size)
			for(int chunk = 0; chunk < numNodes; chunk+=CHUNK_SIZE) {
				int mapValue = 0;
				for(int i = chunk; i < (chunk + CHUNK_SIZE) && i < numNodes; i++) {
					if (DenseHasVertex(u, i) && f(i)) {
						mapValue |= 1 << (i - chunk);
						total_size += 1;
					}
				}
				DenseSetMapValue(ret, chunk / CHUNK_SIZE, mapValue);
			}
			setSize(ret, total_size);		
			return ret;
		}
		else {
			#pragma omp parallel for schedule(static)
			for(int chunk = 0; chunk < numNodes; chunk+=CHUNK_SIZE) {
				int base = chunk / CHUNK_SIZE;
				int map = DenseGetMapValue(u, base);
				for(int i = chunk; i < (chunk + CHUNK_SIZE) && i < numNodes; i++) {
					if((map & (1 << (i-chunk))))
						f(i);
				}
			}
			return NULL;
		}
	}
}

#endif /* __PARAGRAPH_H__ */
