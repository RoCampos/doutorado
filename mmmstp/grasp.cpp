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
	
void Grasp::build_solution ()
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
	
	for (int i=0; i < SIZE; i++) {
		
		int g_idx = group_idx[i];
		
		rca::Group group = m_groups[g_idx];
		std::vector<int> members;
		
		STobserver ob;
		ob.set_container (cg);
		
		int source = group.getSource ();
		STTree steiner_tree(NODES, source, group.getMembers ());
		
		ob.set_steiner_tree (steiner_tree, NODES);
		
		std::vector<rca::Link> links = m_links;
		
		while (!links.empty()) {
			
			auto link = links.begin ();
			
			int cost = m_network->getCost (link->getX(), link->getY());
			ob.add_edge (link->getX(), link->getY(), cost, SIZE);
			links.erase (link);
		}
		
		ob.prune (1, SIZE);
		ob.get_steiner_tree ().xdotFormat ();
		
	}
	
}
	
void Grasp::local_search ()
{
#ifdef DEBUG
	printf ("%s\n",__FUNCTION__);
#endif
}
	
void Grasp::run ()
{
#ifdef DEBUG
	printf ("%s\n",__FUNCTION__);
#endif
	for (int i=0; i < m_iter; i++) {
		
		build_solution ();
		local_search ();
		
	}
	
}

/*Private Methods*/

void update_usage (STTree & sttre)
{
	
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