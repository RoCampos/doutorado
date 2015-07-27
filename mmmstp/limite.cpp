#include "limite.h"

typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> CongestionHandle;
typedef typename rca::sttalgo::SteinerTreeObserver<CongestionHandle> STobserver;

double kruskal_limits (std::vector<rca::Group> & m_groups, 
					   rca::Network & m_network)
{

	std::vector<rca::Link> m_edges, m_edges_aux;
	
	int NODES = m_network.getNumberNodes ();
	int GROUPS_SIZE = m_groups.size ();
	
	for (int i=0; i < NODES; i++) {
		for (int j=0; j < i; j++) {
			double cost = m_network.getCost (i,j);
			if (cost > 0.0) {
				rca::Link link (i,j,cost);
				m_edges.push_back (link);
				m_edges_aux.push_back (link);
			}
		}
	}
	
	std::sort (m_edges.begin (), m_edges.end());
	std::sort (m_edges_aux.begin (), m_edges_aux.end());
	
	CongestionHandle cg;
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);
	
	STobserver ob;
	ob.set_container (cg);
	
	double cost = 0.0;
	for (Group g : m_groups) {
	
		int source = g.getSource ();
		STTree steiner_tree(NODES, source, g.getMembers ());
		ob.set_steiner_tree (steiner_tree, NODES);
		
		while (!m_edges_aux.empty()) {
			
			rca::Link link = m_edges_aux[0];
			
			ob.add_edge (link.getX(), link.getY(), 
						  link.getValue(), GROUPS_SIZE);
			
			m_edges_aux.erase (m_edges_aux.begin ());
			
		}
		
		ob.prune (1,GROUPS_SIZE);
		
		cost += ob.get_steiner_tree().getCost ();
		
		m_edges_aux = m_edges;		
	}
	std::cout << ob.get_container().top () << std::endl;
	return cost;
}

double dijkstra_limits (std::vector<rca::Group> & m_groups, 
						rca::Network & m_network)
{
	int NODES = m_network.getNumberNodes ();
	int GROUPS_SIZE = m_groups.size ();

	std::vector<rca::Link> m_edges, m_edges_aux;	
	for (int i=0; i < NODES; i++) {
		for (int j=0; j < i; j++) {
			double cc = m_network.getCost (i,j);
			if (cc > 0.0) {
				rca::Link link (i,j,cc);
				m_edges.push_back (link);
				m_edges_aux.push_back (link);
			}
		}
	}
	
	std::sort (m_edges.begin (), m_edges.end(), std::greater<rca::Link>());
	std::sort (m_edges_aux.begin (), m_edges_aux.end(), std::greater<rca::Link>());
	
	CongestionHandle cg;
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);
	
	STobserver ob;
	ob.set_container (cg);
	
	double cost = 0.0;
	for (Group g : m_groups) {
		
		for (auto itt : m_edges) {
			m_network.removeEdge (itt);
	 		if ( !is_connected ( m_network, g ) )
				m_network.undoRemoveEdge (itt);
		}
		
		int source = g.getSource ();
		std::vector<int> destinations = g.getMembers ();
		int G_SIZE = destinations.size ();
		
		STTree steiner_tree(NODES, source, g.getMembers ());
		ob.set_steiner_tree (steiner_tree, NODES);
		
		ob.set_steiner_tree (steiner_tree, NODES);
		
		std::vector<int> prev;
		prev = all_shortest_path (source, destinations[0] ,m_network);
		
		for (int m : destinations) {
			rca::Path path = get_shortest_path (source, m, m_network, prev);
				
			auto rit = path.rbegin ();
			for (; rit != path.rend()-1; rit++) {
			
				int x = *rit;
				int y = *(rit+1);
					
				rca::Link l(x, y, 0);
					
				int cc = m_network.getCost (l.getX(), l.getY());
					
				ob.add_edge (l.getX(), l.getY(), cc, GROUPS_SIZE);
			}			
		}
		
// 		ob.prune (1, GROUPS_SIZE);
		cost += ob.get_steiner_tree ().getCost();
		
	}
	
	return cost;
}

int main (int argc, char **argv) {

	rca::Network m_network;
	std::vector<std::shared_ptr<rca::Group>> g;
	std::vector<rca::Group> m_groups;
	std::string file(argv[1]);
	
	MultipleMulticastReader reader(file);
	reader.configure_unit_values (&m_network, g);
	for (std::shared_ptr<rca::Group> i : g) {
		m_groups.push_back (*i);
	}
	
	
	double cost = kruskal_limits (m_groups, m_network);
	printf ("Limite AGM: %.2f\n", cost);
	
}