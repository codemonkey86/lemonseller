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
#include <cmath>
#include <string>
#include <cstdlib>
#include <ctime>
#include <set>
#include<sys/resource.h>
#include<sys/time.h>
#include<sys/wait.h>
#include<unistd.h>
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
int loadGraph(Graph*, int, vector<Node>*, map<pair<int, int>, Edge>*, LengthMap*);
void printNodes(Graph*, vector<Node>);
bool nodeCompare(Node, Node);
int computePathCost(vector<int>, LengthMap*, map<pair<int, int>, Edge>);
void printPath(vector<int>);
int run_naive(int, char**);
set<pair<int, int> > removed;

unsigned int maxStackLocation;
unsigned int minStackLocation;

int main(int argc, char** argv){
  //the first argument will be the size of the input graph.
  int child = fork();
  if (child == 0){
    run_naive(argc, argv);
    int who = RUSAGE_SELF;
    struct rusage usage;
    cout << "PID:" << getpid() << endl;
    int ret;
    ret = getrusage(who, &usage);
    double cpu_time_used = usage.ru_utime.tv_usec + usage.ru_stime.tv_usec;
     double seconds = usage.ru_utime.tv_sec + usage.ru_stime.tv_sec;
     double microseconds = usage.ru_utime.tv_usec + usage.ru_stime.tv_usec;
     cout << "RUN TIME:" << seconds +  microseconds/pow(10,6) << endl;
   }
  wait(NULL);
}

int run_naive(int argc, char** argv){
  if( argc < 2 || argc > 3 )
    {
      cout << "Incorrect Usage:" << endl;
      cout << "\tnaive size [- regular-ness (only 1 or 3 supported, 1 default)]" << endl;
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
    }

//LengthMap distances(*g);

  //Load in graph edges
  for(int i = 0; i < size; ++i)
    {
      for( int j = i+1; j < size; ++j)
    {
      //inFile >> weight;
		Edge newEdge = g->addEdge(nodes->at(i),nodes->at(j));
		edges->insert(make_pair(make_pair(i,j), newEdge));
		if(removed.find(make_pair(i,j)) != removed.end() || removed.find(make_pair(j,i)) != removed.end())
		{
			distances->set(newEdge, 2);
		}
		else
		{
			distances->set(newEdge,1);
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
