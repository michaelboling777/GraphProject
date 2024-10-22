#include "graph.h"
#include "graph_dumb_vector.h"
#include <iostream>

using namespace std;


template<typename graphID>
void test_graph(){
  typedef typename graphID::edge_descriptor ED;
    
  graphID g;
  g.insert_vertex(5);
  g.insert_vertex(4);
  g.insert_edge(0, 1, 0.5);
  g.insert_edge(1, 0, 0.25);
  
  if(g.find_vertex(0) == g.vertices_end()){
	cout<< "Insert vertex failed" <<endl;
	return;
  }
  if(g.find_edge(ED(0, 1)) == g.edges_end()){
	 cout<<"Insert edge failed" << endl;
	return; 
  }
  cout << "Graph after insertion" << endl;
  cout << g;
  
  g.erase_edge(ED(0, 1));
  if(g.find_edge(ED(0, 1)) != g.edges_end()){
	 cout<<"Erase edge failed" << endl;
	return; 
  }
  g.erase_vertex(0);
  if(g.find_vertex(0) != g.vertices_end()){
	cout<< "Erase vertex failed" <<endl;
	return;
  }
  cout << "Graph after deletion" << endl;
  cout << g;	
}

int main() {
  typedef graph<int, double> setGraph;
  typedef graph_vector<int, double> vectorGraph;
  
  test_graph<vectorGraph>();
  test_graph<setGraph>();
}
