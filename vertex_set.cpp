#include "vertex_set.h"

#include <stdlib.h>
#include <string.h>
#include <cassert>
#include <stdio.h>
#include "mic.h"

#include <string.h>

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
	if(type == SPARSE) {		
		set -> vertices = (Vertex*) malloc(sizeof(Vertex) * capacity);
	}
	else {
		set -> vertices = (Vertex*) malloc(sizeof(Vertex) * numNodes);
		memset(set -> vertices, 0, sizeof(Vertex) * numNodes);
	}
  	return set;
}

void freeVertexSet(VertexSet *set)
{
	free(set -> vertices);
	free(set);
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
		set -> vertices[v] = 1;
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
		vertices[index] = vertices[size];
		set -> size = size - 1;
	}
	else {
		// Vertex is typedef'ed as int
		set -> vertices[v] = 0;
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
		// Vertex is typedef'ed as int
		assert(false);
	}
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

