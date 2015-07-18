#include "tabusearch.h"

using namespace rca::sttalgo;

template <class V, class X, class Z>
rca::metaalgo::TabuSearch<V, X, Z>::TabuSearch (std::string& file) 
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
	m_best_cost = std::vector<int>( this->m_groups.size(), this->m_cost );
	
}

template <class V, class X, class Z>
void rca::metaalgo::TabuSearch<V, X, Z>::run () 
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	int iter = 0;
	
	int GROUPS = this->m_groups.size ();
	int NODES = this->m_network.getNumberNodes ();
	std::vector<SolutionType> sol1( GROUPS );
	
	int c=0, r=0;
	build_solution (sol1, r, c);
	std::cout << r << " " << c << std::endl;
	
	this->m_best_sol = sol1;
	
	update_tabu (); //for groups	
	
	int count_iter = 0;
	
	Container _cg;
	do {
		
		count_iter++;
		if (count_iter >= 5) {
			update_tabu ();
			count_iter = 0;
		} 
		
		std::vector<SolutionType> sol ( GROUPS );
		int x, y;
		build_solution (sol, x, y);
		
		//-------------------------------------------------
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
		//-------------------------------------------------
		
		//updating a solution
 		this->update_best_solution (sol, cg.top(), cost);
 		std::cout << cg.top () << " " << cost << std::endl;
		
	} while (iter++ < this->m_iter);

 	std::cout << "best" << std::endl;
  	std::cout << this->m_best << " " << this->m_cost << std::endl;
// 	
// 	Container cg;
// 	cg.init_congestion_matrix (NODES);
// 	cg.init_handle_matrix (NODES);
// 		
// 	ObjectiveType cost = 0;
// 	for (int i=0; i < this->m_best_sol.size (); i++) {
// 		cost += update_container (this->m_best_sol[i], cg, m_groups[i], m_network);
// 	}
// 	rca::sttalgo::print_solutions_stats (this->m_best_sol, cg, m_network);

}

template <class V, class X, class Z>
void rca::metaalgo::TabuSearch<V, X, Z>::build_solution (std::vector<V>& sol, 
														 Z& res_sol, Z& cost_sol)
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

		this->remove_tabu_links (i);
		
		
		ob.set_steiner_tree (tree, NODES);
		int r = rand () % 10 + 1;
		
 		
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
	
	res_sol = cg.top ();
	cost_sol = cost;
	
 
}

template <class V, class X, class Z>
void 
rca::metaalgo::TabuSearch<V, X, Z>::update_best_solution 
								(std::vector<V>& sol,
								const Z res,
								const Z cost)
{
	if (res > this->m_best && cost < this->m_budget) {
		
		this->m_best = res;
		this->m_cost = cost;
		this->m_best_sol = sol;
		
		std::vector<rca::Link> 
		links_cost = this->tabu_list (this->m_best_sol);
		
		m_links_tabu.clear ();
		for (int i=0; i < links_cost.size ()*0.1; i++) {
			m_links_tabu.push_back (links_cost[i]);
		}
		
	} else if ( (res == this->m_best) 
		&& (cost < this->m_cost) && cost < this->m_budget ) {
		
		this->m_best = res;
		this->m_cost = cost;
		this->m_best_sol = sol;
		
		std::vector<rca::Link> 
		links_cost = this->tabu_list (this->m_best_sol);
		
		m_links_tabu.clear ();
		for (int i=0; i < links_cost.size ()*0.1; i++) {
			m_links_tabu.push_back (links_cost[i]);
		}
	}								
}

template <class V, class X, class Z>
void rca::metaalgo::TabuSearch<V, X, Z>::update_tabu ()
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

template <class V, class X, class Z>
void rca::metaalgo::TabuSearch<V, X, Z>::cost_tabu_based(std::vector<V>& sol)
{

	int tree_id = 0;
	for (auto st: sol) {
	
		if (st.getCost () < m_best_cost[tree_id]) {
		
			m_best_cost[tree_id] = st.getCost();
			m_tabu_list[tree_id] = 1;			
		} else {
			m_tabu_list[tree_id] = 0;
		}
		
		tree_id++;
	}
	
}

template <class V, class X, class Z>
std::vector<rca::Link> 
rca::metaalgo::TabuSearch<V, X, Z>::tabu_list (std::vector<V>& trees)
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

template <class V, class X, class Z>
Z rca::metaalgo::TabuSearch<V, X, Z>::update_container (SolutionType& tree, 
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

template <class V, class X, class Z>
Z 
rca::metaalgo::TabuSearch<V, X, Z>::improvement (std::vector<V>& sol, int& res)
{
	int GROUPS = this->m_groups.size ();
	int NODES = this->m_network.getNumberNodes ();
	
	Container cg;
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);

 	ObjectiveType cost = 0;
 	for (int i=0; i < sol.size (); i++) {
 		cost += update_container (sol[i], cg, m_groups[i], m_network);
 	}
 	
 	auto links = this->tabu_list (sol);
	int count = 0;
	int top = cg.top ()-1;
	for (rca::Link & l : links) {
		if (l.getValue() >= top) {
			for (int g = 0; g < GROUPS; g++){
				std::vector<rca::Link> update = 
				get_available_links<SolutionType,Container,rca::Network> 
				(sol[g], cg, m_network, m_groups[g], l);
				
				if (update.size () == 0) continue;
				else {
					
					rca::Link ll = update[0];
					int cost1 = m_network.getCost (l.getX(), l.getY());
					int cost2 = m_network.getCost (ll.getX(), ll.getY());
					
					int band_ll =  m_network.getBand (ll.getX(), ll.getY());
					int etop = (cg.is_used (ll) ? cg.value(l) : band_ll - m_groups[g].getTrequest());
					
					if (cost2 < cost1 && etop > cg.top()) {
						
						rca::Link ll = update[0];
 						std::cout << l << ":"<<cost1<<":"<<cg.value(l)<< "<-->";
 						std::cout << ll << ":"<<cost2<<":"<<etop<<std::endl;
						count+=  (cost1-cost2);
						rca::sttalgo::replace_edge (sol[g], 
 											l, 
 											ll, 
 											m_network);
						
						cg.push(l,+1);
						cg.push(ll,-1);
 						std::cout <<"\t"<< l << ":"<<cost1<<":"<<cg.value(l)<< "<-->";
 						std::cout << ll << ":"<<cost2<<":"<<etop<<std::endl;
						
						break;
					}
				}
			}
			
		}
		
		if (cost-count < this->m_budget) 
			break;
		
	}
	
	Container cg1;
	cg1.init_congestion_matrix (NODES);
	cg1.init_handle_matrix (NODES);
	cost = 0;
 	for (int i=0; i < sol.size (); i++) {
 		cost += update_container (sol[i], cg1, m_groups[i], m_network);
 	}
 	
 	res = cg1.top ();	
	return cost;
	
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