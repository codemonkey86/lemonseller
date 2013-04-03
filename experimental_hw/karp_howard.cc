//To compile:   g++ -o karp_howard karp_howard.cc -lemon

#include <iostream>
#include <lemon/static_graph.h>
#include<lemon/karp_mmc.h>
#include<lemon/howard_mmc.h>
#include<lemon/concepts/digraph.h>
#include<sys/resource.h>
#include<unistd.h>
using namespace lemon;
using namespace std;


int main (){
  
   std::vector<std::pair<int,int> > arcs;
  
   //static graphs are  most efficient, but cannot be modified
   // can only be "built"
    //TODO: random generation of number and pairing of arcs
   arcs.push_back(std::make_pair(0,1));
   arcs.push_back(std::make_pair(0,2));
   arcs.push_back(std::make_pair(1,3));
   arcs.push_back(std::make_pair(1,2));
   arcs.push_back(std::make_pair(3,0));
   StaticDigraph gr;
   gr.build(4, arcs.begin(), arcs.end());
   StaticDigraph::ArcMap<int> map(gr);

   //TODO: random generation of cost function
   map[gr.arc(0)] = 5;
   map[gr.arc(1)] = 6;
   map[gr.arc(2)] = 4;
   map[gr.arc(3)] = 7;  
   map[gr.arc(4)] = 3;
  
 
  cout<<"Hello world lemon-karp";
 
  KarpMmc<StaticDigraph, StaticDigraph::ArcMap<int> > karp_algo(gr, map);
  HowardMmc<StaticDigraph, StaticDigraph::ArcMap<int> > howard_algo(gr,map);
   //TODO: Cost/Time analysis
/* this stuff not working yet, have email pending also inquiring about profiler usage external to code for time/space
   int child = fork();
   if (child == 0){
     karp_algo.run();
     int who = RUSAGE_SELF;
     struct rusage usage;
     int ret;
     ret = getrusage(who, &usage); 
     double cpu_time_used = usage.ru_utime + usage.ru_stime; 
   }
*/

   if (karp_algo.run() && howard_algo.run()){
   cout << "Minimum cycle mean found Karp " << karp_algo.cycleMean();
   cout << "Minimum cycle mean found Howard " << howard_algo.cycleMean();
   cout << "Number of arcs on Cycle found Karp: " << karp_algo.cycleSize();
   cout << "Number of arcs on Cycle found Howard: " << howard_algo.cycleSize();
  //TODO return <algo>.cycle() and iterate through as sanity check for correctness
  //TODO maybe compare with ListGraph, explore tolerancce
   }

 

}

