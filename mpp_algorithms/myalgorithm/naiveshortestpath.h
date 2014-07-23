#ifndef _NAIVESHORTESTPATH_H_
#define _NAIVESHORTESTPATH_H_

#include <iostream>
#include <memory>

#include "strategy_make_tree.h"
#include "steinertree.h"

#include "path.h"

#include "properties.h"


/**
 * Esta classe representa uma algoritmo simples para
 * criação de árvores de steiner usando o algoritmo
 * de caminho mais curto entre nós terminais.
 * 
 */ 
class NaiveShortestPath : public TreeAlgorithmInterface {

public:
  
  void make_tree (std::shared_ptr<SteinerTree> & st);
  
private:
  void getPaths (int group_id, std::vector<rca::Path> & paths);
  void create_steiner_tree (int group_id, std::vector<rca::Path> & paths, 
			    SteinerTree * st);
  
};

#endif