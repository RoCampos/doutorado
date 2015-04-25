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
	
	//definindo capacidade igual a quantidade de streams
	for (int i=0; i < m_network->getNumberNodes (); i++) {
		for (int j=0; j < m_network->getNumberNodes (); j++) {
		
			if (m_network->getBand (i,j) > 0) {
				m_network->setBand (i,j, sb.size () );
			}			
		}
	}
	
	
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
	
	int BAND = w.m_group.getSize ();
	
	int cost = 0;
	
	for (source_t s : sources) {
		
		//building the tree for s
		std::vector<int> members = w.m_group.getMembers ();
		STTree st (NODES, members);
		stob.set_steiner_tree (st, NODES);
		for (int & d : members) {
			
			rca::Path p = inefficient_widest_path (s, d, m_network);
			std::reverse (p.begin (), p.end());
			
// 			std::cout << s << "-->" << d << std::endl;
			assert (p.size () > 0);
			
			auto it = p.begin ();
			for ( ; it != p.end()-1; it++) {
				
				int x = *it;
				int y = *(it+1);
				
				int cost = m_network->getCost (x,y);
				int band = m_network->getBand (x,y);
				rca::Link l(x, y, cost);
				
				stob.add_edge (l.getX(), l.getY(), cost, BAND);
				
			} 
			
		} //end of three construction 
		
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

forest_t YuhChen::wp_forest (stream_t & stream)
{
	//This call is the same as the loop (line 2-4) 
	//in the paper	
	forest_t f = this->widest_path_tree ( stream.m_id );
	
	std::vector<int> members = stream.m_group.getMembers ();
	std::vector<source_t> & sources = stream.m_sources;
		
	std::vector<rca::Path> paths_stream;
	
	//loop line (5-22)
	for (auto const & d : members) {
		int current_node = d;

		//printf ("%d ", current_node);
		rca::Path path;
		path.push (current_node);
		
		while (true) {
			
			//this method should be implemented in Path api.
			tree_t t1 = f.m_trees[0];
			rca::Path path_d_s = t1.find_path (d);
			
			//getting next node
			int next_node = this->next_node ( path_d_s , current_node);
			
			//getting bottleneck
			rca::Link next_width = this->get_bottleneck_link (path_d_s);
			
			//iner-loop lines (10-15)
			//this loop try to find a "better path from current to an source"
			for (int i=1; i < f.m_trees.size(); i++) {
				tree_t t_i = f.m_trees[i];
				rca::Path path_i = t_i.find_path ( d );
								
				rca::Link l = this->get_bottleneck_link (path_i);
				
				if (l.getValue () > next_width.getValue ()) {
				
					next_node = this->next_node ( path_i, current_node );
					next_width.setValue ( l.getValue() );					
				}
			}
			
			//updating current node
			current_node = next_node;
			
			//adding current_node to the path
			path.push (current_node);
			
			//test if the paths arrives at the source
			if (std::find(sources.begin(), 
				sources.end(), current_node) != sources.end())
			{
				//adding the path to list of paths
				paths_stream.push_back ( path );
				
// 				std::cout << path << std::endl;
				
				break;
			}
			
		}//endf while loop
		
	}//end of for loop
	
	forest_t out = to_forest (stream.m_id, paths_stream);
	
	return out;
}

/* Yuh-Rong private methods*/

forest_t YuhChen::to_forest (int stream_id, std::vector<rca::Path> paths)
{
	
	std::vector<source_t> sources = this->m_streams[stream_id].m_sources;
	std::vector<int> members = this->m_streams[stream_id].m_group.getMembers ();
	
	int NODES = m_network->getNumberNodes ();
	int SOURCES = sources.size ();
	
	//manipulador de uso de arestas
	CongestionHandle cg;
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);
	
	std::vector<STTree> sttrees;
	for (int i=0;i < SOURCES; i++) {
		STTree st(NODES, sources[i], members);
		sttrees.push_back (st);
	}
	
		
	STObserver ob;
	ob.set_container (cg);
	std::vector<STObserver> observers( SOURCES );
	std::fill (observers.begin(), observers.end(), ob);
	
	for (int i=0; i < SOURCES; i++) {
		observers[i].set_steiner_tree ( sttrees[i], NODES );
	}
	
	for (auto path : paths) {
	
		int source = path[ path.size () -1 ];
		int source_idx = this->m_streams[stream_id].get_source_index (source);
		
		//std::cout << path << std::endl;
		
		auto it = path.begin ();
		for ( ; it != path.end ()-1; it++) {
			
			int x = (*it);
			int y = *(it+1);
			int cost = (int)m_network->getCost( x, y);
			int band_usage = (int)m_network->getBand( x, y);
			rca::Link l ( x, y, cost );
			
			observers[source_idx].add_edge (l.getX(), l.getY(), cost, band_usage );
			
		}
		
	}
	
	//creating the forest to be returned
	forest_t out_solution;
	out_solution.m_id = stream_id;
	
	int i=0;
	for (auto ob : observers) {
		//o número de grupos é a capacidade da aresta.
		ob.prune (1, this->m_streams.size ());
		//ob.get_steiner_tree ().xdotFormat ();
		int source = this->m_streams[stream_id].m_sources[i];
		std::vector<rca::Path> tree = stree_to_path (ob.get_steiner_tree(),
													 source, NODES);
		
		if (tree.size () > 0) {
			
			tree_t t;
			t.m_paths = tree;
			t.m_source = source;
			
			out_solution.m_trees.push_back ( t );
			out_solution.m_cost += ob.get_steiner_tree ().getCost ();
		}
		i++;
	}
	
	return out_solution;
}

int YuhChen::next_node (rca::Path & path, int current_node)
{
	
	auto it = path.begin ();
	for (;it != path.end(); it++) {
		
		if (*it == current_node)
			break;
	}
	
	return (it != path.end() ? *(it+1) : -1);
	
}

rca::Link YuhChen::get_bottleneck_link (rca::Path & path) 
{
	//paths are stores in reverse order
	auto it = path.cbegin ();
	int min_bottleneck = INT_MAX;
	rca::Link l;
	for ( ; it != path.cend() -1; it++) {
		
		int band = m_network->getBand( *it , *(it+1) );
		//rca::Link l ( *it , *(it+1), );
		
		if ( band < min_bottleneck) {
			min_bottleneck = band;
			l = rca::Link( *it , *(it+1), band);
		}
	}
	
	return l;
}

void YuhChen::run () 
{

	int STREAMS = m_streams.size ();
	std::vector<forest_t> F;
	
	std::vector<rca::Link> links;
	
	for (int i=0; i < STREAMS; i++) {
		
		printf ("Stream(%d)\n", i);
		
		forest_t f = wp_forest (this->m_streams[i]);
		
		std::vector<tree_t> trees = f.m_trees;
		
		for (auto t: trees) {
			
			std::vector<rca::Link> tree_links;
			
			//getting the links of a tree
			for (auto p: t.m_paths) {
				
				auto node = p.begin ();
				for ( ; node != p.end()-1; node++) {
					int x = *node;
					int y = *(node+1);
					
					rca::Link l(x,y,0);
					
					if (std::find (tree_links.begin(), tree_links.end(),l) 
						== tree_links.end() )
					{
						tree_links.push_back (l);
					}
					
				}
			}
			
			//updating the congestion
			for (auto link : tree_links) {
				if (std::find(links.begin(), links.end(), link) == links.end())
				{
					link.setValue( STREAMS-1 );
					links.push_back (link);
				} else {
					auto tmp = std::find(links.begin(), links.end(), link);
					
					int value = tmp->getValue () - 1;
					links.erase (tmp);
					link.setValue (value);
					links.push_back (link);
				}
			}
			tree_links.clear ();
			
		}
		
		F.push_back ( f );
 	
	}

	std::cout << links.size () << std::endl;
	
 	std::sort (links.begin (), links.end());
 	std::cout << links.begin ()->getValue () << std::endl;

	
}

int main (int argc, char**argv) 
{

	std::string m_instance(argv[1]);
 	
 	YuhChen yuhchen(m_instance);
	
	yuhchen.run ();
	
	//forest_t ff = yuhchen.widest_path_tree (0);
	
	//std::cout << ff.m_trees.size () << std::endl;
	//std::cout << ff.m_cost << std::endl;
	//std::cout << ff.Z << std::endl;
	
	
	return 0;
}