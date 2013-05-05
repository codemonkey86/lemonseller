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
    int real_numarcs = 0;
    int numarcs = 15000;
    int numvertices = 15000;
   
    time_t begin_karp, end_karp, begin_howard, end_howard;
	
    time(&begin_howard);
    cout << endl << "---------- Howard ----------" << endl;
    HowardMmc<StaticDigraph, StaticDigraph::ArcMap<int> > howard_algo(gr, map);
    if (howard_algo.run()){
        cout << "\tCost -- Arcs -- MMC" << endl;
        cout << "Howard:\t" << howard_algo.cycleCost() << "\t" << howard_algo.cycleSize() << "\t" << howard_algo.cycleMean() << endl;
        if (PATH_INFO) {
            Path<StaticDigraph> howard_path = howard_algo.cycle();
            cout << endl << "Howard path:";
            for (int i=0; i < howard_path.length(); i++){
                cout<< endl << i + 1 << "-ARC from " << gr.id(gr.source(howard_path.nth(i))) << " to " << gr.id(gr.target(howard_path.nth(i)));
            }
            cout << endl;
        }
    } else {
        cout << "Cycle free - Please retry!" << endl;
    }
    time(&end_howard);
    cout << "Time elapsed: " << difftime(end_howard, begin_howard) << endl;
   
    time(&begin_karp);
    cout << endl << "----------- Karp -----------" << endl;
    KarpMmc<StaticDigraph, StaticDigraph::ArcMap<int> > karp_algo(gr, map);
    if (karp_algo.run()){
        cout << "\tCost -- Arcs -- MMC" << endl;
        cout << "Karp:\t" << karp_algo.cycleCost() << "\t" << karp_algo.cycleSize() << "\t" << karp_algo.cycleMean() << endl;
        if (PATH_INFO) {
            Path<StaticDigraph> karp_path = karp_algo.cycle();
            cout << endl << "Karp Path:";
            for (int i=0; i < karp_path.length(); i++){
                cout << endl << i + 1 << "-ARC from " << gr.id(gr.source(karp_path.nth(i))) << " to " << gr.id(gr.target(karp_path.nth(i)));
            }
            cout << endl;
        }
    } else {
        cout << "Cycle free - Please retry!" << endl;
    }
    time(&end_karp);
    cout << "Time elapsed: " << difftime(end_karp, begin_karp) << endl;
   
    //TODO maybe compare with ListGraph, explore tolerancce
}