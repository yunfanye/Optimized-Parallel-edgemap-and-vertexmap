#ifndef __VERTEX_SET__
#define __VERTEX_SET__

#include "graph.h"

typedef enum {
  SPARSE,
  DENSE
} VertexSetType;

typedef struct {
  int size;     // Number of nodes in the set
  int capacity; // Maximum nodes can hold currently
  int numNodes; // Number of nodes in the graph
  VertexSetType type; 
  Vertex* vertices;
} VertexSet;

VertexSet *newVertexSet(VertexSetType type, int capacity, int numNodes);
void freeVertexSet(VertexSet *set);
bool hasVertex(VertexSet *set, Vertex v);
void addVertex(VertexSet *set, Vertex v);
void removeVertex(VertexSet *set, Vertex v);
void removeVertexAt(VertexSet *set, int index);
VertexSet* vertexUnion(VertexSet *u, VertexSet* v);

#endif // __VERTEX_SET__
