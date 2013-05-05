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

int main () {
    bool DEBUG = false;
    bool PATH_INFO = false;

    std::vector<std::pair<int,int> > temp_vector;
    std::vector<std::pair<int,int> > arcs;
    srand (time(NULL));
    //int numarcs = (rand() % 10) + 16;     // range 16-26
    //int numvertices = (rand() % 5) + 8;   // range 8-13
    int numarcs = 75;
    int real_numarcs = 0;
    int numvertices = 100;
   
    cout << "Number of extra arcs: " << numarcs << endl;
    cout << "Number of vertices: " << numvertices << endl;
   
    // Start with line from 0 -> 1 -> ... -> (numvertices - 1) 
    // this insures the graph is connected
    if (DEBUG) cout << "Building line graph" << endl;
    for (int i = 0; i < numvertices - 1; i++) {
        temp_vector.push_back(std::make_pair(i, i+1));
    }

    // Adds arcs of random source and target
    if (DEBUG) cout << "Adding extra arcs" << endl;
    for (int i = 0; i < numarcs; i++) {
        temp_vector.push_back(std::make_pair(rand() % numvertices, rand() % numvertices));
    }

    // Removing duplicates and self-loops
    if (DEBUG) cout << "Sorting arcs" << endl;
    std::sort(temp_vector.begin(), temp_vector.end());
    if (DEBUG) cout << "Removing duplicate arcs" << endl;
    temp_vector.erase(unique(temp_vector.begin(), temp_vector.end()), temp_vector.end());
    if (DEBUG) cout << "Removing self-loops" << endl;

    for (std::vector< std::pair<int,int> >::iterator it=temp_vector.begin(); it!=temp_vector.end(); ++it){
        if (it->first != it->second) {
            arcs.push_back(std::make_pair(it->first, it->second));
            real_numarcs++;
        }
    }
    if (DEBUG) {
        cout << "List of arcs" << endl;
        for (std::vector< std::pair<int,int> >::iterator it=arcs.begin(); it!=arcs.end(); ++it){
            cout << "(" << it->first << "," << it->second << ")" << endl;
        }
        cout << "Number of arcs: " << real_numarcs << endl;

    }
   
    StaticDigraph gr;
    gr.build(numvertices, arcs.begin(), arcs.end());
    StaticDigraph::ArcMap<int> map(gr);

    // Adding random weights
    if (DEBUG) cout << "Adding weights to arcs" << endl;
    for (int i = 0; i < real_numarcs; i++) {
        map[gr.arc(i)] = (rand() % 10) + 1;
    }
   
    KarpMmc<StaticDigraph, StaticDigraph::ArcMap<int> > karp_algo(gr, map);
    HowardMmc<StaticDigraph, StaticDigraph::ArcMap<int> > howard_algo(gr, map);

    cout << endl;
    if (karp_algo.run() && howard_algo.run()){
        cout << "\tCost -- Arcs -- MMC" << endl;
        cout << "Karp:\t" << karp_algo.cycleCost() << "\t" << karp_algo.cycleSize() << "\t" << karp_algo.cycleMean() << endl;
        cout << "Howard:\t" << howard_algo.cycleCost() << "\t" << howard_algo.cycleSize() << "\t" << howard_algo.cycleMean() << endl;
        if (PATH_INFO) {
            Path<StaticDigraph> karp_path = karp_algo.cycle();
            Path<StaticDigraph> howard_path = howard_algo.cycle();
           
            cout << endl << "Karp path:";
            for (int i=0; i < karp_path.length(); i++){
                cout << endl << i + 1 << "-ARC from " << gr.id(gr.source(karp_path.nth(i))) << " to " << gr.id(gr.target(karp_path.nth(i)));
            }
            cout << endl << "Howard path:";
            for (int i=0; i < karp_path.length(); i++){
                cout<< endl << i + 1 << "-ARC from " << gr.id(gr.source(howard_path.nth(i))) << " to " << gr.id(gr.target(howard_path.nth(i)));

            }
            cout << endl;
        }
    } else {
        cout << "Cycle free - Please retry!" << endl;
    }
    //TODO maybe compare with ListGraph, explore tolerancce
}