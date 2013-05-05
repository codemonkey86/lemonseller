//To compile:   g++ -o karp_howard karp_howard.cc -lemon

#include <iostream>
#include <algorithm>
#include <vector>      
#include <lemon/static_graph.h>
#include <lemon/karp_mmc.h>
#include <lemon/howard_mmc.h>
#include <lemon/concepts/digraph.h>
#include <lemon/concepts/path.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

using namespace lemon;
using namespace std;

int main (){
    bool DEBUG = false;
    std::vector<std::pair<int,int> > temp_vector;
    std::vector<std::pair<int,int> > arcs;
    srand (time(NULL));
    //int numarcs = (rand() % 10) + 16;     // range 16-26
    //int numvertices = (rand() % 5) + 8;   // range 8-13 
    int numarcs = 75;
    int numvertices = 100;
    cout << "Building Graph" << endl;
    cout << "Number of arcs: " << numarcs << endl;
    cout << "Number of vertices: " << numvertices << endl;
    for (int i = 0; i < numarcs; i++) {
        temp_vector.push_back(std::make_pair(rand() % numvertices, rand() % numvertices));
        if ( i + 1 < numvertices) {
            temp_vector.push_back(std::make_pair(i, i+1));
        }
    }
    // Removing duplicates and self-loops
    std::sort(temp_vector.begin(), temp_vector.end());
    temp_vector.erase(unique(temp_vector.begin(), temp_vector.end()), temp_vector.end());
    for (std::vector< std::pair<int,int> >::iterator it=temp_vector.begin(); it!=temp_vector.end(); ++it){
        if (it->first != it->second) {
            arcs.push_back(std::make_pair(it->first, it->second));
        }
    }
    // Prints elements if DEBUG is set
    if (DEBUG) {
        for (std::vector< std::pair<int,int> >::iterator it=arcs.begin(); it!=arcs.end(); ++it){
            cout << "(" << it->first << "," << it->second << ")" << endl;
        }
    }
    else {
    StaticDigraph gr;
    gr.build(numvertices, arcs.begin(), arcs.end());
    StaticDigraph::ArcMap<int> map(gr);
   
    for (int i = 0; i < numarcs; i++) {
        map[gr.arc(i)] = (rand() % 10) + 1;
    }
   
    KarpMmc<StaticDigraph, StaticDigraph::ArcMap<int> > karp_algo(gr, map);
    HowardMmc<StaticDigraph, StaticDigraph::ArcMap<int> > howard_algo(gr, map);

    cout << endl;
    if (karp_algo.run() && howard_algo.run()){
        cout << "Number of arcs on cycle" << endl;
        cout << "Karp:\t" << karp_algo.cycleSize() << endl;
        cout << "Howard:\t" << howard_algo.cycleSize() << endl;
        cout << "Cost of cycle" << endl;
        cout << "Karp:\t" << karp_algo.cycleCost() << endl;
        cout << "Howard:\t" << howard_algo.cycleCost() << endl;
        cout << "Minimum Cycle Mean" << endl;
        cout << "Karp:\t" << karp_algo.cycleMean() << endl;
        cout << "Howard:\t" << howard_algo.cycleMean() << endl;
        //Path<StaticDigraph> p = karp_algo.cycle();
        //concepts::Path<StaticDigraph>::Arc a(karp_algo.cycle().nth(0));
        //THIS is verification that we get a path may not belong in final code,  also, ARCIT may be preferrable, but I had trouble getting it to work properly
        //for (int i=0;i<p.length(); i++){
        //    cout<< endl << "ARC from " << gr.id(gr.source(howard_algo.cycle().nth(i))) << " to " << gr.id(gr.target(howard_algo.cycle().nth(i)));
        //}
        cout << endl;
    } else {
        cout << "Cycle free - Please retry!" << endl;
    }
    }
    //TODO maybe compare with ListGraph, explore tolerancce
}