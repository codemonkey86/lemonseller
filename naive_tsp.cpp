/*
 * Building this:
 * g++ -o naive naive_tsp.cpp -lemon
 */

#include <iostream>
#include <lemon/list_graph.h> 
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <math>
#include <string>
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

//Function prototypes
void printEdgeMap(Graph*, LengthMap*);
int loadGraph(Graph*, char*, vector<Node>*, map<pair<int, int>, Edge>*, LengthMap*);
void printNodes(Graph*, vector<Node>);
bool nodeCompare(Node, Node);
int computePathCost(vector<int>, LengthMap*, map<pair<int, int>, Edge>);
void printPath(vector<int>);

int main(int argc, char** argv){
  //the first argument will be the data file to read in.


  //Validate the agruments
  if( argc != 2 )
    {
      cout << "Incorrect Usage:" << endl;
      cout << "\tnaive inputFile" << endl;
      return -1;
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
  int result = loadGraph(&g, argv[1], &nodes, &edges, &distances);

    //printNodes(&g, nodes);

  if(result != 0)
    {
      cout << "Problem opening input file." << endl;
      return result;
    }

	vector<int> nodeCounter;
	
	for(int i = 0; i < nodes.size(); ++i)
	{
		nodeCounter.push_back(i);
	}

  //Sort the nodes so we're at the first permutation
	sort(nodeCounter.begin(), nodeCounter.end());


  //iterate through all of the permutations
  //and compute the length of each tour
  //Save the minimum tour length and the minimum tour
	vector<int> minimumPath = vector<int>(nodeCounter);
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
	}
	
	//Compute the bound/check if our path is within it
	//n is size of graph and k is k-regular-ness of it
	int n = minimumPath.size();
	int k = n;  //For our test data, it's been fully connected
	float bound = (1 + sqrt(64/log(n)) * n;
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

int loadGraph(Graph *g, char *input, vector<Node> *nodes, map<pair<int, int>, Edge> *edges, LengthMap *distances)
{
  //Change this vector to be an input parameter by reference
  //cout << "Initializing vector" << endl;
  //vector <Node> nodes;
  
  //cout << "Opening file" << endl;

  ifstream inFile;
  inFile.open(input);

  //cout << "Opened file" << endl;
  if(!inFile.is_open())
    {
      return -1;
    }
  //cout << "Checked if open" << endl;

  //Get size of graph
  int size = 0;
  float weight = 0;

  //cout << "Assigned initial values" << endl;

  inFile >> size;
  //cout << "Size is: " << size << endl;

  //Initialize graph
  for(int i = 0; i < size; ++i)
    {
      nodes->push_back(g->addNode());
    }

//LengthMap distances(*g);

  //Load in graph edges
  for(int i = 0; i < size; ++i)
    {
      for( int j = 0; j < size; ++j)
    {
      inFile >> weight;
		Edge newEdge = g->addEdge(nodes->at(i),nodes->at(j));
		edges->insert(make_pair(make_pair(i,j), newEdge));
		distances->set(newEdge, weight);
      //cout << weight << " ";
    }
      //cout << endl;
    }

  //Close our input file
  inFile.close();

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
		sum += (*distances)[edges[make_pair(path[i],path[i+1])]];
	}
	//Compute edge to return to source
	sum += (*distances)[edges[make_pair(path[path.size()-1],path[0])]];
	
	return sum;
}

void printPath(vector<int> path)
{
	for(int i = 0; i < path.size(); ++i)
	{
		cout << path[i] << " ";
	}
}