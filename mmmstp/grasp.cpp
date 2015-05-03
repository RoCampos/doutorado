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
	
	std::cout << m_links[0].getValue () << std::endl;
	
	for (auto & it: m_links) {
		it.setValue (0);
	}
}
	
sttree_t Grasp::build_solution ()
{
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
	
	STobserver ob;
	ob.set_container (cg);
	double m_cost = 0.0;
	
	sol.m_trees.resize (SIZE);
	
	for (int i=0; i < SIZE; i++) {
		
		int g_idx = group_idx[i];
		
		rca::Group group = m_groups[g_idx];
		
		int source = group.getSource ();
		STTree steiner_tree(NODES, source, group.getMembers ());
		
		ob.set_steiner_tree (steiner_tree, NODES);
		
		std::vector<rca::Link> links = m_links;
		
		std::set<int> terminals;
		
		while (!links.empty()) {
			
			auto link = links.begin ();
			
			int cost = m_network->getCost (link->getX(), link->getY());
			ob.add_edge (link->getX(), link->getY(), cost, SIZE);
			links.erase (link);
		}
		
		ob.prune (1, SIZE);
		
		m_cost += ob.get_steiner_tree ().getCost ();
		
		this->update_usage (ob.get_steiner_tree ());
		
		sol.m_trees[g_idx] = ob.get_steiner_tree ();
	}
	
	sol.m_cost = m_cost;
	sol.m_residual_cap = ob.get_container ().top ();
	sol.cg = cg;
	
	return sol;
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
		
// 		edge_t *e = st.get_edges ().begin;
// 		while (e != NULL) {
// 		
// 			if (e->in) {
// 				printf ("%d - %d:%d\n", e->x+1, e->y+1, i+1);
// 			}
// 			e = e->next;
// 		}
// 		i++;
	}
	
	std::cout << sol->cg.top () << std::endl;
	std::cout << tt << std::endl;
	
}
	
void Grasp::run ()
{
#ifdef DEBUG
	printf ("%s\n",__FUNCTION__);
#endif
	for (int i=0; i < m_iter; i++) {
		
		sttree_t sol = build_solution ();
		local_search (&sol);
		std::cout << sol.m_cost << std::endl;
		std::cout << sol.m_residual_cap << std::endl;
		
		this->reset_links_usage ();
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


int main (int argc, char**argv) {
	
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
	grasp.set_iter (10);
	grasp.run ();
	
}