#include <iostream>
#include <memory>


#include "properties.h"
#include "steinertree.h"
#include "breadth_search_tree.h"

int main (int argv, char **argc) {
 
  //method from properties to init the 
  //variables
  rca::init_properties (argc[1]);
    
  BeadthSearchTree tree_maker;  
  TreeAlgorithm algorithm(tree_maker);
  
  std::vector<std::vector<int>> congestion (30);
  for (int i=0; i < 30; i++) {
     congestion[i] = std::vector<int> (30,0);
  }
  
  //for (int i=0; i < 1000; i++) {
    std::shared_ptr<SteinerTree> st;
    for (unsigned int i=0; i < rca::g_groups.size(); i++) {
      //cout << "tree " << i << endl;
      
      if (!rca::g_network->isConnected ())
	 rca::g_network->clearRemovedEdges ();
      
      algorithm.execute (i,st);   
      st->xdotFormat ();
      
      Edge * e = st->listEdge.head;
      while (e != NULL) {
	
	congestion[e->i][e->j]++;
	congestion[e->j][e->i]++;
	
	e = e->next;
      }	
      e = NULL;
    }
    
    int aux = 0;
    for (int i=0; i< 30; i++) {
      for (int j=0; j < i; j++) {
	if (congestion[i][j] > aux) {
	  aux = congestion[i][j];
	}
      }      
    }
    
    cout << aux << endl;
    
  //}	
  return 0;
}