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
  	set -> vertices = (Vertex*) malloc(sizeof(Vertex) * capacity);
  	return set;
}

void freeVertexSet(VertexSet *set)
{
	free(set -> vertices);
	free(set);
}

void addVertex(VertexSet *set, Vertex v)
{
	// non thread-safe
	int size = set -> size;
	set -> vertices[size] = v;
	set -> size = size + 1;
}

void removeVertex(VertexSet *set, Vertex v)
{
  	// Assume exactly one match
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

