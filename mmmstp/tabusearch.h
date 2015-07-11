#ifndef _TABU_SEARCH_H_
#define _TABU_SEARCH_H_

#include <string>
#include <vector>
#include <bitset>

#include "sttree.h"
#include "network.h"
#include "edgecontainer.h"
#include "group.h"
#include "reader.h"
#include "steiner_tree_factory.h"
#include "steiner_tree_observer.h"

namespace rca{
	
	namespace metaalgo {

template <class SolutionType, class Container, class ObjectiveType>
class TabuSearch {
	
public:
	TabuSearch (std::string& );
	~TabuSearch () {
		delete m_factory;
	}
	
	inline void set_iterations (int iter) {m_iter = iter;}
// 	inline void set_budget (ObjectiveType budget) {m_budget = budget;}
	
	void run ();
	
	void build_solution (std::vector<SolutionType>& );
	void update_tabu ();
	
//*auxiliar methods
private:
	
	void update_container (SolutionType&, Container&, rca::Group&, rca::Network&);
	
private:
	
	ObjectiveType m_best;
	ObjectiveType m_cost;
	std::vector<SolutionType> m_best_sol;
	
	int m_iter; //número de iterações
	
	std::vector<int> m_tabu_list;
	
	/*--- Problem informations*/
	rca::Network m_network;
	std::vector<rca::Group> m_groups;
	
	ObjectiveType m_budget;
	
	rca::sttalgo::SteinerTreeFactory<Container> * m_factory;
	
	
};

	};
};

#endif