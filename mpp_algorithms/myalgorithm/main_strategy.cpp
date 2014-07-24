#include <iostream>

#include "strategy_make_tree.h"
#include "steinertree.h"
#include "naiveshortestpath.h"

#include "network.h"
#include "reader.h"

using namespace rca;

int main (int argv, char**argc) {
 
  rca::Network network;
  std::vector<std::shared_ptr<Group>> groups;
  
  //reading informations about network and groups
  Reader r(argc[1]);
  r.configNetwork (&network);
  groups = r.readerGroup ();
  
  NaiveShortestPath tree_maker;  
  TreeAlgorithm algorithm(tree_maker);
  
  std::shared_ptr<SteinerTree> st;
  algorithm.execute (0,st);
  
  std::cout << st.use_count() << std::endl;
  
  return 0;
}