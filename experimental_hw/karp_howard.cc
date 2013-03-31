//To compile:   g++ -o karp_howard karp_howard.cc -lemon

#include <iostream>
#include <lemon/static_graph.h>
#include<lemon/karp_mmc.h>
#include<lemon/howard_mmc.h>
#include<lemon/concepts/digraph.h>
using namespace lemon;
using namespace std;


int main (){
   //TODO: Random generation of graphs of random sizes
   std::vector<std::pair<int,int> > arcs;
   arcs.push_back(std::make_pair(0,1));
   arcs.push_back(std::make_pair(0,2));
   arcs.push_back(std::make_pair(1,3));
   arcs.push_back(std::make_pair(1,2));
   arcs.push_back(std::make_pair(3,0));
   StaticDigraph gr;
   gr.build(4, arcs.begin(), arcs.end());
   StaticDigraph::ArcMap<int> map(gr);
   //TODO work with arcMap to establish cost function
  cout<<"Hello world lemon-karp";
 
  KarpMmc<StaticDigraph, StaticDigraph::ArcMap<int> > karp_algo(gr, map);
  HowardMmc<StaticDigraph, StaticDigraph::ArcMap<int> > howard_algo(gr,map);
   if (karp_algo.run() && howard_algo.run()){
   cout << "Minimum cycle mean found Karp " << karp_algo.cycleMean();
   cout << "Minimum cycle mean found Howard " << howard_algo.cycleMean();
   cout << "Number of arcs on Cycle found Karp: " << karp_algo.cycleSize();
   cout << "Number of arcs on Cycle found Howard: " << howard_algo.cycleSize();
  //TODO return <algo>.cycle() and iterate through to display cycle found
  //TODO wrap this in time space analysis, maybe compare with ListGraph, explore tolerancce
   }

 

}

