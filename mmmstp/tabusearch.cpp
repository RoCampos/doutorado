#include "tabusearch.h"

bool has_improv = false;

using namespace rca::sttalgo;

template <class SolutionType, class Container, class ObjectiveType>
rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::TabuSearch (std::string& file) 
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	m_factory = NULL;
	
	std::vector<std::shared_ptr<rca::Group>> g;
	
	rca::reader::MultipleMulticastReader reader(file);
	reader.configure_real_values (&this->m_network, g);
	for (std::shared_ptr<rca::Group> i : g) {
		this->m_groups.push_back (*i);
	}
	
	this->m_best = std::numeric_limits<ObjectiveType>::min ();
	this->m_cost = std::numeric_limits<ObjectiveType>::max ();
	
	
	m_tabu_list = std::vector<int>( this->m_groups.size(), 0 );
	m_best_cost = std::vector<int>( this->m_groups.size(), this->m_cost );
	
	m_has_init = false;
	
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
	
	rca::elapsed_time _time_;
	_time_.started ();
	
	int c=0, r=0;
	
	Container _cg;
	_cg.init_congestion_matrix (NODES);
	_cg.init_handle_matrix (NODES);
	
	build_solution (sol1, r, c, _cg);
	
	this->zig_zag (sol1, r, c, _cg);
	
	this->m_best_sol = sol1;
	this->m_best = r;
	this->m_cost = c;
	
	update_tabu (); //for groups	
	
	int count_iter = 0;
	int best_iteration = 0;
	
	do {
		
		this->m_has_init = true;
		
		count_iter++;
 		if (count_iter >= this->m_update) {
 			cost_tabu_based (this->m_best_sol);
 			count_iter = 0;	
 		} else {
			update_tabu ();
 		}
		
		std::vector<SolutionType> sol ( GROUPS );
		Container cg;
		cg.init_congestion_matrix (NODES);
		cg.init_handle_matrix (NODES);
		
		int x, y;
		build_solution (sol, x, y, cg);
		
   		int rr = 0, cc = 0;
   		this->zig_zag (sol, rr, cc, cg);
		
		//updating a solution
 		if (this->update_best_solution (sol, rr, cc) ) 
			best_iteration = iter;
				
	} while (iter++ < this->m_iter);

	_time_.finished ();

  	std::cout << this->m_best << " ";
	std::cout << this->m_cost << " ";
	std::cout << _time_.get_elapsed() << " ";
	std::cout << m_seed << " ";
	std::cout << best_iteration << std::endl;
	
 	rca::sttalgo::print_solution2<SolutionType> (this->m_best_sol);

 	// for (auto & st : m_best_sol) {
 	// 	st.print ();
 	// }

}


template <class SolutionType, class Container, class ObjectiveType>
void rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::build_solution (std::vector<SolutionType>& sol, 
														 ObjectiveType& res_sol, ObjectiveType& cost_sol,
														Container &cg)
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	

	
	if (this->m_factory == NULL)
		// this->m_factory = new rca::sttalgo::ShortestPathSteinerTree<Container, SolutionType>();
		this->m_factory = new rca::sttalgo::AGMZSteinerTree<Container, SolutionType>();

	this->m_factory->create_list (this->m_network);
	
	int NODES = this->m_network.getNumberNodes();
	int GROUPS= this->m_groups.size ();

	rca::sttalgo::SteinerTreeObserver<CongestionHandle, SolutionType> ob;
	ob.set_container (cg);
	ob.set_network (m_network);
	
	std::vector<int> index(GROUPS, 0);
	iota (index.begin(), index.end(), 0);
	
	std::random_shuffle (index.begin(), index.end());
	
	ObjectiveType cost = 0;
	for (int j=0; j < GROUPS; j++) {
	
		int i = index[j];

		int trequest = m_groups[i].getTrequest ();
		
 		if (m_tabu_list[j] == 1) {
 			
			sol[i] = m_best_sol[i];			
			cost += update_container (m_best_sol[i], cg, m_groups[i], m_network);
			
 			continue;
 		}
		
		//cria árvore
		SolutionType tree = SolutionType(NODES, 
										 m_groups[i].getSource(), 
										 m_groups[i].getMembers());

		//se a primeira solução tiver sido criada
		if (m_has_init) {
			
			//cria lista com base nos tabus 
			std::vector<rca::Link> 
				links = this->redo_tabu_list (m_links_tabu);
			
			//remove tabus com com base na melhor solução
			this->remove_tabu_links (i, links);
			
		} else {
		
			//remove tabus com base na solução atual
			auto links = tabu_list (sol);
			this->redo_tabu_list (links);
			this->remove_tabu_links (i, links);
		}
		
		
		ob.set_steiner_tree (tree, NODES);
		
		//se a primeira solução tiver sido criada, 
		//evita congestionamento de arestas
		if (m_has_init) {
			// rca::sttalgo::remove_top_edges<CongestionHandle> (cg, 
 		// 											m_network, 
 		// 											m_groups[i], 0);
		}
		
		//building the tree
		this->m_factory->build (ob, m_network, m_groups[i], cg);
		ob.prune (trequest, GROUPS);
		
		//updating the cost
		cost += ob.get_steiner_tree ().get_cost ();
		
 		m_network.clearRemovedEdges ();
		
		sol[i] = ob.get_steiner_tree ();

		this->m_factory->update_usage (m_groups[i], m_network, ob.get_steiner_tree ());
		
	}

	res_sol = cg.top ();
	cost_sol = cost;	
}

template <class SolutionType, class Container, class ObjectiveType>
bool 
rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::update_best_solution 
								(const std::vector<SolutionType>& sol,
								const ObjectiveType res,
								const ObjectiveType cost)
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	
	if (res > this->m_best && cost < this->m_budget) {
		
		this->m_best = res;
		this->m_cost = cost;
		
		this->m_best_sol.clear ();
		for (auto st : sol) {
			this->m_best_sol.push_back (st);
		}
		
		std::vector<rca::Link> 
		links_cost = this->tabu_list (this->m_best_sol);
		
		m_links_tabu.clear ();
		for (int i=0; i < links_cost.size () * m_links_perc ; i++) {
			m_links_tabu.push_back (links_cost[i]);
		}
		
		return true;
		
	} else if ( (res == this->m_best) 
		&& (cost < this->m_cost) && cost < this->m_budget ) {
		
		this->m_best = res;
		this->m_cost = cost;
		this->m_best_sol.clear ();
		for (auto st : sol) {
			this->m_best_sol.push_back (st);
		}
		
		std::vector<rca::Link> 
		links_cost = this->tabu_list (this->m_best_sol);
		
		m_links_tabu.clear ();
		
		for (int i=0; i < links_cost.size () * m_links_perc; i++) {
			m_links_tabu.push_back (links_cost[i]);
		}
		
		return true;
		
	} 
	
	return false;
}

template <class SolutionType, class Container, class ObjectiveType>
void rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::update_tabu ()
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	unsigned long int uli = pow( 2.0, (double)m_groups.size () );
	unsigned long int value = rand () % uli;
	
 	std::bitset<32> str = std::bitset< 32 >( value );
	
	for (int i=0; i < m_tabu_list.size (); i++) {
		if (str[i] == 1) {
			m_tabu_list[i] = 1;
		} else {
			m_tabu_list[i] = 0;
		}
	}
		
}

template <class SolutionType, class Container, class ObjectiveType>
void rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::cost_tabu_based(std::vector<SolutionType>& sol)
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	int tree_id = 0;
	for (auto st: sol) {
	
		if (st.get_cost () < m_best_cost[tree_id]) {			
			m_best_cost[tree_id] = st.get_cost();
			m_tabu_list[tree_id] = 1;			
		} else {
			m_tabu_list[tree_id] = 0;
		}
		
		tree_id++;
	}

}

template <class SolutionType, class Container, class ObjectiveType>
std::vector<rca::Link> 
rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::tabu_list (std::vector<SolutionType>& trees)
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
	
	std::cout << trees.size () << std::endl;
	
#endif
	std::vector<rca::Link> links_cost;
	//calculating the cost accumalated
	for (auto st : trees) {

		for (auto & edge : st.get_all_edges ()) {
			rca::Link l (edge.first, edge.second, 0);
			auto it = std::find (std::begin(links_cost), std::end(links_cost),l);
			
			if (it == std::end(links_cost)) {
				//tem que mudar para o caso de tk aleatório					
				int cost = (int)this->m_network.getCost (l.getX(), l.getY());				
				l.setValue (cost);					
				links_cost.push_back (l);
			} 
		}
		
	}


	std::sort (std::begin(links_cost), 
			   std::end(links_cost), 
			   std::greater<rca::Link>());

	return links_cost;
}

template <class SolutionType, class Container, class ObjectiveType>
ObjectiveType 
rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::update_container (
														SolutionType& tree, 
														Container& cg, 
														rca::Group& g, 
														rca::Network& net)
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	ObjectiveType tree_cost = 0;
	
	for (auto & edge : tree.get_all_edges ()) {

		rca::Link l (edge.first, edge.second, 0);
		
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

		tree_cost += (int)net.getCost (l.getX(), l.getY());
	}

	return tree_cost;
}

template <class SolutionType, class Container, class ObjectiveType>
void
rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::improvement (std::vector<SolutionType>& sol, 
												 int& res, int &cos)
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	int NODES = m_network.getNumberNodes ();
// 	int GROUPS = m_groups.size();
	Container cg;
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);
	
	ObjectiveType cost = 0;
	for (int i=0; i < sol.size (); i++) {
		cost += update_container (sol[i], cg, m_groups[i], m_network);
	}
	
	ChenReplaceVisitor c(&sol);
	c.setNetwork (&m_network);
	c.setMulticastGroups (m_groups);
	c.setEdgeContainer (cg);
	
	// rca::sttalgo::cycle_local_search<Container> cls;
	
	bool imp_cost = false, imp_res = false;
	int orig_cost = cost;
	std::vector<SolutionType> copy;
	
	do {
		imp_cost = false; imp_res = false;
		
		//improving by cost
		int tt = cost;
		
		do {
			
			cost = tt;
			c.visitByCost ();
			tt = c.get_solution_cost ();
				
		} while (tt < cost);
		
		cost = tt;
		
		// cls.local_search (sol, m_network, m_groups, cg, cost);
		
		if (cost < orig_cost) {
			orig_cost = cost;
			imp_cost = true;
		}
		
		m_network.clearRemovedEdges ();
		
// 		auto links = tabu_list (sol);
		auto links = c.get_replaced ();
		redo_tabu_list (links);
		for (int i=0;i < m_groups.size (); i++)
			remove_tabu_links (i);
		
		int ress = cg.top ();
		
		copy.clear ();
		for (auto st : sol) copy.push_back (st);
		
		c.visit ();
		
		int ccc = c.get_solution_cost ();
		std::cout << res << " " << ccc << std::endl;
		
		m_network.clearRemovedEdges ();
		
		links = c.get_replaced ();
		redo_tabu_list (links);
		for (int i=0;i < m_groups.size (); i++)
			remove_tabu_links (i);
		
		if (cg.top () > ress) {
			
			res = cg.top ();
			imp_res = true;
			
		} else {

			res = ress;
			
			break;
		}
		
	} while (imp_cost || imp_res);
	
	sol.clear ();
	cost = 0;
	for (auto st : copy) {
		cost += (int)st.get_cost ();
		sol.push_back (st);
	}
	
	cos = cost;
	
	this->m_network.clearRemovedEdges ();
}

template <class SolutionType, class Container, class ObjectiveType>
void rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::zig_zag (std::vector<SolutionType>& sol, 
												  ObjectiveType& res, ObjectiveType& cos,
												  Container& cg)
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	this->m_network.clearRemovedEdges();
	
	//creating the container to store the edges usage
	int GROUPS = m_groups.size();
	
	//object to perform cycle local search
	// OCycleLocalSearch cls;
	
	//creatig ChenReplaceVisitor to perform cost and residual refinement
	ChenReplaceVisitor c(&sol);
	c.setNetwork (&m_network);
	c.setMulticastGroups (m_groups);
	c.setEdgeContainer (cg);

#ifdef DEBUG1
	std::cout << "\t visit by cost\n";
#endif	
	ObjectiveType cost = 0;

	for (auto & st: sol){
		cost += (int)st.get_cost ();
	}

	//old local search
	// int tt = cost;
	// do {		
	// 	cost = tt;		
	// 	c.visitByCost ();
	// 	tt = c.get_solution_cost ();
	// } while (tt < cost);
	
	//performing cost refinement
	rca::sttalgo::LocalSearch ls(m_network, m_groups, cg);	
	int ocost = cost;
	int z = cg.top ();
	do {				
		ocost = cost;
		ls.apply (sol, cost, z);	
	} while (cost < ocost);	
	
	
#ifdef DEBUG1
	std::cout << "\t cycle_local_search\n";
#endif	

	// tt = cost;
	// do {		
	// 	cost = tt;
	// 	c.visitByCost ();
	// 	tt = c.get_solution_cost ();
	// } while (tt < cost);


	//performing cycle local search
	CycleLocalSearch cls(m_network, m_groups, cg);	
	z = cg.top ();	
	ocost = cost;
	do {
		ocost = cost;
		cls.apply (sol, cost, z);	 	
	} while (cost < ocost);
	
	//cleaning the network
	this->m_network.clearRemovedEdges();
	
	//builing tabu list based on the most expensive edges	
	//from solution sol
 	auto tabu = this->tabu_list (sol);	
	//getting the tabus 
 	auto links = this->redo_tabu_list (tabu);
	//removing the tabus
 	for (int i=0;i < GROUPS; i++) {
 		this->remove_tabu_links (i, links);
 	}
		
#ifdef DEBUG1
	std::cout << "\t visit\n";
#endif	
	//applying ChenReplaceVisitor by cost
	if(this->m_has_init) {
		c.visit ();
	}

	//updating cost of the solution after apply residual refinement
	cost = 0;
	for (auto & st: sol){
		cost += (int)st.get_cost ();
	}
	

#ifdef DEBUG1
	std::cout << "\t visit by cost\n";
#endif	

	// tt = cost;
	// do {		
	// 	cost = tt;
	// 	c.visitByCost ();
	// 	tt = c.get_solution_cost ();
	// } while (tt < cost);


 	// applying cost refinement based on
 	rca::sttalgo::LocalSearch lss(m_network, m_groups, cg); 	
	ocost = cost;
	z = cg.top ();
	do {				
		ocost = cost;
		lss.apply (sol, cost, z);		
	} while (cost < ocost);

#ifdef DEBUG1
	std::cout << "\t cls\n";
#endif	

	// if (this->m_has_init)
	//  	cls.local_search (sol, m_network, m_groups, cg, cost);



	//applying cycle local search after refine by cost
	CycleLocalSearch cls2(m_network, m_groups, cg);	
	if (this->m_has_init) {	 	
	 	z = cg.top ();
	 	ocost = cost;
	 	do {
			ocost = cost;
			cls2.apply (sol, cost, z);	 	
	 	} while (cost < ocost);
	}	


	res = cg.top ();
	cos = cost;
	
	//cleang the network
	this->m_network.clearRemovedEdges();
}

template <class SolutionType, class Container, class ObjectiveType>
void rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::zig (std::vector<SolutionType>& sol, 
												ObjectiveType& res, 
												ObjectiveType& cos)
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	typedef std::tuple<int,rca::Link, rca::Link> TupleRemove;
	
	
	int NODES = this->m_network.getNumberNodes ();
	int GROUPS = this->m_groups.size ();
	
	Container * cg = new Container;
	cg->init_congestion_matrix (NODES);
	cg->init_handle_matrix (NODES);
	
	ObjectiveType cost = 0;
	for (int i=0; i < sol.size (); i++) {
		cost += update_container (sol[i],*cg, m_groups[i], m_network);
	}

	//getting all links that could be replaced and for what link
	std::vector<rca::Link> all_links;
	const auto & heap = cg->get_heap ();
	auto begin = heap.ordered_begin ();
	auto end = heap.ordered_end ();
	
	for (; begin != end; begin++) {
	
		rca::Link l = *begin;
		int COST = m_network.getCost (l.getX(), l.getY());
		l.setValue (COST);
		all_links.push_back (l);
		
	}
	
	std::sort (std::begin(all_links), 
			   std::end(all_links), 
			   std::greater<rca::Link>());
	
	std::vector<rca::Link> replaced;
 	int ccc = 0;
 	
	std::vector<TupleRemove> to_remove;
	
 	for (auto link : all_links) {
	
// 		if (cg->value (link) == cg->top()) continue;
		
// 		std::cerr << link.getValue () << " " << cg->value (link) << std::endl;
		
		for (int g = 0; g < GROUPS; g++){
		
			std::vector<rca::Link> update = 
			get_available_links<SolutionType,Container,rca::Network> 
				(sol[g], *cg, m_network, m_groups[g], link);	
			
			if (update.size () > 0) {
			
				bool pushed = false;
				for (auto & link_rep : update) {
 					
					int value = cg->is_used(link_rep)? cg->value(link_rep): m_groups.size();
					int _cost = (int)m_network.getCost (link_rep.getX(), link_rep.getY());
					
					if (value >= cg->top ()) {
						
						if (_cost < link.getValue() && link != link_rep) {
// 							std::cerr << "  " << link_rep;
// 							std::cerr << "| Res (" << value<< ")";
// 							std::cerr << "Cost (" << _cost << ")";
// 							std::cerr << std::endl;
							
							auto bb = replaced.begin();
							auto ee = replaced.end();
							if (std::find(bb, ee, link_rep) == ee) {
								replaced.push_back (link_rep);
								ccc += link.getValue ()-_cost;
								
								link_rep.setValue (value);
								
								cg->push (link_rep, -1);
								cg->push (link, 1);
								
								TupleRemove t (g, link, link_rep);
								to_remove.push_back (t);
								
								pushed = true;
								break;
								
							}
						}					
					}
				}//end edge for loop
				
				if (pushed) break;
				
			}//test if there is some edge to be used
		
		}
		
	}
	
	for (TupleRemove & t : to_remove) {
// 		std::cout << std::get<0> (t);
// 		std::cout << " " << std::get<1> (t);
// 		std::cout << " " << std::get<2> (t) << std::endl;
		
		int tree = std::get<0> (t);
		rca::Link l = std::get<1> (t);
		rca::Link ll = std::get<2> (t);
		
		rca::sttalgo::replace_edge (sol[tree], l, ll, m_network);
	}
	
#ifdef DEBUG1
	std::cout << "Expected value: ";	
 	std::cout << cg->top () << " "<< (cost-ccc) << std::endl;
#endif
	
	delete cg;
	
	cg = new Container;
	cg->init_congestion_matrix (NODES);
	cg->init_handle_matrix (NODES);
	
	cost = 0;
	for (int i=0; i < sol.size (); i++) {
		cost += update_container (sol[i],*cg, m_groups[i], m_network);
	}
	
#ifdef DEBUG1
	std::cout << "Real Value: ";
	std::cout << cg->top () << " " << cost << std::endl;
#endif
	
	res = cg->top ();
	cos = cost;
}

typedef rca::EdgeContainer<rca::Comparator, rca::HCell> CongestionHandle;

int main (int argv, char**argc) {

	int r = time(NULL);

  	srand ( r );
	
	using namespace rca;
	using namespace rca::metaalgo;
	
	std::string file(argc[1]);
	int iterations = atoi(argc[2]);
	
	int budget = atoi (argc[3]);
	
	if (budget == 0) budget = INT_MAX;
	
	double list_perc = atof (argc[4]);
	
	double redo_tabu_perc = atof (argc[5]);
	
	int update = atoi (argc[6]);
	
	
	
	TabuSearch<steiner, CongestionHandle, int> tabueSearch (file);
	tabueSearch.set_iterations ( iterations );
	tabueSearch.set_budget ( budget );
	tabueSearch.set_tabu_links_size (list_perc);
	tabueSearch.set_seed ( r );
	tabueSearch.set_update_by_cost (update);
	tabueSearch.set_redo_tabu_perc (redo_tabu_perc);
		
 	tabueSearch.run ();	
	
	
	return 0;
}