#include <iostream>
#include <memory>


#include "properties.h"
#include "steinertree.h"
#include "naiveshortestpath.h"

int main (int argv, char **argc) {
 
  //method from properties to init the 
  //variables
  rca::init_properties (argc[1]);
    
  NaiveShortestPath tree_maker;  
  TreeAlgorithm algorithm(tree_maker);
  
  std::shared_ptr<SteinerTree> st;
  for (int i=0; i < rca::g_groups.size(); i++) {
    algorithm.execute (i,st);
  
    std::cout << st->getCost() << std::endl;
  }
  
  return 0;
}