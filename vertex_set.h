#ifndef __VERTEX_SET__
#define __VERTEX_SET__

#include "graph.h"

#define CHUNK_SIZE 32

typedef enum {
  SPARSE,
  DENSE
} VertexSetType;

typedef struct {
  int size;     // Number of nodes in the set
  int numNodes; // Number of nodes in the graph
  VertexSetType type; 
  Vertex* vertices;
  int* map;
} VertexSet;

VertexSet *newVertexSet(VertexSetType type, int capacity, int numNodes);
void freeVertexSet(VertexSet *set);
void addVertex(VertexSet *set, Vertex v);
void removeVertex(VertexSet *set, Vertex v);
void removeVertexAt(VertexSet *set, int index);

inline bool DenseHasVertex(VertexSet *set, Vertex v) {
	int base = v / CHUNK_SIZE;
	int offset = v % CHUNK_SIZE;
	return (set -> map[base] & (1 << offset));
}

inline void DenseSetMapValue(VertexSet* set, Vertex v, int value) {
	set -> map[v] = value;
}

inline int DenseGetMapValue(VertexSet* set, Vertex v) {
	return set -> map[v];
}

inline void setSize(VertexSet *set, int size) {
	set -> size = size;
}

VertexSet* vertexUnion(VertexSet *u, VertexSet* v);

VertexSet* ConvertSparseToDense(VertexSet* old);
VertexSet* ConvertDenseToSparse(VertexSet* old);

#endif // __VERTEX_SET__
