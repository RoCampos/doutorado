#include <iostream>

#include "strategy_make_tree.h"
#include "steinertree.h"

int main (int argv, char**argc) {
 
  ShortestPathTree tree_maker;
  
  TreeAlgorithm algorithm(tree_maker);
  
  std::shared_ptr<SteinerTree> st;
  algorithm.execute (st);
  
  return 0;
}