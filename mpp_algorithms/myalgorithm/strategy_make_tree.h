#ifndef _STRATEGY_MAKE_TREE_
#define _STRATEGY_MAKE_TREE_

#include <iostream>
#include <memory>

#include "steinertree.h"
#include "network.h"
#include "group.h"
#include "algorithm.h"

/**
 * Classe que representa de forma abstra uma estratégia
 * para construir árvores de Steiner.
 * 
 * @author Romerito Campos.
 * @version 0.1 
 * @date 06/17/2014
 */
class TreeAlgorithmInterface {
 
public:
  
  TreeAlgorithmInterface (rca::Network & net, 
			  std::vector<std::shared_ptr<rca::Group>> & groups){
    m_net = net;
    m_groups = groups;
    
  }
  
  /*Este método resolve uma árvore de Steiner*/
  virtual void make_tree (std::shared_ptr<SteinerTree> & st) = 0;
  
public:
  rca::Network m_net;
  std::vector<std::shared_ptr<rca::Group>> m_groups;
  
  
};

/**
 * Classe usada para armazenar e gerenciar a estratégia
 * utilizada pelo algoritmo. 
 * Esta classe mantém uma instância para o tipo de estratégia
 * utilizada para construir uma árvore de steiner.
 * 
 * @author Romerito Campos.
 * @version 0.1
 * @date 06/17/2014
 */
class TreeAlgorithm {
  
public:
  
  TreeAlgorithm (TreeAlgorithmInterface & param);
  
  void execute (std::shared_ptr<SteinerTree> & st) {
    
    if (m_strategy != NULL) {         
     
      m_strategy->make_tree(st);     
     
    }
    
  }
  
  void set_strategy (TreeAlgorithmInterface & strategy) {
    m_strategy = &strategy;
  }
  
private:
  
  TreeAlgorithmInterface * m_strategy;
  
};

/**
 * Classe que implementa a construção de árvores de 
 * Steiner com base em caminhos mais curtos.
 * 
 * @author Romerito Campos.
 * @version 0.1
 * @date 06/17/2014
 */
class ShortestPathTree : public TreeAlgorithmInterface {
  
public:
  void make_tree (std::shared_ptr<SteinerTree> & st) {
    std::cout << "Shortest Path\n";
    std::make_shared<SteinerTree>(1,0);
  }
  
};

#endif /*_STRATEGY_MAKE_TREE_*/