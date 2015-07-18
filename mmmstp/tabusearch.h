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
#include "sttree_local_search.h"

#include "mpp_visitor.h"

namespace rca{
	
	namespace metaalgo {

template <class V, class Z, class X>
class TabuSearch {
	
	typedef V SolutionType;
	typedef Z Container;
	typedef X ObjectiveType;
	
public:
	TabuSearch (std::string& );
	~TabuSearch () {
		delete m_factory;
	}
	
	inline void set_iterations (int iter) {m_iter = iter;}
	inline void set_budget (ObjectiveType budget) {m_budget = budget;}
	
	void run ();
	
	void build_solution (std::vector<SolutionType>& ,
						ObjectiveType& res, 
						ObjectiveType& cost);
	
	void update_tabu ();
	
	void cost_tabu_based (std::vector<V>&);
	
	
//*auxiliar methods
private:
	
	ObjectiveType update_container (SolutionType&, Container&, rca::Group&, rca::Network&);
	
	std::vector<rca::Link> tabu_list (std::vector<SolutionType>&);	
	
	ObjectiveType improvement (std::vector<SolutionType>&, int&);
	
	void update_best_solution (std::vector<SolutionType>&,
								const ObjectiveType,
								const ObjectiveType);
	
private:
	
	ObjectiveType m_best;
	ObjectiveType m_cost;
	std::vector<SolutionType> m_best_sol;
	
	int m_iter; //número de iterações
	
	std::vector<int> m_tabu_list; //control each tree will be build
	std::vector<int> m_best_cost; //used as tabu
	
	std::vector<rca::Link> links_tabu;
	
	/*--- Problem informations*/
	rca::Network m_network;
	std::vector<rca::Group> m_groups;
	
	ObjectiveType m_budget;
	
	rca::sttalgo::SteinerTreeFactory<Container> * m_factory;
	
	
};

	};
};

#endif