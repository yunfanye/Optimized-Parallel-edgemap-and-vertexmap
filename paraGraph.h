#ifndef __PARAGRAPH_H__
#define __PARAGRAPH_H__

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


	// const Vertex* start = outgoing_begin(g, i);
 //  const Vertex* end = outgoing_end(g, i);
 //  for (const Vertex* v=start; v!=end; v++)
 //    printf("Edge %u %u\n", i, *v);
 /*	int numNode = u.numNode;
	int start, end;
	VertexSet* ret = newVertexSet(SPARSE, numNode, numNode);

	for (int i = 0; i < numNode; i++) {
		Vertex* start = outgoing()
	}
*/
  return NULL;
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
//   2. return a new vertex subset containing all vertices u in U
//      for which F(u) == true
	if (returnSet) {
		int numNode = u -> size;
		VertexSet* ret = newVertexSet(SPARSE, numNode, numNode);
		Vertex * vertices = u -> vertices;
		#pragma omp parallel for 
		for (int i = 0; i < numNode; i++) {
			if (f(vertices[i])) {
				#pragma omp critical
				addVertex(ret, vertices[i]);
			}
		}
		return ret;
	}
	else 
		return NULL;
}

#endif /* __PARAGRAPH_H__ */
