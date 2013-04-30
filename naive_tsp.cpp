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
			
		}
	}

  //Load the graph  
  Graph g;

  vector<Node> nodes;
  map<pair<int, int>, Edge> edges;




  LengthMap distances(g);


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
