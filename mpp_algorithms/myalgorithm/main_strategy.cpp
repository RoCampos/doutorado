#include <iostream>

#include "strategy_make_tree.h"

int main (int argv, char**argc) {
 
  ShortestPathTree tree_maker;
  
  TreeAlgorithm algorithm(tree_maker);
  
  algorithm.execute ();
  
  return 0;
}