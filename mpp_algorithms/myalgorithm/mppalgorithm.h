#ifndef _MYCHENALGORITHM_
#define _MYCHENALGORITHM_

#include <iostream>
#include <string>

#include <memory>

#include "network.h"
#include "reader.h"

/**
 * Classe que representa meu algoritmo
 * Esta classe contém as rotinas utilizadas para resolver
 * o Multicast Packing Problem com um objetivo.
 * 
 * @author Romerito Campos.
 * @version 0.1 07/11/2014
 */
class MPPAlgorithm {
  
public:
  MPPAlgorithm ();
  MPPAlgorithm (std::string algo, std::string roblem, 
		int objectives, std::string instance);
  
private:
  std::string m_algo_info; 
  std::string m_problem;
  int m_objectives;
 
  //variábeis do problema
  std::string m_instance;
  std::shared_ptr<rca::Network> m_net;
  std::vector<shared_ptr<rca::Group>> m_groups;
  
public:
  
  //método para iniciar variábeis do problema
  void init (rca::Network & network, 
	     std::vector<shared_ptr<rca::Group>> groups);
  
  //método para rodar os algoritmos
  void run ();
  
    
};

#endif