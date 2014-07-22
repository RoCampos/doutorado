#ifndef _STRATEGY_MAKE_TREE_
#define _STRATEGY_MAKE_TREE_

#include <iostream>
#include <memory>

#include "steinertree.h"

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
  /*Este método resolve uma árvore de Steiner*/
  virtual std::shared_ptr<SteinerTree> & make_tree () = 0;
  
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
      m_strategy->make_tree ();
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
  std::shared_ptr<SteinerTree> & make_tree () {
    std::cout << "Shortest Path\n";    
    std::make_shared<SteinerTree> (0,0);
    std::shared_ptr<SteinerTree> st;
    return st;
  }
  
};

/**
 * Esta classe representa uma algoritmo simples para
 * criação de árvores de steiner usando o algoritmo
 * de caminho mais curto entre nós terminais.
 * 
 */ 
class NaiveShortestPath : public TreeAlgorithmInterface {

public:
  std::shared_ptr<SteinerTree> & make_tree () {
    std::cout << "Naive Shortest Path\n";
    std::shared_ptr<SteinerTree> st = std::make_shared<SteinerTree> (0,0);
    return st;
  }
  
};

#endif /*_STRATEGY_MAKE_TREE_*/