#ifndef _NAIVESHORTESTPATH_H_
#define _NAIVESHORTESTPATH_H_

#include <iostream>
#include <memory>

#include "strategy_make_tree.h"

#include "steinertree.h"


/**
 * Esta classe representa uma algoritmo simples para
 * criação de árvores de steiner usando o algoritmo
 * de caminho mais curto entre nós terminais.
 * 
 */ 
class NaiveShortestPath : public TreeAlgorithmInterface {

public:
  void make_tree (std::shared_ptr<SteinerTree> & st) {
    std::cout << "Naive Shortest Path\n";
    st = std::make_shared<SteinerTree> (1,0);    
  }
  
};

#endif