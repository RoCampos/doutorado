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
  algorithm.execute (0,st);
  
  std::cout << st.use_count () << std::endl;
  std::cout << st->getCost() << std::endl;
  
  return 0;
}