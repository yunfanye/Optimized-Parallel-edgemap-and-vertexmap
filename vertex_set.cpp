#include "vertex_set.h"

#include <stdlib.h>
#include <string.h>
#include <cassert>
#include <stdio.h>
#include "mic.h"


/**
 * Creates an empty VertexSet with the given type and capacity.
 * numNodes is the total number of nodes in the graph.
 * 
 * Student may interpret type however they wish.  It may be helpful to
 * use different representations of VertexSets under different
 * conditions, and they different conditions can be indicated by 'type'
 */
VertexSet *newVertexSet(VertexSetType type, int capacity, int numNodes)
{
	VertexSet* set = (VertexSet*) malloc(sizeof(VertexSet));
	set -> size = 0;     // Number of nodes in the set
	set -> type = type; 
	set -> numNodes = numNodes;
	if(type == SPARSE) {		
		set -> vertices = (Vertex*) malloc(sizeof(Vertex) * capacity);
	}
	else {
		set -> map = (bool*) malloc(sizeof(bool) * numNodes);
		memset(set -> map, 0, sizeof(bool) * numNodes);
	}
  	return set;
}

void freeVertexSet(VertexSet *set)
{
	if(set -> type == SPARSE)
		free(set -> vertices);
	else
		free(set -> map);
	free(set);
}

bool hasVertex(VertexSet *set, Vertex v) {
	// thread safe, only for DENSE matrix
  	if(set -> type == SPARSE) {
  		assert(false);
	}
	else {
		// Vertex is typedef'ed as int
		#pragma vector nontemporal(set -> map)
		return set -> map[v];
	}
}

void addVertex(VertexSet *set, Vertex v)
{
	// thread-safe
	if(set -> type == SPARSE) {
		int size = __sync_fetch_and_add(&set -> size, 1);
		set -> vertices[size] = v;
	} 
	else {
		// Vertex is typedef'ed as int
		__sync_fetch_and_add(&set -> size, 1);
		#pragma vector nontemporal(set -> map)
		set -> map[v] = true;
	}
}

void addVertexBatch(VertexSet *set, Vertex v)
{
	// thread-safe
	if(set -> type == SPARSE) {
		int size = __sync_fetch_and_add(&set -> size, 1);
		set -> vertices[size] = v;
	} 
	else {
		// Vertex is typedef'ed as int
		#pragma vector nontemporal(set -> map)
		set -> map[v] = true;
	}
}

void removeVertex(VertexSet *set, Vertex v)
{
  	// non-thread safe, assume exactly one match
  	if(set -> type == SPARSE) {
		int size = set -> size;
		int index;
		Vertex* vertices = set -> vertices;
		#pragma omp parallel for 
		for(int i = 0; i < size; i++) {
			if(vertices[i] == v)
				index = i;
		}
		vertices[index] = vertices[size - 1];
		set -> size = size - 1;
	}
	else {
		// Vertex is typedef'ed as int
		__sync_fetch_and_sub(&set -> size, 1);
		set -> map[v] = false;
	}
}

void removeVertexAt(VertexSet *set, int index)
{
  	// thread safe, only for SPARSE matrix
  	if(set -> type == SPARSE) {
  		Vertex* vertices = set -> vertices;
  		int size = __sync_fetch_and_sub(&set -> size, 1);
		vertices[index] = vertices[size];
	}
	else {
		assert(false);
	}
}

VertexSet* ConvertSparseToDense(VertexSet* old) {
	int size = old -> size;
	int numNodes = old -> numNodes;
	VertexSet* new_set = newVertexSet(DENSE, size, numNodes);
	Vertex * vertices = old -> vertices;
	#pragma omp parallel for
	for(int i = 0; i < size; i++) {
		addVertexBatch(new_set, vertices[i]);
	}
	setSize(new_set, size);
	return new_set;
}

VertexSet* ConvertDenseToSparse(VertexSet* old) {
	int size = old -> size;
	int numNodes = old -> numNodes;
	VertexSet* new_set = newVertexSet(SPARSE, size, numNodes);
	#pragma omp parallel for 
	for (int i = 0; i < numNodes; ++i) {
		if(hasVertex(old, i))
			addVertex(new_set, i);
	}
	return new_set;
}

/**
 * Returns the union of sets u and v. Destroys u and v.
 */
VertexSet* vertexUnion(VertexSet *u, VertexSet* v)
{
  // TODO: Implement

  // STUDENTS WILL ONLY NEED TO IMPLEMENT THIS FUNCTION IN PART 3 OF
  // THE ASSIGNMENT

  return NULL;
}


