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
	
	void cost_tabu_based (std::vector<SolutionType>&);
	
	
	
	void zig_zag (std::vector<SolutionType>&, 
					ObjectiveType& res, 
					ObjectiveType& cost);
	
//*auxiliar methods
private:
	
	ObjectiveType update_container (SolutionType&, Container&, rca::Group&, rca::Network&);
	
	std::vector<rca::Link> tabu_list (std::vector<SolutionType>&);	
	
	ObjectiveType improvement (std::vector<SolutionType>&, int&);
	
	void update_best_solution (std::vector<SolutionType>&,
								const ObjectiveType,
								const ObjectiveType);
	
	//receives group id
	void remove_tabu_links (int g_id) {
		for (auto l : this->m_links_tabu) {
			if (m_network.isRemoved(l)) continue;
			
			m_network.removeEdge (l);
			
			if ( !is_connected (m_network, m_groups[g_id]) ) {
				m_network.undoRemoveEdge (l);
			}
		}
	}
	
	
	//update tabu list based on 
	void redo_tabu_list (std::vector<rca::Link> & links_cost) {		
			
		m_links_tabu.clear ();
		for (int i=0; i < links_cost.size ()*0.1; i++) {
			m_links_tabu.push_back (links_cost[i]);
		}	
	}
	
private:
	
	//objective value
	ObjectiveType m_best;
	
	//solution cost
	ObjectiveType m_cost;
	
	//solution representation
	std::vector<SolutionType> m_best_sol;
	
	//max iteration number
	int m_iter; 
	
	//indicates if the firs solutions has been created
	bool m_has_init;
	
	std::vector<int> m_tabu_list; //control each tree will be build
	std::vector<int> m_best_cost; //used as tabu
	
	//links used as tabu - based on cost
	std::vector<rca::Link> m_links_tabu; 
	
	/*--- Problem informations*/
	rca::Network m_network;
	std::vector<rca::Group> m_groups;
	
	//limit of budget
	ObjectiveType m_budget;
	
	//multicast tree factory
	rca::sttalgo::SteinerTreeFactory<Container> * m_factory;
	
	
};

	};
};

#endif