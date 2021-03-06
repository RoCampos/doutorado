#include "tabusearch.h"

bool has_improv = false;

using namespace rca::sttalgo;

template <class SolutionType, class Container, class ObjectiveType>
rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::TabuSearch (
	std::string& file, 
	std::string reverse,
	std::string sort) 
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	this->m_agm_fact = NULL;
	this->m_sph_fact = NULL;
	this->m_wsp_fact = NULL;
	
	std::vector<std::shared_ptr<rca::Group>> g;
	
	rca::reader::MultipleMulticastReader reader(file);

#ifdef MODEL_REAL
	reader.configure_real_values (&this->m_network,g);
#endif
	
#ifdef MODEL_UNIT
	reader.configure_unit_values (&this->m_network,g);
#endif
	
	for (std::shared_ptr<rca::Group> i : g) {
		this->m_groups.push_back (*i);
	}

	if (reverse.compare ("yes") == 0) {		 
		if (sort.compare("request") == 0) {			
			std::sort (this->m_groups.begin(), this->m_groups.end(), rca::CompareGreaterGroup());
		} else if (sort.compare("size") == 0){			
			std::sort (this->m_groups.begin(), this->m_groups.end(), rca::CompareGreaterGroupBySize());
		}
	} else {
		if (sort.compare("request") == 0) {
			std::sort (this->m_groups.begin(), this->m_groups.end(), rca::CompareLessGroup());
		} else if (sort.compare("size") == 0){
			std::sort (this->m_groups.begin(), this->m_groups.end(), rca::CompareLessGroupBySize());
		}
	}

	
	this->m_best = std::numeric_limits<ObjectiveType>::min ();
	this->m_cost = std::numeric_limits<ObjectiveType>::max ();
	
	m_tabu_list = std::vector<int>( this->m_groups.size(), 0 );
	m_best_cost = std::vector<int>( this->m_groups.size(), this->m_cost );
	
	m_has_init = false;
	
}

template <class SolutionType, class Container, class ObjectiveType>
void rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::run (
	std::string result) 
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
 		if (this->update_best_solution (sol, rr, cc) ) {
			best_iteration = iter;
			this->check (sol, __FUNCTION__);
 		}
				
	} while (iter++ < this->m_iter);

	_time_.finished ();

	if (result.compare ("full") == 0) {
		std::cout << this->m_best << " ";
		std::cout << this->m_cost << " ";
		std::cout << _time_.get_elapsed() << " ";
		std::cout << m_seed << " ";
		std::cout << best_iteration << std::endl;

			rca::sttalgo::print_solution2<SolutionType> (this->m_best_sol);
	} else if (result.compare ("res") == 0) {
		std::cout << this->m_best << "\n";
	} else if (result.compare ("cos") == 0) {
		std::cout << this->m_cost << "\n";
	}

  	

}


template <class SolutionType, class Container, class ObjectiveType>
void rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::build_solution (
	std::vector<SolutionType>& sol, 
	ObjectiveType& res_sol, 
	ObjectiveType& cost_sol,
	Container &cg)
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
		
	this->start_factory ();

	assert (this->m_network.isConnected());

	rca::Network copy;

	if (this->m_type.compare ("AGM") == 0)
		this->m_agm_fact->create_list (this->m_network);

	if (this->m_type.compare ("WSP") == 0)
		copy = this->m_network;

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
			assert (sol[i].get_all_edges().size () > 0);
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
			if (this->m_type.compare ("SPH") == 0) {
				rca::sttalgo::remove_top_edges<CongestionHandle> (cg, 
 													m_network, 
 													m_groups[i], 0);	
			}
		
			if (this->m_type.compare ("WSP") == 0) {
				rca::sttalgo::remove_top_edges<CongestionHandle>(
					m_network, m_groups[i], cg);
			}

		}
		
		//building the tree
		if (this->m_type.compare ("AGM") == 0) {
			this->m_agm_fact->build (ob, m_network, m_groups[i], cg);
			ob.prune (trequest, GROUPS);
		} else if (this->m_type.compare ("SPH") == 0){
			this->m_sph_fact->build (ob, m_network, m_groups[i], cg);
		} else {
			this->m_wsp_fact->build (ob, m_network, m_groups[i], cg);
		}
		
		
		//updating the cost
		cost += ob.get_steiner_tree ().get_cost ();
		
 		m_network.clearRemovedEdges ();
		
		sol[i] = ob.get_steiner_tree ();
		assert (sol[i].get_all_edges().size () > 0);

		if (this->m_type.compare ("AGM") == 0) { 
			this->m_agm_fact->update_usage (m_groups[i], m_network, ob.get_steiner_tree ());
		}		
		
	}

	if (this->m_type.compare ("WSP") == 0) {
		this->m_network = copy;
	}

	this->check (sol, __FUNCTION__);

	this->finish_factory();

	res_sol = cg.top ();
	cost_sol = cost;	
}

template <class SolutionType, class Container, class ObjectiveType>
bool rca::metaalgo::TabuSearch<SolutionType, Container, ObjectiveType>::update_best_solution (
		const std::vector<SolutionType>& sol,
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

	for (auto st : sol) {
		assert (st.get_all_edges ().size () > 0);
	}
	

	this->check (sol, __FUNCTION__);

	//cleang the network
	this->m_network.clearRemovedEdges();
}

typedef rca::EdgeContainer<rca::Comparator, rca::HCell> CongestionHandle;

std::string commandLine() {
	std::string command = "tabu --inst [brite]";
	command += " --iter [int] --budget --list_perc [0.0,1.0] --tabu [0.0, 1.0] --upd8 [int]";
	// command += ""
	return command;
}

int main (int argv, char const *argc[]) {

	if (message (argv, argc, commandLine()) ) {
		exit (1);
	}

	int r = time(NULL);
  	srand ( r );
	
	using namespace rca;
	using namespace rca::metaalgo;
	
	std::string file(argc[2]);
	int iterations = atoi(argc[4]);
	
	int budget = atoi (argc[6]);
	
	if (budget == 0) budget = std::numeric_limits<int>::max();
	
	double list_perc = atof (argc[8]);
	
	double redo_tabu_perc = atof (argc[10]);
	
	int update = atoi (argc[12]);

	std::string type = argc[14];

	std::string reverse = argc[16];

	std::string sort = argc[18];

	std::string result = argc[20];
	
	TabuSearch<steiner, CongestionHandle, int> tabueSearch (file, result, type);
	tabueSearch.set_iterations ( iterations );
	tabueSearch.set_budget ( budget );
	tabueSearch.set_tabu_links_size (list_perc);
	tabueSearch.set_seed ( r );
	tabueSearch.set_update_by_cost (update);
	tabueSearch.set_redo_tabu_perc (redo_tabu_perc);
	tabueSearch.set_type (type);

		
 	tabueSearch.run (result);	
		
	return 0;
}
