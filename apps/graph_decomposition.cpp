#include "paraGraph.h"
#include "graph.h"
#include "graph_decomposition.h"
#define NA -1
#define NORETURN 0
#define YESRETURN 1
/**
	Given a graph, a deltamu per node, the max deltamu value, and the id
	of the node with the max deltamu, decompose the graph into clusters. 
        Returns for each vertex the cluster id that it belongs to inside decomp.
	NOTE: deltamus are given as integers, floating point differences
	are resolved by node id order

**/

class Decomosition
{
  public:
    int* cluster;
    int* nextcluster;
    // int* radii;
    // int iter;

    Decomosition(int* cluster, int* nextcluster) :
      cluster(cluster), nextcluster(nextcluster) {};

    Decomosition(Graph g, int* solution)
      : cluster(solution)
    {
      for (int i = 0; i < num_nodes(g); i++) {
      	cluster[i] = NA;
      }
    }

    bool update(Vertex src, Vertex dst) {
    	bool changed = false;
    	#pragma omp critical
    	{
    		if (nextcluster[dst] == NA || nextcluster[dst] > src) {
    			nextcluster[dst] = src;
    			changed = true;
    		}
    	}
    	return changed;
    }	

    bool cond(Vertex v) {
    	return cluster[v] == NA;
    }
};

class ClusterCopy 
{
  public:
    int* cluster;
    int* nextcluster;
    ClusterCopy(int* cluster, int* nextcluster) : 
      cluster(cluster), nextcluster(nextcluster) {};

    bool operator()(Vertex v) {
      if (cluster[v] == nextcluster[v])
      	return false;
      cluster[v] = nextcluster[v];
      return true;
    }
};

class UpdateFrontier
{
  public:
    int* dus;
    int iter;
    int max_dus;
    UpdateFrontier(int max_dus, int iter, int* dus) : 
      max_dus(max_dus), iter(iter), dus(dus) {};

    bool operator()(Vertex v) {
      	return (iter > max_dus - dus[v]);
    }
};


void decompose(graph *g, int *decomp, int* dus, int maxVal, int maxId) {
  int* cluster;
  int* nextcluster;
  int iter = 0;
  int total_num = g -> num_nodes;

  cluster = (int*) malloc(sizeof(int*) * total_num);
  nextcluster = (int*) malloc(sizeof(int*) * total_num);
  memset(cluster, NA, sizeof(int) * total_num);
  memset(nextcluster, NA, sizeof(int) * total_num);
  
  // int* dus = getDus(total_num, decomp, &maxVal, &maxId); // rate = 1/beta beta哪里来

  VertexSet* frontier = newVertexSet(SPARSE, 1, total_num);

  addVertex(frontier, maxId);

  VertexSet *newFrontier;

  int max_dus = 0;
  for (int i = 0; i < total_num; i++) {
  	max_dus = max_dus >= dus[i] ? max_dus : dus[i];
  }

  while (frontier->size > 0) {
    iter += 1;
    Decomosition dec(cluster, nextcluster);
    newFrontier = edgeMap(g, frontier, dec, NORETURN);

    freeVertexSet(frontier);
    frontier = newFrontier;
    // start growing all balls i at the next iter with 
//     // uncluster center i and with maxDu - dus[i] < iter 
//     foreach vertex i in not_cluster {
//       if (iter > du_max - dus[i]) {
//         frontier->add(i);
//       }
//     }
    UpdateFrontier uf(max_dus, iter, dus);
    vertexMap(frontier, uf, YESRETURN);

    ClusterCopy vc(cluster, nextcluster);
    vertexMap(frontier, vc, NORETURN);
  }

  freeVertexSet(frontier);
  free(cluster);
  free(nextcluster);
}
