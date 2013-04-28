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
typedef Graph::EdgeMap<bool> BoolMap;
typedef Graph::NodeMap<Node> NodesMap;
//typedef MaxMatching<Graph> MaxMatching;

//Function prototypes
void printEdgeMap(Graph*, LengthMap*);
int run_permanent(int, char**);
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
  int child = fork();
  if (child == 0){
    run_permanent(argc, argv);
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
   else
     cout <<"ERROR FORKING";
  wait(NULL);
}
int run_permanent(int argc, char** argv){
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
		
		}
	}

  //Load the graph  
  Graph g;
  Graph g2;
  vector<Node> nodes2;
  map<pair<int,int>, Edge> edges2;
  
  vector<Node> nodes;
  map<pair<int, int>, Edge> edges;



  LengthMap distances(g);
  LengthMap distances2(g2);
  cout << size;
  //cout << "Load graph" << endl;
  //Get all of the nodes of the graph and put it in a list
  int result = loadGraph(&g, size, &nodes, &edges, &distances);
  printEdgeMap(&g, &distances);
    //printNodes(&g, nodes);
  //admittedly this is a hack to have get around lemon's
  //difficult copy/assignment mechanisms for edge maps 
  // we want the original weights available for computing cost at the end 
   int orig_weights[size*(size-1)/2][size*(size-1)/2];
    for (int i =0; i < size*(size-1)/2; i++)
      for (int j= 0; j < size*(size-1)/2; j++)
        orig_weights[i][j] = 0;
  for(EdgeIt i(g); i!=INVALID; ++i)
  {
      orig_weights[g.id(g.u(i))][g.id(g.v(i))] = (distances)[i];
      cout << "WEIGHT: " << distances[i] << " " << g.id(g.u(i)) << " " << g.id(g.v(i)) << endl;
 
  }


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
	
	for(EdgeIt e(tempGraph); e!=INVALID; ++e)
	{
		if(matching.matching(e))
			tempGraph.contract(tempGraph.u(e), tempGraph.v(e));
	}

	
	
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
	
	
	//Find Eulerian Tour
	//This is our TSP tour
	vector<int> minimumPath;
	set<int> traversed;
	for(EulerIt<ListGraph> e(g); e!=INVALID; e++) 
	{
               
		if(traversed.find(g.id(g.u(Edge(e)))) == traversed.end())
		{
			//Haven't visited this node yet
			//minimumPath.push_back(g.id(g.v(Edge(e))));
			//traversed.insert(g.id(g.v(Edge(e))));
			minimumPath.push_back(g.id(g.u(Edge(e))));
			traversed.insert(g.id(g.u(Edge(e))));
		}
	       if(traversed.find(g.id(g.v(Edge(e)))) == traversed.end())
		{		
			
			//already visited the previous node, haven't visited it's adjacent node
			minimumPath.push_back(g.id(g.v(Edge(e))));
			traversed.insert(g.id(g.v(Edge(e))));
		}
		else
                  continue;
			//This should only occur for the last edge.
                       
	}
   
         //this computing miin cost is admittedly hacky but seems to work!
	int sum = 0;
	//cout << "Computing cost of path size " << path.size() << endl;
        cout << "PATH SIZE" << minimumPath.size() << orig_weights[0][4] << endl;
	for(int i = 0; i < (minimumPath.size()-1); ++i)
	{
		if(minimumPath[i] < minimumPath[i+1])
		{
                    sum += orig_weights[minimumPath[i]][minimumPath[i+1]];
		//	sum = sum +  (*distances)[edges[make_pair(path[i],path[i+1])]];
			cout << minimumPath[i] << " " << minimumPath[i+1]<< " " << orig_weights[minimumPath[i]][minimumPath[i+1]] << endl;
		}
		else
		{
                       sum += orig_weights[minimumPath[i+1]][minimumPath[i]];
                        
		//	sum  = sum +  (*distances)[edges[make_pair(path[i+1],path[i])]];
                        cout << minimumPath[i+1] << " " << minimumPath[i] << " " << orig_weights[minimumPath[i+1]][minimumPath[i]] << endl;
		}
	}
        cout << "SUMMING: " << sum << endl;
	
              sum += orig_weights[minimumPath[0]][minimumPath.size()-1];
	









        int minimumCost = sum;

	
	//Compute the bound/check if our path is within it
	//n is size of graph and k is k-regular-ness of it
	int n = nodes.size();
	int k = n - regular;
        cout << n << endl;
        cout << k << endl;
	float bound = (1 + sqrt(64/log(k))) * n; 
        cout << bound;
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
 

  //Get size of graph
  //int size = 0;
  float weight = 0;

  

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
	
    }
      
    }



  return 0;
}

bool nodeCompare(Node i, Node j)
{
	return (i<j);
}


void printPath(vector<int> path)
{
	for(int i = 0; i < path.size(); ++i)
	{
		cout << path[i] << " ";
	}
}
