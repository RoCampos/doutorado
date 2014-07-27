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
  
  for (int i=0; i < 1000; i++) {
    std::shared_ptr<SteinerTree> st;
    for (int i=0; i < rca::g_groups.size(); i++) {
      algorithm.execute (i,st);
    }
  }	
  return 0;
}