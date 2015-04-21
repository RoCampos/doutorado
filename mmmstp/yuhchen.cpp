#include "yuhchen.h"

YuhChen::YuhChen ()
{
	m_network = NULL;
}

YuhChen::YuhChen (rca::Network * net)
{
	m_network = net;
}

std::ostream & operator<< (std::ostream & out, stream_t const & t) {
		out << "id (" << t.m_id << ")\n";
		out << "source = [";
		for (source_t s : t.m_sources) {
			out << " " << s;
		}
		out << "]\n";
		out << t.m_group;
		
		return out;
}

void YuhChen::add_stream(int id, std::vector<source_t> S, rca::Group D)
{
	stream_t w;
	
	w.m_id = id;
	
	for (source_t s : S) {
	
		w.m_sources.push_back (s);
		
	}
	
	w.m_group = D;
	
	//adding the stream to list of streams
	m_streams.push_back (w);
}


forest_t YuhChen::widest_path_tree (int stream_id)
{
	int NODES = m_network->getNumberNodes ();
	
	//typedef st observer
	CongestionHandle cg;
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);
	STObserver stob;
	stob.set_container (cg);
	
	stream_t w = m_streams[stream_id];
			
	std::vector<source_t> sources = w.m_sources;
	
	//creating the forest to stream w.
	forest_t forest;
	forest.m_id = w.m_id;
	
	int BAND = w.m_group.getSize () + 1;
	
	int cost = 0;
	
	for (source_t s : sources) {
		
		//building the tree for s
		std::vector<int> members = w.m_group.getMembers ();
		STTree st (NODES, members);
		stob.set_steiner_tree (st, NODES);
		for (int & d : members) {
			
			rca::Path p = inefficient_widest_path (s, d, m_network);
			std::reverse (p.begin (), p.end());
			
			//if (p.size () == 0) exit (1);
			
			auto it = p.begin ();
			for ( ; it != p.end()-1; it++) {
				
				int x = *it;
				int y = *(it+1);
				
				int cost = m_network->getCost (x,y);
				int band = m_network->getBand (x,y);
				rca::Link l(x, y, cost);
				
				//m_network->setBand (l.getX(), l.getY(), band-1);
				
				stob.add_edge (l.getX(), l.getY(), cost, band);
				
			} 
			
		} //end of three construction 
		
		m_network->clearRemovedEdges ();
		
		stob.prune (1, BAND);
		
		cost += stob.get_steiner_tree().getCost ();
		//stob.get_steiner_tree().xdotFormat ();
		
		std::vector<rca::Path> tree = stree_to_path (stob.get_steiner_tree (), 
													 s, 
													NODES);
		tree_t t_s;
		t_s.m_paths = tree;
		t_s.m_source = s;
		
		forest.m_trees.push_back (t_s);
		
	}	
	
	forest.m_cost = cost;
	forest.Z = cg.top ();
	
	//std::cout << "Z =" << cg.top () << std::endl;
	
	return forest;
	
}

int main (int argc, char**argv) 
{

	std::string m_instance(argv[1]);
	MultipleMulticastReader r(m_instance);
	rca::Network m_network;
	std::vector<std::shared_ptr<rca::Group>> g;
	std::vector<rca::Group> m_groups;
	
#ifdef MODEL_UNIT
	r.configure_unit_values (&m_network, g);
#endif
	
	for (unsigned int i =0; i < g.size (); i++) {
 		m_groups.push_back (*g[i].get ());
 	}
 	
 	YuhChen yuhchen(&m_network);
	
	for (Group & g : m_groups) {
		std::vector<source_t> sources;
		sources.push_back (g.getSource ());
		yuhchen.add_stream (g.getId(), sources, g);
	}
	
	stream_t t = yuhchen.get_stream (0);
	//std::cout << t << std::endl;
	
	forest_t F;
	
	double gcost = 0;	
	for (int i=0; i < m_groups.size (); i++) {
		forest_t forest = yuhchen.widest_path_tree (i);
		gcost += forest.m_cost;
		std::cout << forest.m_cost << std::endl;
		std::cout << forest.Z << std::endl;
		
		
		F.m_trees.push_back (forest.m_trees[0]);
	}
	
	std::cout << "Cost(" << gcost << ")" << std::endl;
	
	
	int NODES = m_network.getNumberNodes ();
	int BAND = m_groups.size ();
	CongestionHandle cg;
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);
	for (tree_t t : F.m_trees) {
	
		for (rca::Path & p : t.m_paths) {
			//std::cout<< p << std::endl;			
			auto it = p.begin();
			for ( ; it != p.end()-1; it++) {
				int x = *it;
				int y = *(it+1);
				rca::Link l (x,y,0);
				
				if (cg.is_used (l)) {
					int value = cg.value (l);
					l.setValue (value+1);
					cg.update (l);
				} else {
					l.setValue (1);
					cg.push(l);
				}				
			}			
		}		
	}
	
	
	int size = cg.get_heap ().size ();
	auto it = cg.get_heap ().begin ();
	
	std::advance (it, size-1);
	std::cout << *it << " " << it->getValue () << std::endl;
	
	
	return 0;
}