#ifndef _TABU_SEARCH_H_
#define _TABU_SEARCH_H_

#include <string>
#include <vector>
#include <bitset>
#include <cmath>

#include "config.h"

#include "sttree.h"
#include "network.h"
#include "edgecontainer.h"
#include "group.h"
#include "reader.h"

#include "steiner_tree_factory.h"
#include "steiner_tree_observer.h"
#include "sttree_local_search.h"

#include "mpp_visitor.h"

#include "localsearch.h"

#include "rcatime.h"

namespace rca{
	
	namespace metaalgo {

template <class SolutionType, class Container, class ObjectiveType>
class TabuSearch {
	
	typedef rca::sttalgo::ChenReplaceVisitor<SolutionType> ChenReplaceVisitor;
	typedef rca::sttalgo::cycle_local_search<Container, SolutionType> OCycleLocalSearch;	

	typedef rca::sttalgo::ShortestPathSteinerTree<Container, SolutionType> sph_t;
	typedef rca::sttalgo::AGMZSteinerTree<Container, SolutionType> agm_t;
	typedef rca::sttalgo::MinmaxSteinerFactory<Container, SolutionType> wsp_t;


	
public:
	TabuSearch (std::string&, std::string, std::string);
	~TabuSearch () {
		this->finish_factory();
	}
	
	inline void set_iterations (int iter) {m_iter = iter;}
	inline void set_budget (ObjectiveType budget) {m_budget = budget;}
	inline void set_has_init (bool value) {m_has_init = value;}
	inline void set_tabu_links_size (double value){m_links_perc = value;}
	inline void set_seed (int seed) {m_seed = seed;}
	inline void set_update_by_cost (int value) {m_update = value;}	
	inline void set_redo_tabu_perc (int value) { m_redo_tabu_perc = value;}
	inline void set_type (std::string & type) {m_type = type;}
	
	void run (std::string);
	
	void build_solution (std::vector<SolutionType>& ,
						ObjectiveType& res, 
						ObjectiveType& cost, 
						Container& cg);
	
	void update_tabu ();
	
	void cost_tabu_based (std::vector<SolutionType>&);
	
	
	
	void zig_zag (std::vector<SolutionType>&, 
					ObjectiveType& res, 
					ObjectiveType& cost,
					Container& cg);
	
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
	void remove_tabu_links (int g_id, std::vector<rca::Link>& links) {
		
		for (auto l : links) {
			if (this->m_network.isRemoved(l)) continue;
			
			this->m_network.removeEdge (l);
			
			if ( !is_connected (this->m_network, m_groups[g_id]) ) {
				this->m_network.undoRemoveEdge (l);
			}
		}
		
	}
	
	/**
	 * Método utilizado para atualizar a lista tabu com
	 * novas aretas.
	 * 
	 * Este método retonar uma nova lista de arestas tabu com
	 * base nas arestas passadas como parâmetro.
	 */
	std::vector<rca::Link> redo_tabu_list (std::vector<rca::Link> & links_cost) {		
		
		std::vector<rca::Link> toReturn;
		for (int i=0; i < links_cost.size ()*this->m_redo_tabu_perc; i++) {
			toReturn.push_back (links_cost[i]);
		}
		return toReturn;
	}

	void start_factory () {
		if (this->m_type.compare("AGM") == 0) {
			this->m_agm_fact = new agm_t();
		}

		if (this->m_type.compare("SPH") == 0) {
			this->m_sph_fact = new sph_t();	
		}

		if (this->m_type.compare("WSP") == 0) {
			this->m_wsp_fact = new wsp_t(this->m_network);	
		}
	}

	void finish_factory () {
		if (this->m_agm_fact != NULL) {
			delete this->m_agm_fact;
			this->m_agm_fact = NULL;
		} 

		if (this->m_sph_fact != NULL) {
			delete this->m_sph_fact;
			this->m_sph_fact = NULL;
		} 

		if (this->m_wsp_fact != NULL) {
			delete this->m_wsp_fact;
			this->m_wsp_fact = NULL;
		}
	}

	void check (std::vector<SolutionType>const & sol, char const * str)
	{
		std::string strc = str;		
		for (auto s : sol) {
			if (s.get_all_edges ().size () == 0) {
				cout << "Invalid after: " << strc << endl;
				cout << "Graph Status:" << m_network.isConnected () << endl;
				exit (1);
			}
		}
	}
	void check (std::vector<SolutionType>const & sol, int strc)
	{		
		for (auto s : sol) {
			if (s.get_all_edges ().size () == 0) {
				cout << "Invalid Line: " << strc << endl;
				cout << "Graph Status:" << m_network.isConnected () << endl;
				exit (1);
			}
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
	
	//seed used to execute
	int m_seed;
	
	//indicates if the firs solutions has been created
	bool m_has_init;
	
	//this variable is used to control the number
	//of iterations without update based on cost
	int m_update;
	
	std::vector<int> m_tabu_list; //control each tree will be build
	std::vector<int> m_best_cost; //used in cost_tabu_based
	
	//links used as tabu - based on cost
	std::vector<rca::Link> m_links_tabu; 
	double m_links_perc;
	
	//used on redo_tabu method
	double m_redo_tabu_perc;
	
	/*--- Problem informations*/
	rca::Network m_network;
	std::vector<rca::Group> m_groups;
	
	//limit of budget
	ObjectiveType m_budget;
	
	//multicast tree factory
	agm_t * m_agm_fact;
	sph_t * m_sph_fact;
	wsp_t * m_wsp_fact;

	//type of factory
	std::string m_type;

	
	
};

	};
};

#endif