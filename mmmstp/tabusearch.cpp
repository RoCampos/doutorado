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
		
		if (iter%5 == 0) {
			update_tabu ();
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
 		std::cout << std::endl;
		int tt = cost;
		do {
		
			cost = tt;
			c.visitByCost ();
			tt = 0.0;
			for (auto st : sol) {
				tt += (int)st.getCost ();		
			}
			std::cout << cost << " " << tt <<  " " << cg.top() << std::endl;
			
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

// 	rca::sttalgo::cycle_local_search<Container> cls;
// 	
// 	Container cg;
// 	cg.init_congestion_matrix (NODES);
// 	cg.init_handle_matrix (NODES);
// 	
// 	ObjectiveType cost = 0;
// 	for (int i=0; i < m_best_sol.size (); i++) {
// 		cost += update_container (m_best_sol[i], cg, m_groups[i], m_network);
// 	}
// 	
// 	std::vector<SolutionType> sol = m_best_sol;
	std::cout << "best" << std::endl;
 	std::cout << this->m_best << " " << this->m_cost << std::endl;

	
	
// 	std::vector<rca::Link> links;
// 	std::vector<rca::Link> links2;
// 	
// 	std::vector<std::vector<rca::Link>> m_links;
// 	
// 	Container cg;
// 	cg.init_congestion_matrix (NODES);
// 	cg.init_handle_matrix (NODES);
// 	
// 	for (auto st : m_best_sol) {
// 	
// 		std::vector<rca::Link> tree;
// 		edge_t * e = st.get_edge ();
// 		while (e != NULL) {
// 			
// 			if (e->in) {
// 					
// 				rca::Link l (e->x, e->y, 0);
// 				
// 				if (std::find (std::begin(links), std::end(links), l) == 
// 					std::end(links) )
// 				{
// 					l.setValue ((int)this->m_network.getCost (l.getX(), l.getY()));
// 					links.push_back (l);
// 					l.setValue (1);
// 					links2.push_back (l);
// 				} else {
// 				
// 					auto it = std::find (std::begin(links), std::end(links), l);
// 					auto it2 = std::find (std::begin(links2), std::end(links2), l);
// 					int cost = it->getValue ();
// 					cost += this->m_network.getCost (l.getX(), l.getY());
// 					it->setValue (cost);
// 					
// 					int value = it2->getValue ();
// 					it2->setValue (value + 1);
// 					
// 				}
// 				tree.push_back (l);
// 				
// 				if ( !cg.is_used(l) ) {
// 					
// 					l.setValue (GROUPS-1);
// 					cg.push (l);
// 				} else {
// 					cg.push (l,-1);	
// 				}
// 				
// 				std::cerr << l << std::endl;
// 				
// 			}
// 			
// 			e = e->next;
// 		}
// 		std::cerr << std::endl;
// 		m_links.push_back (tree);
// 		
// 	}
// 	
//  	std::sort (std::begin(links),std::end(links), std::greater<rca::Link>());
//  	std::sort (std::begin(links2),std::end(links2), std::greater<rca::Link>());
// 	
//  	for (int i=0; i < links.size (); i++) {
//  		
// 		if (links2[i].getValue () == (m_best-1))
// 		{
// 			std::cout << links[i] << " " <<(int)m_network.getCost (links[i].getX(), links[i].getY());
// 			std::cout << " " << links[i].getValue ();
// 			std::cout << " " << links2[i].getValue () << std::endl;
// 			
// 			
// 			std::vector<int> nodes_x = std::vector<int>(NODES, -1);
// 			std::vector<int> nodes_y = std::vector<int>(NODES, -1);
// 			
// 			rca::Link _old (links[i].getX(), links[i].getY(), links[i].getValue());
// 			//for each tree
// 			for (auto st : m_links) {
// 				rca::sttalgo::make_cut_visitor(st, _old.getX(), _old, nodes_x, NODES);
// 				rca::sttalgo::make_cut_visitor(st, _old.getY(), _old, nodes_y, NODES);				
// 				
// 				for (int x=0; x < nodes_x.size (); x++) {
// 				
// 					for (int w=0; w < nodes_y.size (); w++) {
// 						
// 						if (nodes_x[x] == -1 || nodes_y[w] == -1)
// 							continue;
// 						
// 						double cost = m_network.getCost (x,w);
// 						if (cost > 0) {
// 							
// 							rca::Link X(x, w, 0);
// 
// 							if ( cg.is_used (X) ) {
// 								
// 								int value = ( cg.value (X) + 1) * cost; 
// 								
// 								int old_cost_i = links[i].getValue () - m_network.getCost (links[i].getX(), links[i].getY());
// 								if (value < (old_cost_i) && cg.value (X) > cg.top () ) {
// 								
// 									std::cout << _old << ":";
// 									std::cout << links2[i].getValue() <<":";
// 									std::cout << (int)m_network.getCost (links[i].getX(), links[i].getY())<<":";
// 									std::cout << links[i].getValue () << " -- ";
// 									
// 									std::cout << X << ":";
// 									std::cout << cg.value(X) << ":";
// 									std::cout << value << ":";
// 									std::cout << (int)m_network.getCost (X.getX(), X.getY()) << std::endl;
// 									
// 								}
// 								
// 							} 
// 							
// 						}
// 						
// 					}
// 					
// 				}
// 				
// 				
// 			}
// 		}
// 		
// 	}
//  	
//  	std::cout << _cg.get_heap ().size () << std::endl;
 	
}

template <class SolutionType, class Container, class ObjectiveType>
void rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::build_solution (std::vector<SolutionType>& sol)
{
#ifdef DEBUG1
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

// 	srand (time(NULL));
	
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