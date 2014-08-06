#ifndef _MYCHENALGORITHM_
#define _MYCHENALGORITHM_

#include <iostream>
#include <string>

#include <memory>

#include "network.h"
#include "reader.h"
#include "strategy_make_tree.h"


typedef typename std::vector<std::shared_ptr<rca::Group>> MulticatGroups;
typedef typename std::shared_ptr<rca::Network> RCANetwork;
/**
 * Classe que representa meu algoritmo
 * Esta classe contém as rotinas utilizadas para resolver
 * o Multicast Packing Problem com um objetivo.
 * 
 * @author Romerito Campos.
 * @version 0.1 06/08/2014
 */
template<typename TreeStrategy>
class MPPAlgorithm {

public:
	MPPAlgorithm (RCANetwork &, MulticatGroups &);
	
	void init_strategy (const TreeStrategy & T);
	
	void run ();
	
private:
	
	TreeStrategy strategy;
	RCANetwork m_network;
	MulticatGroups m_groups;
    
};

#endif