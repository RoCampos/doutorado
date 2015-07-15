#include "tabusearch.h"

template <class SolutionType, class Container, class ObjectiveType>
rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::TabuSearch (std::string& file) 
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	m_factory = NULL;
	
	std::vector<std::shared_ptr<rca::Group>> g;
	
	MultipleMulticastReader reader(file);
	reader.configure_unit_values (&this->m_network, g);
	for (std::shared_ptr<rca::Group> i : g) {
		this->m_groups.push_back (*i);
	}
	
	this->m_best = std::numeric_limits<ObjectiveType>::min ();
	this->m_cost = std::numeric_limits<ObjectiveType>::max ();
	
	
	m_tabu_list = std::vector<int>( this->m_groups.size(), 0 );
	
}

template <class SolutionType, class Container, class ObjectiveType>
void rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::run () 
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	int iter = 0;
	
	int GROUPS = this->m_groups.size ();
	int NODES = this->m_network.getNumberNodes ();
	std::vector<SolutionType> sol1( GROUPS );
	build_solution (sol1);
	
	
	Container _cg;
	do {
		
		if (iter%10 == 0) {
			update_tabu ();
			
			
			//outro tabu
			std::vector<rca::Link> 
				links_cost = tabu_list (m_best_sol);
			
			links_tabu.clear ();
			for (int i=0; i < links_cost.size ()*0.3; i++) {
				links_tabu.push_back (links_cost[i]);
			}
				
				
		}
		
		std::vector<SolutionType> sol ( GROUPS );
		build_solution (sol);
		
		rca::sttalgo::cycle_local_search<Container> cls;
	
		Container cg;
		cg.init_congestion_matrix (NODES);
		cg.init_handle_matrix (NODES);
		
		ObjectiveType cost = 0;
		for (int i=0; i < sol.size (); i++) {
			cost += update_container (sol[i], cg, m_groups[i], m_network);
		}
		cls.local_search (sol, m_network, m_groups, cg, cost);
		
		rca::sttalgo::ChenReplaceVisitor c(&sol);
		c.setNetwork (&m_network);
		c.setMulticastGroups (m_groups);
		c.setEdgeContainer (cg);

		cost = 0;
 		for (auto st : sol) {
 			cost += (int)st.getCost ();
 		}		

		int tt = cost;
		do {
		
			cost = tt;
			c.visitByCost ();
			tt = 0.0;
			for (auto st : sol) {
				tt += (int)st.getCost ();		
			}
			
		} while (tt < cost);
		
		if (cg.top () > this->m_best) {
		
			this->m_best = cg.top ();
			this->m_cost = cost;
			this->m_best_sol = sol;
			
			_cg = cg;
			
		} else if ( (cg.top() == this->m_best) && (cost < this->m_cost) ) {
			
			this->m_best = cg.top ();
			this->m_cost = cost;
			this->m_best_sol = sol;
			
			_cg = cg;
		}
		
		
	} while (iter++ < this->m_iter);

	std::cout << "best" << std::endl;
 	std::cout << this->m_best << " " << this->m_cost << std::endl;
		
//   	rca::sttalgo::print_solution<STTree> (this->m_best_sol); 

}

template <class SolutionType, class Container, class ObjectiveType>
void rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::build_solution (std::vector<SolutionType>& sol)
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	if (this->m_factory == NULL)
		this->m_factory = new rca::sttalgo::ShortestPathSteinerTree<Container>();
	
	int NODES = this->m_network.getNumberNodes();
	int GROUPS= this->m_groups.size ();

	Container cg;
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);
	
	rca::sttalgo::SteinerTreeObserver<Container> ob;
	ob.set_container (cg);
	
	std::vector<int> index(GROUPS, 0);
	iota (index.begin(), index.end(), 0);
	
	std::random_shuffle (index.begin(), index.end());
	
	ObjectiveType cost = 0;
	for (int j=0; j < GROUPS; j++) {
	
		int i = index[j];
		
 		if (m_tabu_list[j] == 1) {
 			
			sol[i] = m_best_sol[i];			
			cost += update_container (m_best_sol[i], cg, m_groups[i], m_network);
			
 			continue;
 		}
		
		SolutionType tree = SolutionType(NODES, 
										 m_groups[i].getSource(), 
										 m_groups[i].getMembers());
		
		ob.set_steiner_tree (tree, NODES);
		rca::sttalgo::remove_top_edges<CongestionHandle> (cg, 
													m_network, 
													m_groups[i], 0);
		
		for (auto l : links_tabu) {
			
			if (m_network.isRemoved(l)) continue;
			m_network.removeEdge (l);
			if ( !is_connected (m_network, m_groups[i]) ) {
				m_network.undoRemoveEdge (l);
			}
		}
		
		//building the tree
		this->m_factory->build (ob, m_network, m_groups[i], cg);
		ob.prune (1,GROUPS);
		
		//updating the cost
		cost += ob.get_steiner_tree ().getCost ();
		
 		m_network.clearRemovedEdges ();
		
		sol[i] = ob.get_steiner_tree ();
		
	}
	
// 	if (cg.top () > this->m_best) {
// 		
// 		this->m_best = cg.top ();
// 		this->m_cost = cost;
// 		this->m_best_sol = sol;
// 		
// 	} else if ( (cg.top() == this->m_best) 
// 		&& (cost < this->m_cost)  ) {
// 		
// 		this->m_best = cg.top ();
// 		this->m_cost = cost;
// 		this->m_best_sol = sol;
// 		
// 	}
 
}

template <class SolutionType, class Container, class ObjectiveType>
void rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::update_tabu ()
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	int value = rand () % ( 2^m_groups.size () );
	
 	std::string str = std::bitset< 32 >( value ).to_string();
	
	for (int i=0; i < m_tabu_list.size (); i++) {
		if (str[i] == '1') {
			m_tabu_list[i] = 1;
		}
	}
		
}
template <class SolutionType, class Container, class ObjectiveType>
std::vector<rca::Link> 
rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::tabu_list (std::vector<SolutionType>& trees)
{
	std::vector<rca::Link> links_cost;
	//calculating the cost accumalated
	for (auto st : trees) {
		
		edge_t * e = st.get_edge ();
		while (e!= NULL) {
			
			if (e->in) {
				
				rca::Link l (e->x, e->y, 0);
				
				auto it = std::find (std::begin(links_cost), std::end(links_cost),l);
				if (it == std::end(links_cost)) {
					//tem que mudar para o caso de tk aleatório
					
					int cost = this->m_network.getCost (l.getX(), l.getY());
					
					l.setValue (cost);
					
					links_cost.push_back (l);
				} else {
					
					int cost = it->getValue () + this->m_network.getCost (l.getX(), 
																		  l.getY());								
					it->setValue (cost);
					
				}
			}
			
			e = e->next;
		}
		
	}
	std::sort (std::begin(links_cost), 
			   std::end(links_cost), 
			   std::greater<rca::Link>());
	
	return links_cost;
}

template <class SolutionType, class Container, class ObjectiveType>
ObjectiveType rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::update_container (SolutionType& tree, 
																			  Container& cg, 
																			  rca::Group& g, 
																			  rca::Network& net)
{
	ObjectiveType tree_cost = 0;
	edge_t * e = tree.get_edge ();
	while (e != NULL) {
	
		if (e->in) {
		
			rca::Link l(e->x, e->y, 0);
			
			if (cg.is_used (l)) {
			
				int value = cg.value (l);
				value -= g.getTrequest ();				
				
				cg.erase (l);
				l.setValue (value);
				cg.push (l);
				
			} else {
				int band = net.getBand(l.getX(), l.getY());				
				l.setValue (band-g.getTrequest ());
				
				cg.push (l);
			}
			
			tree_cost += net.getCost (l.getX(), l.getY());
			
		}
		
		e = e->next;
	}
	return tree_cost;
}

typedef rca::EdgeContainer<rca::Comparator, rca::HCell> CongestionHandle;

int main (int argv, char**argc) {

	srand (time(NULL));
	
// 	int r = time(NULL);
// 	std::cout << r << std::endl;
	
	using namespace rca;
	using namespace rca::metaalgo;
	
	std::string file(argc[1]);
	int iterations = atoi(argc[2]);
	int budget = atoi (argc[3]);
	
	TabuSearch<STTree, CongestionHandle, int> tabueSearch (file);
	tabueSearch.set_iterations ( iterations );
	tabueSearch.set_budget ( budget );
	
	tabueSearch.run ();
	
	return 0;
}