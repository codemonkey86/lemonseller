#include <iostream>
#include <lemon/list_graph.h> 
//#include <lemon/concepts/graph.h>

using namespace lemon;
using namespace std;

//All of our convenient typedefs
typedef ListGraph Graph;
typedef Graph::EdgeIt EdgeIt;
typedef Graph::Edge Edge;
typedef Graph::NodeIt NodeIt;
typedef Graph::Node Node;
typedef Graph::EdgeMap<int> LengthMap;

//Function prototypes
void printEdgeMap(Graph*, LengthMap*);


int main(int argc, char** argv){
  //the first argument will be the data file to read in.
  
  Graph g;

  Node u = g.addNode();
  Node v = g.addNode();
  Node w = g.addNode();
  Edge a = g.addEdge(u,v);
  Edge b = g.addEdge(v,w);
  Edge c = g.addEdge(w,u);

  LengthMap distances(g);

  cout << "Hello World! This is the LEMON library here." << endl;
  cout << "We have a directed graph with " << countNodes(g) << " nodes "
       << "and " << countEdges(g) << " arc." << endl;
  
  distances[a] = 5;
  distances[b] = 3;
  distances[c] = 1;
  
  printEdgeMap(&g, &distances);

  return 0;
}

void printEdgeMap(Graph *g, LengthMap *edges)
{
  cout << "Printing edges!" << endl;
  for(EdgeIt i(*g); i!=INVALID; ++i)
  {
    cout << g->id(g->u(i)) << "<->" << g->id(g->v(i))
	 << "=" << (*edges)[i] << endl;
  }
  return;
}
