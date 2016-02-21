#ifndef __VERTEX_SET__
#define __VERTEX_SET__

#include "graph.h"

typedef enum {
  SPARSE,
  DENSE
} VertexSetType;

typedef struct {
  int size;     // Number of nodes in the set
  int numNodes; // Number of nodes in the graph
  VertexSetType type; 
  Vertex* vertices;
  bool* map;
} VertexSet;

VertexSet *newVertexSet(VertexSetType type, int capacity, int numNodes);
void freeVertexSet(VertexSet *set);
bool hasVertex(VertexSet *set, Vertex v);
void addVertex(VertexSet *set, Vertex v);
void addVertexBatch(VertexSet *set, Vertex v);
void removeVertex(VertexSet *set, Vertex v);
void removeVertexAt(VertexSet *set, int index);

inline void setSize(VertexSet *set, int size) {
	set -> size = size;
}

VertexSet* vertexUnion(VertexSet *u, VertexSet* v);

VertexSet* ConvertSparseToDense(VertexSet* old);
VertexSet* ConvertDenseToSparse(VertexSet* old);

#endif // __VERTEX_SET__
