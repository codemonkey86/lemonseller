/*
 * Building this:
 * g++ -o permanent permanent_tsp.cpp -lemon
 */

#include <iostream>
#include <lemon/list_graph.h>
#include <lemon/matching.h>
#include <lemon/core.h>
#include <lemon/kruskal.h>
#include <lemon/euler.h>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>
#include <string>
#include <cstdlib>
#include <ctime>
#include <set>
//#include <ifstream>
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
typedef Graph::EdgeMap<bool> BoolMap;
typedef Graph::NodeMap<Node> NodesMap;
//typedef MaxMatching<Graph> MaxMatching;

//Function prototypes
void printEdgeMap(Graph*, LengthMap*);
int loadGraph(Graph*, int, vector<Node>*, map<pair<int, int>, Edge>*, LengthMap*);
void printNodes(Graph*, vector<Node>);
bool nodeCompare(Node, Node);
int computePathCost(vector<int>, LengthMap*, map<pair<int, int>, Edge>);
void printPath(vector<int>);

set<pair<int, int> > removed;
Graph tempGraph;
LengthMap tempDistances(tempGraph);


int main(int argc, char** argv){
  //the first argument will be the size of the input graph.
	
  //Validate the agruments
  if( argc < 2 || argc > 3 )
    {
      cout << "Incorrect Usage:" << endl;
      cout << "\tpermanent size [- regular-ness (only 1 or 3 supported, 1 default)]" << endl;
      return -1;
    }
	int size = atoi(argv[1]);
	int regular = 1;
	if( argc == 3)
	{
		if(atoi(argv[2]) == 3)
		{
			regular = 3;
			//construct path to remove
			vector<int> vertices;
			for(int i = 0; i < size; ++i)
			{
				vertices.push_back(i);
			}
			srand(time(0));
			random_shuffle(vertices.begin(), vertices.end());
			for(int i = 0; i < (vertices.size()-1); ++i)
			{
				removed.insert(make_pair(vertices[i],vertices[i+1]));
			}
			removed.insert(make_pair(vertices[vertices.size()-1],vertices[0]));
			/*for( int i = 0; i < vertices.size(); ++i)
			{
				cout << vertices[i] << " ";
			}
			cout << endl;
			for(set<pair<int, int> >::iterator it = removed.begin(); it != removed.end(); ++it)
			{
				cout << "(" << it->first << "," << it->second << "), ";
			}
			cout << endl;*/
		}
	}

  //Load the graph  
  Graph g;

  vector<Node> nodes;
  map<pair<int, int>, Edge> edges;

  /*Node u = g.addNode();
  Node v = g.addNode();
  Node w = g.addNode();
  Edge a = g.addEdge(u,v);
  Edge b = g.addEdge(v,w);
  Edge c = g.addEdge(w,u);*/

/*nodes.push_back(u);
nodes.push_back(v);
nodes.push_back(w);

cout << g.id(nodes[0]) << endl;*/


  LengthMap distances(g);


  //cout << "Load graph" << endl;
  //Get all of the nodes of the graph and put it in a list
  int result = loadGraph(&g, size, &nodes, &edges, &distances);

    //printNodes(&g, nodes);

  if(result != 0)
    {
      cout << "Problem opening input file." << endl;
      return result;
    }

	//Create a matching of our graph
	MaxMatching<Graph> matching(tempGraph);
	matching.init();
	matching.run();
	
	
	//Contract the edges in the matching
	//in a duplicated graph
	//for each edge e in graph
	
	//printEdgeMap(&tempGraph, &tempDistances);
	for(EdgeIt e(tempGraph); e!=INVALID; ++e)
	{
		if(matching.matching(e))
			tempGraph.contract(tempGraph.u(e), tempGraph.v(e));
	}

	//printEdgeMap(&tempGraph, &tempDistances);
	
	//All of the below is from failed attempts at using graphCopy
	/*Graph temp;
	GraphCopy<Graph, Graph> cg(g, temp);
	Graph::EdgeMap<Edge> er(g);
	Graph::NodeMap<Node> nr(g);
	cg.edgeRef(er);
	cg.nodeRef(nr);
	cg.run();
	
	//graphCopy(g, temp);
	LengthMap testDist(temp);*/
	
	//printEdgeMap(&g, &distances);
	//printEdgeMap(&tempGraph, &tempDistances);
	//graphCopy(src, trg).nodeRef(nr).edgeCrossRef(ecr).run();
	
	/*GraphCopy<Graph, Graph> cg(g, temp);
	
	
	NodesMap nr(g);
	cg.nodeRef();
	LengthMap ecr(temp);
	cg.edgeCrossRef(ecr);*/
	//End failed attempts at graphCopy
	
	
	//Compute minimum spanning tree of contracted graph
	BoolMap tempBool(tempGraph);
	vector<Edge> tree;
	kruskal(tempGraph, tempDistances, back_inserter(tree));
	/*for(int i = 0; i < tree.size(); ++i)
	{
	    cout << tempGraph.id(tempGraph.u(tree[i])) << "<->" << tempGraph.id(tempGraph.v(tree[i])) << endl;
	}*/
	
	
	//Combine two copies of the MST with the matching
	//First, remove all the edges that aren't in the matching
	for(EdgeIt e(g); e!=INVALID; ++e)
	{
		if(!matching.matching(e))
			g.erase(e);
	}
	//Next, add all the edges that are in the MST, twice
	for(int i = 0; i < tree.size(); ++i)
	{
		Node u = g.nodeFromId(tempGraph.id(tempGraph.u(tree[i])));
		Node v = g.nodeFromId(tempGraph.id(tempGraph.v(tree[i])));
		Edge newEdge = g.addEdge(u,v);
		Edge newEdge2 = g.addEdge(v,u);
		distances.set(newEdge, 1);
		distances.set(newEdge2, 1);
		edges.insert(make_pair(make_pair(tempGraph.id(tempGraph.u(tree[i])),tempGraph.id(tempGraph.v(tree[i]))), newEdge));
	}
	//printEdgeMap(&g, &distances);
	
	//First, create a graph with all of the nodes of the original
	/*Graph final;
	for(NodeIt n(g); n != INVALID; ++n)
	{
		
	}
	for(int i = 0; i < tree.size(); ++i)
	{
		Edge newEdge = tempGraph.addEdge(tempGraph.u(tree[i]), tempGraph.v(tree[i]));
		Edge newEdge2 = tempGraph.addEdge(tempGraph.v(tree[i]), tempGraph.u(tree[i]));
		tempDistances.set(newEdge, 1);
		tempDistances.set(newEdge2, 1);
	}
	printEdgeMap(&tempGraph, &tempDistances);*/
	
	
	//Find Eulerian Tour
	//This is our TSP tour
	vector<int> minimumPath;
	set<int> traversed;
	for(EulerIt<ListGraph> e(g); e!=INVALID; ++e) 
	{
		//if(traversed.find(g.id(g.u(Edge(e)))) == traversed.end())
		{
			//Haven't visited this node yet
			minimumPath.push_back(g.id(g.u(Edge(e))));
			traversed.insert(g.id(g.u(Edge(e))));
		}
		//else if(traversed.find(g.id(g.v(Edge(e)))) == traversed.end())
		{
			//already visited the previous node, haven't visited it's adjacent node
			minimumPath.push_back(g.id(g.v(Edge(e))));
			traversed.insert(g.id(g.v(Edge(e))));
		}
		//else
			//This should only occur for the last edge.
			continue;
	}
	int minimumCost = computePathCost(minimumPath, &distances, edges);

	//Leftovers from naive, in case we need them
	/*vector<int> nodeCounter;
	
	for(int i = 0; i < nodes.size(); ++i)
	{
		nodeCounter.push_back(i);
	}*/

  //Sort the nodes so we're at the first permutation
	//sort(nodeCounter.begin(), nodeCounter.end());


  //iterate through all of the permutations
  //and compute the length of each tour
  //Save the minimum tour length and the minimum tour
	/*vector<int> minimumPath = vector<int>(nodeCounter);
	int minimumCost = computePathCost(nodeCounter, &distances, edges);
	//cout << "Initial minimum cost is " << minimumCost << endl;
	
	while(next_permutation(nodeCounter.begin(), nodeCounter.end()))
	{
		int newMin = computePathCost(nodeCounter, &distances, edges);
		if( newMin < minimumCost)
		{
			minimumCost = newMin;
			minimumPath = vector<int>(nodeCounter);
		}
	}*/
	
	//Compute the bound/check if our path is within it
	//n is size of graph and k is k-regular-ness of it
	int n = nodes.size();
	int k = n - regular;
	float bound = (1 + sqrt(64/log(k))) * n;
	string bounded = "no"; 
	if( minimumCost <= bound )
	{
		bounded = "yes";
	}


  cout << "The minimum tour length of this graph is " << minimumCost << endl;
  cout << "The minimum tour of this graph is ";
  printPath(minimumPath);
  cout << endl;
  cout << "Tour within bounds: " << bounded << endl;
  
  /*cout << "Hello World! This is the LEMON library here." << endl;
  cout << "We have a directed graph with " << countNodes(g) << " nodes "
       << "and " << countEdges(g) << " arc." << endl;*/
  
  /*distances[a] = 5;
  distances[b] = 3;
  distances[c] = 1;*/
  
  //printEdgeMap(&g, &distances);

	//for att_48, should return 4727
	//cout << "0<->1=" << distances[edges[make_pair(0,1)]] << endl;

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

void printNodes(Graph *g, vector<Node> nodes)
{
    cout << "Printing " << nodes.size() << " Nodes!" << endl;
	for( int i =0; i < nodes.size(); ++i)
    {
        cout << g->id(nodes[i]) << endl;
    }
}

int loadGraph(Graph *g, int size, vector<Node> *nodes, map<pair<int, int>, Edge> *edges, LengthMap *distances)
{
  //Change this vector to be an input parameter by reference
  //cout << "Initializing vector" << endl;
  //vector <Node> nodes;
  
  //cout << "Opening file" << endl;

  //ifstream inFile;
  //inFile.open(input);

  //cout << "Opened file" << endl;
  /*if(!inFile.is_open())
    {
      return -1;
    }*/
  //cout << "Checked if open" << endl;

  //Get size of graph
  //int size = 0;
  float weight = 0;

  //cout << "Assigned initial values" << endl;

  //inFile >> size;
  //cout << "Size is: " << size << endl;

  //Initialize graph
  for(int i = 0; i < size; ++i)
    {
      nodes->push_back(g->addNode());
	  tempGraph.addNode();
    }

//LengthMap distances(*g);

  //Load in graph edges
  for(int i = 0; i < size; ++i)
    {
      for( int j = i+1; j < size; ++j)
    {
      //inFile >> weight;
		Edge newEdge = g->addEdge(nodes->at(i),nodes->at(j));
		Edge tempNewEdge = tempGraph.addEdge(nodes->at(i),nodes->at(j));
		edges->insert(make_pair(make_pair(i,j), newEdge));
		if(removed.find(make_pair(i,j)) != removed.end() || removed.find(make_pair(j,i)) != removed.end())
		{
			distances->set(newEdge, 2);
			tempDistances.set(tempNewEdge, 2);
		}
		else
		{
			distances->set(newEdge,1);
			tempDistances.set(tempNewEdge, 1);
		}
		//distances->set(newEdge, weight);
		//distances->set(newEdge, weight);
      //cout << weight << " ";
    }
      //cout << endl;
    }

  //Close our input file
  //inFile.close();

  return 0;
}

bool nodeCompare(Node i, Node j)
{
	return (i<j);
}
    
int computePathCost(vector<int> path, LengthMap *distances, map<pair<int, int>, Edge > edges)
{
	int sum = 0;
	//cout << "Computing cost of path size " << path.size() << endl;
	for(int i = 0; i < (path.size()-1); ++i)
	{
		if(path[i] < path[i+1])
		{
			sum += (*distances)[edges[make_pair(path[i],path[i+1])]];
		}
		else
		{
			sum += (*distances)[edges[make_pair(path[i+1],path[i])]];
		}
	}
	//Compute edge to return to source
	if(path[path.size()-1] < path[0])
	{
		sum += (*distances)[edges[make_pair(path[path.size()-1],path[0])]];
	}
	else
	{
		sum += (*distances)[edges[make_pair(path[0],path[path.size()-1])]];
	}
	
	return sum;
}

void printPath(vector<int> path)
{
	for(int i = 0; i < path.size(); ++i)
	{
		cout << path[i] << " ";
	}
}