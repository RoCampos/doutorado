#include "grasp.h"

void sttree_t::print_solution ()
{
	int i =0;
	for (auto st : this->m_trees) {
		edge_t *e = st.get_edges ().begin;
		while (e != NULL) {
		
			if (e->in)
				std::cerr <<  e->x+1 << " - " << e->y+1 << ":" << i+1 << std::endl;
			
			e = e->next;
		}
		i++;
	}		
}

Grasp::Grasp () 
{
#ifdef DEBUG
	printf ("%s\n",__FUNCTION__);
#endif
}

Grasp::Grasp (rca::Network *net, 
			  std::vector<rca::Group> & groups, 
			  int budget)
{
#ifdef DEBUG
	printf ("%s\n",__FUNCTION__);
#endif
	
	m_network = net;
	m_groups = groups;
	
	auto iter = m_network->getLinks ().begin();
	auto end = m_network->getLinks ().end();
	for (; iter != end; iter++) {
		m_links.push_back (*iter);
	}
	
	for (auto & it: m_links) {
		it.setValue (0);
	}
	
	if (budget == 0) {
		m_budget = INT_MAX;
	} else {
		m_budget = budget;
	}
}
	
sttree_t Grasp::build_solution () {
#ifdef DEBUG
	printf ("%s\n",__FUNCTION__);
#endif
	
	int SIZE = m_groups.size ();
	int NODES = m_network->getNumberNodes ();
	std::vector<int> group_idx(SIZE);
	iota (group_idx.begin(), group_idx.end(), 0);
	std::random_shuffle (group_idx.begin(), group_idx.end());
	
	CongestionHandle cg;
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);
	
	sttree_t sol;
	sol.cg = cg;
	
	STobserver ob;
	ob.set_container (sol.cg);
	double m_cost = 0.0;
	
	sol.m_trees.resize (SIZE);
	
	//O(K)
	for (int i=0; i < SIZE; i++) {
		
		int g_idx = group_idx[i];
		
		rca::Group group = m_groups[g_idx];
		
		int source = group.getSource ();
		STTree steiner_tree(NODES, source, group.getMembers ());
		
		ob.set_steiner_tree (steiner_tree, NODES);
			
		//create a tree
		double r = (double) (rand () % 100)/100.0;
		
		auto it = cg.get_heap ().ordered_begin ();
		auto end = cg.get_heap ().ordered_end ();
// 		if (i>0)
			for ( ; it != end; it++) {
				
				if (it->getValue () <= cg.top()+2) {
					this->m_network->removeEdge (*it);
					if ( !is_connected (*m_network, m_groups[g_idx]) )
						this->m_network->undoRemoveEdge (*it);
				}
			}
		
		
		if (r < this->m_heur) {
			this->shortest_path_tree (g_idx, &ob);		
		} else {
			this->spanning_tree (&ob);
		}
		//make prunning
		ob.prune (1, SIZE);
		
		//computing the cost and store the tree
		m_cost += ob.get_steiner_tree ().getCost ();
		sol.m_trees[g_idx] = ob.get_steiner_tree ();
		
		this->update_usage (ob.get_steiner_tree());
		
		
		m_network->clearRemovedEdges();
	}
	
	sol.m_cost = m_cost;
	sol.m_residual_cap = ob.get_container ().top ();
	
	this->reset_links_usage ();
	
	return sol;
}

void Grasp::spanning_tree (STobserver * ob)
{
#ifdef DEBUG
	printf ("%s\n",__FUNCTION__);
#endif
	
	std::vector<rca::Link> links = m_links;
	int GROUPS_SIZE = m_groups.size ();
		
	//O(E)
	while (!links.empty()) {
		
		int size = m_lrc * links.size ();
		
		int pos = -1;
		if (size > 0) {
			pos = rand () % size;
		}else {
			pos = 0;
		}
			
		rca::Link link = links[pos];
						
		int cost = m_network->getCost (link.getX(), link.getY());
			
		if (this->m_network->isRemoved(link)){
			links.erase ( (links.begin () + pos) );
			continue;
		}
		
		//checar disjoint set
		ob->add_edge (link.getX(), link.getY(), cost, GROUPS_SIZE);
		links.erase ( (links.begin () + pos) );
	}
		
	//O(E)
// 	ob->prune (1, GROUPS_SIZE);
}

void Grasp::shortest_path_tree (int id, STobserver* ob)
{
	
	int N_SIZE = this->m_groups.size ();
	
	int source = this->m_groups[ id ].getSource ();
	std::vector<int> destinations = this->m_groups[ id ].getMembers ();
	int G_SIZE = destinations.size ();
	
// 	std::random_shuffle(destinations.begin(), destinations.end());
	
	int d = 0;
	
	rca::Path spath = shortest_path (source, destinations[ d ], *m_network);
	
	do {

		//current path will be removed from network
		std::vector<rca::Link> current_path;
		
		auto rit = spath.rbegin ();
		for (; rit != spath.rend()-1; rit++) {
		
			int x = *rit;
			int y = *(rit+1);
			
			rca::Link l(x, y, 0);
			
			int cost = this->m_network->getCost (l.getX(), l.getY());
			
			ob->add_edge (l.getX(), l.getY(), cost, N_SIZE);
			
 			this->m_network->removeEdge (l);

			current_path.push_back (l);
			
		}
		
		d++;
		
		if (d == G_SIZE) break;

		spath = shortest_path (source, destinations[ d ], *m_network);
		
		bool cleaned = false;
		if (spath.size () == 0) {
			this->m_network->clearRemovedEdges ();
			spath = shortest_path (source, destinations[ d ], *m_network);
			cleaned = true;			
		}
		
		//removing a path d if path d+1 was found.
		if ( !cleaned ) {
		
			for (auto l : current_path) {
				m_network->removeEdge (l);
			}
			
		}
		
	} while (d < G_SIZE);

}
	
void Grasp::cost_refinament (sttree_t * sol, ChenReplaceVisitor & c)
{
#ifdef DEBUG
	printf ("%s\n",__FUNCTION__);
#endif
	
	c.visitByCost ();
	int tt = 0.0;
	for (auto st : sol->m_trees) {
		tt += (int)st.getCost ();		
	}
	
#ifdef DEBUG
 	if (sol->m_cost > tt) {
 		printf ("Improved by cost refinement ");
 		printf ("from %d to %d ", sol->m_cost, tt);
	}
#endif
	
	sol->m_cost = tt;
	sol->m_residual_cap = sol->cg.top ();
	
}

void Grasp::residual_refinament (sttree_t * sol, ChenReplaceVisitor& c)
{
#ifdef DEBUG
	printf ("%s\n",__FUNCTION__);
#endif
	
	bool improve = true;
	int tmp_cong = sol->cg.top();
	
	double cost = 0;
	double congestion = 0;
	
 	sttree_t tmp_tree;
	
	while (improve) {
		c.visit ();
				
		int temp_cost = 0;
		for (auto st : sol->m_trees) {
			temp_cost += (int)st.getCost ();
		}
		
		if (sol->cg.top () > tmp_cong && cost <= m_budget) {
			congestion = sol->cg.top ();
			
			tmp_cong = congestion;
			cost = temp_cost;
			
#ifdef DEBUG
	std::cout << "Improved by residual refinement ";
	std::cout << sol->m_residual_cap << " to "<< congestion << std::endl;
#endif
			tmp_tree.m_trees = sol->m_trees;
			tmp_tree.m_cost = cost;
			tmp_tree.m_residual_cap = congestion;
			tmp_tree.cg = sol->cg;

		} else if (sol->cg.top () > tmp_cong 
			&& temp_cost < sol->m_cost
			&& cost <= m_budget){
			
			tmp_tree.m_trees = sol->m_trees;
			tmp_tree.m_cost = cost;
			tmp_tree.m_residual_cap = congestion;
			tmp_tree.cg = sol->cg;

		}else {
			break;
		}
	}
	if (tmp_tree.m_trees.size () > 0)
		*sol = tmp_tree;
}
	
void Grasp::run ()
{
#ifdef DEBUG
	printf ("%s\n",__FUNCTION__);
#endif
	
	int best_cap = 0;
	int best_cost = 0;
	int best_iter = 0;
	
	sttree_t best;
	
	sttree_t alt_best;
	
	rca::elapsed_time time_elapsed;	
	time_elapsed.started ();
	
	for (int i=0; i < m_iter; i++) {
		
		sttree_t sol = build_solution ();
		
		ChenReplaceVisitor c(&sol.m_trees);
		c.setNetwork (m_network);
		c.setMulticastGroups (m_groups);
		c.setEdgeContainer (sol.cg);

		cost_refinament (&sol, c);
 		residual_refinament (&sol, c);
		
#ifdef DEBUG
	std::cout << sol.m_residual_cap << " " << sol.m_cost << "\n";	
#endif
		std::cout << sol.m_residual_cap << " " << sol.m_cost << "\n";
		if (sol.m_residual_cap > best_cap 
			&& sol.m_cost <= m_budget) {
			
			best_cap = sol.m_residual_cap;
			best_cost = sol.m_cost;
			best = sol;
		
			best_iter = i;
		
		} else if (sol.m_residual_cap == best_cap 
			&& sol.m_cost < best_cost 
			&& sol.m_cost <= m_budget) {
			
			best_cap = sol.m_residual_cap;
			best_cost = sol.m_cost;
			best = sol;
		
			best_iter = i;
		
		} else {
		
			if (sol.m_residual_cap > best_cap) {
				alt_best = sol;
			} else if (sol.m_residual_cap == best_cap 
						&& sol.m_cost){
				alt_best = sol;
			}
			
		}
	}
	
	time_elapsed.finished ();
	
	if (!best_cap == 0) {
	
		std::cout << best.m_cost << " ";
		std::cout << best.m_residual_cap << " ";
		std::cout << time_elapsed.get_elapsed () << " ";
		std::cout << best_iter << " ";
		std::cout << m_seed << std::endl;
#ifdef DEBUG		
		best.print_solution ();
#endif
		
	} else {		
		std::cout << alt_best.m_cost << " ";
		std::cout << alt_best.m_residual_cap << " ";
		std::cout << time_elapsed.get_elapsed () << " ";
		std::cout << best_iter << " ";
		std::cout << m_seed << std::endl;
#ifdef DEBUG		
		alt_best.print_solution ();
#endif
	}
	
	
}

/*Private Methods*/

void Grasp::update_usage (STTree & sttre)
{
	
	edge_t * e = sttre.get_edges ().begin;
	while (e != NULL) {
	
		if (e->in) { 
		
			rca::Link l(e->x, e->y,0);
			auto link = std::find(m_links.begin (), m_links.end(), l);
			link->setValue ( link->getValue () + 1);
			
		}
		
		e = e->next;
	}
	
	std::sort(m_links.begin(), m_links.end());
	
}

void Grasp::reset_links_usage ()
{
	for (auto & it: m_links) {
		it.setValue (0);
	}
}

void help ()
{

	printf ("Grasp Algorithm For the MMRBP problem\n");
	printf ("Input:\n");
	printf ("\t./grasp <instance> ");
	printf (" --iter <value> --lrc <value> --heur <value> --budget <value>\n");
	
	std::string description = "Descrition\n";
	description += "\t--iter: defines the number of iterations\n";
	description += "\t--lrc: parameter used in spanning_tree algorithm\n";
	description += "\t--budget: the budget used for limite the cost of solution\n";
	description += "\t--heur: value the determins the algorithm to used";
	description += "ShortestPH or Spanning Minimum Tree\n";
	
	printf ("%s", description.c_str ());
}

int main (int argc, char**argv) {
	
	if (argc < 10) {
		help ();
		exit (0);
	} 
	
	int m_seed = std::time(NULL);
	srand ( m_seed );
	
	rca::Network m_network;
	std::vector<std::shared_ptr<rca::Group>> g;
	std::vector<rca::Group> m_groups;
	std::string file(argv[1]);
	
	MultipleMulticastReader reader(file);
	reader.configure_unit_values (&m_network, g);
	for (std::shared_ptr<rca::Group> i : g) {
		m_groups.push_back (*i);
	}
	
	Grasp grasp(&m_network, m_groups);
	int iter = atoi (argv[3]);	
	double lrc = atof (argv[5]);
	double heur = atof (argv[7]);
	
	int budget = atof (argv[9]);
// 	std::string file2(argv[9]);
// 	std::fstream bud(file2.c_str());
// 	if (bud.good()) {
// 		bud >> budget;
// 	} else {
// 		budget = 0;
// 	}
	
#ifdef DEBUG
	printf ("grasp %s --iter %d --lrc %.2f --heur %d --budget %.2f\n",file.c_str(),iter, lrc, budget, heur);
#endif
	
	grasp.set_iter (iter);
	grasp.set_lrc (lrc);
	grasp.set_budget (budget);
	grasp.set_heur (heur);
	grasp.set_seed (m_seed);
	
	grasp.run ();
	
}