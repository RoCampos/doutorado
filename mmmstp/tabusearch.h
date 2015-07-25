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

#include "rcatime.h"

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
	inline void set_has_init (bool value) {m_has_init = value;}
	inline void set_tabu_links_size (double value){m_links_perc = value;}
		
	
	void run ();
	
	void build_solution (std::vector<SolutionType>& ,
						ObjectiveType& res, 
						ObjectiveType& cost);
	
	void update_tabu ();
	
	void cost_tabu_based (std::vector<SolutionType>&);
	
	
	
	void zig_zag (std::vector<SolutionType>&, 
					ObjectiveType& res, 
					ObjectiveType& cost);
	
	void improvement (std::vector<SolutionType>&, int&, int&);
	
	void zig (std::vector<SolutionType>&, 
			  ObjectiveType& res, ObjectiveType& cos);
	
//*auxiliar methods
private:
	
	ObjectiveType update_container (SolutionType&, Container&, rca::Group&, rca::Network&);
	
	/**
	 * Método utilizado para criar uma lista tabu com as arestas
	 * da solução passada como parâmetro.
	 * 
	 */
	std::vector<rca::Link> tabu_list (std::vector<SolutionType>&);	
	
	bool update_best_solution (const std::vector<SolutionType>&,
								const ObjectiveType,
								const ObjectiveType);
	
	/**
	 * Este método é utilizado para remover arestas
	 * consideradas tabu.
	 * 
	 * A remoção é realizada consisderando grupos individuais
	 */
	void remove_tabu_links (int g_id) {
		for (auto l : this->m_links_tabu) {
			if (m_network.isRemoved(l)) continue;
			
			m_network.removeEdge (l);
			
			if ( !is_connected (m_network, m_groups[g_id]) ) {
				m_network.undoRemoveEdge (l);
			}
		}
	}
	
	/**
	 * Remove todos as arestas que são tabu na iteração
	 * 
	 * 
	 */
	void remove_tabu_links () {
		m_network.clearRemovedEdges();
		for (auto l : this->m_links_tabu) {
			m_network.removeEdge (l);
		}
		
	}
	
	
	//update tabu list based on 
	/**
	 * Método utilizado para atualizar a lista tabu com
	 * novas aretas.
	 * 
	 * 
	 */
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
	std::vector<int> m_best_cost; //used in cost_tabu_based
	
	//links used as tabu - based on cost
	std::vector<rca::Link> m_links_tabu; 
	double m_links_perc;
	
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