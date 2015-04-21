#include "yuhchen.h"

YuhChen::YuhChen ()
{
	m_network = NULL;
}

YuhChen::YuhChen (rca::Network * net)
{
	m_network = net;
}

YuhChen::YuhChen (std::string file) 
{

	m_network = new rca::Network;
	
	YuhChenReader ycr(file);
	stream_list sb;
	
	//configuring the Network
	ycr.configure_network (*m_network, sb);
	
	configure_streams (sb);
	
}

std::ostream & operator<< (std::ostream & out, stream_t const & t) {
		out << "id (" << t.m_id << ")\n";
		out << "Request (" << t.m_trequest << ")\n";
		out << "source = [";
		for (source_t s : t.m_sources) {
			out << " " << s;
		}
		out << "]\n";
		out << "Members = [";
		for (auto const & m : t.m_group.getMembers ()) {
			out << m << " ";
		}
		out << "]\n";
		
		return out;
}

void YuhChen::configure_streams (stream_list & sb)
{
	//reading the streams
	int id = 0;
	for (auto & it : sb) {
		
		//configuring the group of stream with id
		rca::Group g(id, -1, it.trequest);		
		for (auto & m : it.get_members()) {
			g.addMember (m); //adding the members of group
		}		
		
		//creating stream w_id
		stream_t stream (id, it.trequest, it.get_sources(), g);			
		
		m_streams.push_back (stream);
		
		id++;
	}
}

void YuhChen::add_stream(int id, int req,  std::vector<source_t> S, rca::Group D)
{
	stream_t w;
	
	w.m_id = id;
	w.m_trequest = req;
	
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
	
	return forest;
	
}

int main (int argc, char**argv) 
{

	std::string m_instance(argv[1]);
 	
 	YuhChen yuhchen(m_instance);
	

	
	
	return 0;
}