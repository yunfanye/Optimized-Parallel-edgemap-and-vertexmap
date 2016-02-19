#ifndef __PARAGRAPH_H__
#define __PARAGRAPH_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "vertex_set.h"
#include "graph.h"

#include "mic.h"

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
	int numNode = u -> size;
	Vertex * vertices = u -> vertices;
	int capacity = numNode ;
	for (int i = 0; i < numNode; i++) {
		int diff = outgoing_end(g, vertices[i]) - outgoing_begin(g, vertices[i]);
		capacity += diff;
	}
	VertexSet* ret = newVertexSet(SPARSE, capacity, num_nodes(g));

	#pragma omp parallel for 
	for (int i = 0; i < numNode; i++) {
		const Vertex* start = outgoing_begin(g, vertices[i]);
		const Vertex* end = outgoing_end(g, vertices[i]);
		for (const Vertex* k = start; k != end; k++) {
			if (f.cond(*k) && f.update(vertices[i], *k)) {
				addVertex(ret, *k);
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
