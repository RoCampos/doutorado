#ifndef _SOLVER_SOLUTION_TEST_
#define _SOLVER_SOLUTION_TEST_

#include <iostream>
#include <string>
#include <cassert>
#include <memory>
#include <sstream>
#include <map>

#include "network.h"
#include "reader.h"
#include "group.h"

template <typename Solver>
class SolverSolution : private Solver
{
	
	using Solver::do_test;
	
public:
	
	bool test (std::string instance, std::string result, int objective) {		
		return do_test (instance, result,  objective);
	}
	
};

typedef std::vector<std::shared_ptr<rca::Group> >  list_groups;
class MMMSTPGurobiResult {

public:
	bool do_test (std::string instance, std::string result, int);
	
private:
	//computa o valor objetivo com base nas árvores geradas
	int objective_test (rca::Network *,list_groups, std::string result);
	
	//faz os teste necessários para garantir que há uma árvore
	//de steiner para cada group
	int steiner_tree_test (rca::Network *,rca::Group *g, std::string result);
	
	int cost (rca::Network *, std::string result);
	
	//faz o teste de connectividade da árvore
	//considerando que todos os terminais devem estar no mesmo
	//disjoint set
	bool connectivity (rca::Group *g, DisjointSet2& dset, int numberOfnodes);
	
	
	/**
	 * Este teste verifica se há algum nó folha não terminal que 
	 * seja folha. Se for o teste considerado falho,  o resultado é inválido e
	 * o programa encerra.
	 */
	bool non_terminal_leaf_test (std::vector<int>&, rca::Group *g);
	
	int count_terminals (std::vector<int>&, rca::Group *g);
	
	int tree_cost (rca::Network *, rca::Group *g, 
					  DisjointSet2& dset, std::string);
	
private:
	int m_verbose;

};

#endif