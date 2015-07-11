#include "tabusearch.h"

template <class SolutionType, class Container, class ObjectiveType>
rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::TabuSearch (std::string& file) 
{
#ifdef DEBUG
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
#ifdef DEBUG
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	int iter = 0;
	
	int GROUPS = this->m_groups.size ();
	int NODES = this->m_network.getNumberNodes ();
	std::vector<SolutionType> sol1( GROUPS );
	build_solution (sol1);
	
	do {
		
		if (iter%5 == 0) {
			update_tabu ();
		}
		std::vector<SolutionType> sol ( GROUPS );
		build_solution (sol);
		
	} while (iter++ < this->m_iter);

	rca::sttalgo::cycle_local_search<Container> cls;
	
	Container cg;
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);
	
	
	ObjectiveType cost = 0;
	for (int i=0; i < m_best_sol.size (); i++) {
		cost += update_container (m_best_sol[i], cg, m_groups[i], m_network);
	}
	
	std::vector<SolutionType> sol = m_best_sol;
	std::cout << this->m_best << " " << this->m_cost << std::endl;
	cls.local_search (sol, m_network, m_groups, cg, cost);
	std::cout << cg.top () << " " << cost << std::endl;
	
	
	
}

template <class SolutionType, class Container, class ObjectiveType>
void rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::build_solution (std::vector<SolutionType>& sol)
{
#ifdef DEBUG
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	if (this->m_factory == NULL)
		this->m_factory = new rca::sttalgo::WildestSteinerTree<Container>();
	
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
		
		//building the tree
		this->m_factory->build (ob, m_network, m_groups[i], cg);
		ob.prune (1,GROUPS);
		
		//updating the cost
		cost += ob.get_steiner_tree ().getCost ();
		
 		m_network.clearRemovedEdges ();
		
		sol[i] = ob.get_steiner_tree ();
		
	}
	
	if (cg.top () > this->m_best) {
		
		this->m_best = cg.top ();
		this->m_cost = cost;
		this->m_best_sol = sol;
		
	} else if ( (cg.top() == this->m_best) && (cost < this->m_cost) ) {
		
		this->m_best = cg.top ();
		this->m_cost = cost;
		this->m_best_sol = sol;
		
	}
	
// 	std::cout << cg.top () << " " << cost << std::endl;
	 
}

template <class SolutionType, class Container, class ObjectiveType>
void rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::update_tabu ()
{
#ifdef DEBUG
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	int value = rand () % ( 2^m_groups.size () );	
 	std::string str = std::bitset< 8 >( value ).to_string();
	
	for (int i=0; i < m_tabu_list.size(); i++) {
		if (str[i] == '1') {
			m_tabu_list[i] = 1;
		}
	}
	
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
	
	int r = time(NULL);
	std::cout << r << std::endl;
	
	using namespace rca;
	using namespace rca::metaalgo;
	
	std::string file(argc[1]);
	int iterations = atoi(argc[2]);
	
	TabuSearch<STTree, CongestionHandle, int> tabueSearch (file);
	tabueSearch.set_iterations ( iterations );
	
	tabueSearch.run ();
	
	return 0;
}