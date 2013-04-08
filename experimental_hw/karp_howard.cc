//To compile:   g++ -o karp_howard karp_howard.cc -lemon

#include <iostream>
#include <lemon/static_graph.h>
#include<lemon/karp_mmc.h>
#include<lemon/howard_mmc.h>
#include<lemon/concepts/digraph.h>
#include<lemon/concepts/path.h>
#include<sys/resource.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/time.h>

using namespace lemon;
using namespace std;


int main (){
  
   std::vector<std::pair<int,int> > arcs;
  
   //static graphs are  most efficient, but cannot be modified
   // can only be "built"
    //TODO: codecleanup, pull stuff into functions, etc

    int numarcs =  rand() % 100+ 1;
    int numvertices = rand() % 100 + 1;
    cout <<"Number of arcs: " << numarcs;
    cout << "Number of vertices: " << numvertices;
     //RANDOMIZATION IN PROGRESS, currently violates requirement (and hence coredumps)
     //of pushing arcs in order of source, refactor this to generate paris first then order
     // by source, then push_back

  //  for (int i = 0; i < numarcs; i++){
  //    arcs.push_back(std::make_pair(rand() % numvertices, rand() % numvertices)); //watch indexing here, start at 0 so no +1
   // }
      
    arcs.push_back(std::make_pair(0,1));
   arcs.push_back(std::make_pair(0,2));
   arcs.push_back(std::make_pair(1,3));
   arcs.push_back(std::make_pair(1,2));
   arcs.push_back(std::make_pair(3,0)); 
   StaticDigraph gr;
   gr.build(numvertices, arcs.begin(), arcs.end());
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
    

 
   if (karp_algo.run() && howard_algo.run()){
   cout << "Minimum cycle mean found Karp " << karp_algo.cycleMean();
   cout << "Minimum cycle mean found Howard " << howard_algo.cycleMean();
   cout << "Number of arcs on Cycle found Karp: " << karp_algo.cycleSize();
   cout << "Number of arcs on Cycle found Howard: " << howard_algo.cycleSize();
   Path<StaticDigraph> p = karp_algo.cycle();
   concepts::Path<StaticDigraph>::Arc a(karp_algo.cycle().nth(0));
   //THIS is verification that we get a path may not belong in final code,  also, ARCIT may be preferrable, but I had trouble getting it to work properly
   for (int i=0;i<p.length(); i++){
     cout<< endl << "ARC from " << gr.id(gr.source(howard_algo.cycle().nth(i))) << " to " << gr.id(gr.target(howard_algo.cycle().nth(i)));
   }

  //TODO maybe compare with ListGraph, explore tolerancce
   }

 

}

