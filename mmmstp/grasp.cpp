#include "grasp.h"

Grasp::Grasp () 
{
#ifdef DEBUG
	printf ("%s\n",__FUNCTION__);
#endif
}

Grasp::Grasp (rca::Network *net, std::vector<rca::Group> & groups)
{
#ifdef DEBUG
	printf ("%s\n",__FUNCTION__);
#endif
	
	m_network = net;
	m_groups = groups;
	
	//std::vector<rca::Link> m_links(m_network->getLinks ().begin(), 
	//							 m_network->getLinks ().end());
	auto iter = m_network->getLinks ().begin();
	auto end = m_network->getLinks ().end();
	for (; iter != end; iter++) {
		m_links.push_back (*iter);
	}
	
	//std::sort (m_links.begin(), m_links.end());
	
	for (auto & it: m_links) {
		it.setValue (0);
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
	
	//O(n^2)
	CongestionHandle cg;
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);
	
	sttree_t sol;
	
	STobserver ob;
	ob.set_container (cg);
	double m_cost = 0.0;
	
	sol.m_trees.resize (SIZE);
	
	//O(K)
	for (int i=0; i < SIZE; i++) {
		
		int g_idx = group_idx[i];
		
		rca::Group group = m_groups[g_idx];
		
		int source = group.getSource ();
		STTree steiner_tree(NODES, source, group.getMembers ());
		
		ob.set_steiner_tree (steiner_tree, NODES);
		
		//this->spanning_tree (&ob);
		this->shortest_path_tree (g_idx, &ob);
		
		m_cost += ob.get_steiner_tree ().getCost ();
		
		//O(E)
		this->update_usage (ob.get_steiner_tree ());
		
		sol.m_trees[g_idx] = ob.get_steiner_tree ();
	}
	
	sol.m_cost = m_cost;
	sol.m_residual_cap = ob.get_container ().top ();
	sol.cg = cg;
	
	//local_search app
	ChenReplaceVisitor c(&sol.m_trees);
	c.setNetwork (m_network);
	c.setMulticastGroups (m_groups);
	c.setEdgeContainer ( sol.cg );
	
	c.visitByCost ();
	int tt = 0.0;
	int i = 0;
	for (auto st : sol.m_trees) {
		tt += (int)st.getCost ();
	}
	
	sol.m_cost = tt;
	
	return sol;
}

void Grasp::spanning_tree (STobserver * ob)
{
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
			
		//checar disjoint set
		ob->add_edge (link.getX(), link.getY(), cost, GROUPS_SIZE);
		links.erase ( (links.begin () + pos) );
	}
		
	//O(E)
	ob->prune (1, GROUPS_SIZE);
}

void Grasp::shortest_path_tree (int id, STobserver* ob)
{
	
	int N_SIZE = this->m_groups.size ();
	
	int source = this->m_groups[ id ].getSource ();
	const std::vector<int> & destinations = this->m_groups[ id ].getMembers ();
	int G_SIZE = destinations.size ();
	
	int d = 0;
	
	rca::Path spath = shortest_path (source, destinations[ d ], *m_network);
	
	do {
		
		auto rit = spath.rbegin ();
		for (; rit != spath.rend()-1; rit++) {
		
			int x = *rit;
			int y = *(rit+1);
			
			rca::Link l(x, y, 0);
			
			int cost = this->m_network->getCost (l.getX(), l.getY());
			
			ob->add_edge (l.getX(), l.getY(), cost, N_SIZE);
			
			this->m_network->removeEdge (l);
			
		}
		
		d++;
		
		if (d == G_SIZE) break;
		
		spath = shortest_path (source, destinations[ d ], *m_network);
		if (spath.size () == 0) {
			this->m_network->clearRemovedEdges ();
			spath = shortest_path (source, destinations[ d ], *m_network);
		}
		
	} while (d < N_SIZE);
	
	this->m_network->clearRemovedEdges();
}
	
void Grasp::local_search (sttree_t * sol)
{
#ifdef DEBUG
	printf ("%s\n",__FUNCTION__);
#endif
	
	ChenReplaceVisitor c(&sol->m_trees);
	c.setNetwork (m_network);
	c.setMulticastGroups (m_groups);
	c.setEdgeContainer ( sol->cg );
	
	c.visitByCost ();
	int tt = 0.0;
	int i = 0;
	for (auto st : sol->m_trees) {
		tt += (int)st.getCost ();		
	}
	
	std::cout << sol->cg.top () << std::endl;
	std::cout << tt << std::endl;
	
}
	
void Grasp::run ()
{
#ifdef DEBUG
	printf ("%s\n",__FUNCTION__);
#endif
	
	int best_cap = 0;
	int best_cost = INT_MAX;
	sttree_t best;
	
	rca::elapsed_time time_elapsed;	
	time_elapsed.started ();
	
	for (int i=0; i < m_iter; i++) {
		
		sttree_t sol = build_solution ();
		//local_search (&sol);
		
		if (sol.m_residual_cap > best_cap) {
			best_cap = sol.m_residual_cap;
			best_cost = sol.m_cost;
			best = sol;
		} else if (sol.m_residual_cap == best_cap && sol.m_cost < best_cost ) {
			best_cap = sol.m_residual_cap;
			best_cost = sol.m_cost;
			best = sol;
		}
		
		this->reset_links_usage ();
	}
	
	time_elapsed.finished ();
	
	std::cout << best.m_cost << "\n";
 	std::cout << best.m_residual_cap << " ";
 	std::cout << time_elapsed.get_elapsed () << std::endl;
	
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


int main (int argc, char**argv) {
	
	srand ( std::time(NULL) );
	
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
	int iter = atoi (argv[2]);
	double lrc = atof (argv[3]);
	grasp.set_iter (iter);
	grasp.set_lrc (lrc);
	
	
	grasp.run ();
	
}