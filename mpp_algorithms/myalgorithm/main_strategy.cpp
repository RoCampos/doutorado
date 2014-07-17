#include <iostream>

#include "strategy_make_tree.h"
//#include "strategy_make_tree.cpp"

int main (int argv, char**argc) {
 
  ShortestPathTree tree_maker;
  
  TreeAlgorithm algorithm(tree_maker);
  
  algorithm.execute ();
  
  return 0;
}