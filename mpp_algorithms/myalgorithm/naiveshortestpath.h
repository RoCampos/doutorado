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
  
  /**
   * Este método é a implementação do método make_tree
   * de TreeAlgorithmInterface.
   */
  void make_tree (int group_id,std::shared_ptr<SteinerTree> & st);
  
private:
  //Método auxiliar que obtém os caminhos entre pares de terminais
  //do Grupo group_id
  void getPaths (int group_id, std::vector<rca::Path> & paths);
  
  //Método auxiliar que cria uma steiner tree com os caminhos
  //obtidos por getPaths
  void create_steiner_tree (int group_id, std::vector<rca::Path> & paths, 
			    std::shared_ptr<SteinerTree> & st);
  
};

#endif