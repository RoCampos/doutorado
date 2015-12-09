#include "yuhchen.h"

YuhChen::YuhChen ()
{
	m_network = NULL;
}

YuhChen::YuhChen (rca::Network * net)
{
	m_network = net;
	
	//creating the congestion handle that will be used 
	//to improve the cost of solution
	int NODES = this->m_network->getNumberNodes ();
	m_cg.init_congestion_matrix (NODES);
	m_cg.init_handle_matrix (NODES);
	
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
	
	int NODES = this->m_network->getNumberNodes ();
	m_cg.init_congestion_matrix (NODES);
	m_cg.init_handle_matrix (NODES);
	
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

void YuhChen::configure_streams (std::vector<rca::Group> & groups)
{
	
	int id = 0;
	for (auto group : groups) {
		
		std::vector<source_t> sources;
		sources.push_back ( group.getSource() );
		stream_t stream (id, group.getTrequest (), sources, group);
		
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
		
		//shortest_path_tree
		//computed here
		std::vector<int> previous = 
				inefficient_widest_path (s, members[0], m_network);
		
		for (int & d : members) {
			
			rca::Path p = get_shortest_path (s, d, *m_network, previous);
			
			//TODO BUILDING A PATH			
 			std::reverse (p.begin (), p.end());
			
			assert (p.size () > 0);
			
			auto it = p.begin ();
			for ( ; it != p.end()-1; it++) {
				
				int x = *it;
				int y = *(it+1);
				
				int cost = m_network->getCost (x,y);
				
				rca::Link l(x, y, cost);
				
				stob.add_edge (l.getX(), l.getY(), cost, BAND);
				
			} 
			
		} //end of three construction 
		
		stob.prune (1, BAND);
		
		this->update_usage (stob.get_steiner_tree ());
		
		cost += stob.get_steiner_tree().get_cost ();
		//stob.get_steiner_tree().xdotFormat ();
		
		std::vector<rca::Path> tree = rca::sttalgo::stree_to_path (
											stob.get_steiner_tree (), 
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
			for (size_t i=1; i < f.m_trees.size(); i++) {
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
		std::vector<rca::Path> tree = rca::sttalgo::stree_to_path (
														ob.get_steiner_tree(),
														source, NODES);
		
		if (tree.size () > 0) {
			
			tree_t t;
			t.m_paths = tree;
			t.m_source = source;
			
			///adding the STTRe to tree_t
			//it will be used to improve the cost of solutioon
			t.m_tree = ob.get_steiner_tree ();
			
			out_solution.m_trees.push_back ( t );
			out_solution.m_cost += ob.get_steiner_tree ().get_cost ();
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

void YuhChen::update_usage (STTree & st)
{
	
	edge_t * e = st.get_edges ().begin;
	while ( e != NULL) {
		
		rca::Link l (e->x, e->y,0);
		int band_value = this->m_network->getBand (l.getX(), l.getY());
		this->m_network->setBand (l.getX(), l.getY(),band_value-1);
		this->m_network->setBand (l.getY(), l.getX(),band_value-1);
		
		e = e->next;
	}
	
}

void YuhChen::update_cg (STTree & st)
{
	int STREAMS = m_streams.size ();
	edge_t *e = st.get_edges().begin;
	while (e != NULL) {
		
		if (e->in) {
		
			rca::Link l (e->x, e->y, 0);
			if (!m_cg.is_used (l)) {
				
				l.setValue ( STREAMS - 1 );
				this->m_cg.push(l);
				
			} else {
				
				int value = m_cg.value (l) - 1;
				this->m_cg.erase (l);
				l.setValue ( value );
				this->m_cg.push (l);
				
			}
		}
		
		e = e->next;
	}
	
	
}

/* ---------------------- main routine -----------------------*/
void YuhChen::run (int param) 
{
	
	this->m_improve_cost = param;

	int STREAMS = m_streams.size ();
	std::vector<forest_t> F;
	
	std::vector<rca::Link> links;
	
	double cost = 0.0;
	
	//list of sttrees to be improved
	std::vector<STTree> improve;
	
	for (int i=0; i < STREAMS; i++) {
		
		forest_t f = wp_forest (this->m_streams[i]);
		
		std::vector<tree_t> trees = f.m_trees;
		
		for (tree_t t: trees) {
			
			std::vector<rca::Link> tree_links;
			
			if (this->m_improve_cost == 1) {
				//updating the CongestioonHandle Strucute
				this->update_cg (t.m_tree);
				//storing three to improve Cost.
				improve.push_back (t.m_tree);
			}
		
			
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
				
				cost += m_network->getCost (link.getX(), link.getY());
			}
			tree_links.clear ();
			
		}
		
		F.push_back ( f );
 	
	}

 	std::sort (links.begin (), links.end());
 	std::cout << links.begin ()->getValue () << "\t" << cost << "\t";
	
	
	if (this->m_improve_cost == 1) {
		/*Improving solution cost*/
		std::vector<rca::Group> m_groups;
		for (auto stm : m_streams) {			
			rca::Group g= stm.m_group;
			g.setSource (stm.m_sources[0]);
			m_groups.push_back (g);
		}
		
		rca::sttalgo::ChenReplaceVisitor<> c(&improve);
		c.setNetwork (m_network);
		c.setMulticastGroups (m_groups);
		c.setEdgeContainer (m_cg);
		
		c.visitByCost ();
		int tt = 0.0;
		for (auto st : improve) {
			tt += (int)st.get_cost ();
		}
		std::cout << tt << "\t";
		CycleLocalSearch cls;
		cls.local_search (improve, *m_network, 
					m_groups, m_cg, tt);
		std::cout << tt << "\t";
	} 
}

int main (int argc, char**argv) 
{

	std::string m_instance(argv[1]);
	int improve_cost = atoi (argv[2]);
 	
	rca::Network net;
	std::vector<shared_ptr<rca::Group>> g;
	std::vector<rca::Group> groups;
	
	MultipleMulticastReader r(m_instance);	
	r.configure_unit_values (&net, g);
	
	for (auto it : g) {
		groups.push_back (*it.get());
	}
	
	rca::elapsed_time time_elapsed;	
	time_elapsed.started ();
	
	YuhChen yuhchen (&net);
	yuhchen.configure_streams (groups);
	
 	//YuhChen yuhchen(m_instance);
	
	yuhchen.run (improve_cost);
	
	time_elapsed.finished ();
	std::cout << time_elapsed.get_elapsed () << std::endl;
	//forest_t ff = yuhchen.widest_path_tree (0);
	
	//std::cout << ff.m_trees.size () << std::endl;
	//std::cout << ff.m_cost << std::endl;
	//std::cout << ff.Z << std::endl;
	
	return 0;
}