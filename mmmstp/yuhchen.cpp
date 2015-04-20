#include "yuhchen.h"

YuhChen::YuhChen ()
{
	
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

void YuhChen::set_stream(int id, std::vector<source_t> S, rca::Group D)
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


tree_t YuhChen::widest_path_tree (source_t & source)
{
	
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
 	
 	
 	YuhChen yuhchen;
	
	for (Group & g : m_groups) {		
		std::vector<source_t> sources;
		sources.push_back (g.getSource ());
		yuhchen.set_stream (g.getId(), sources, g);
		
	}
	
	stream_t t = yuhchen.get_stream (0);
	std::cout << t << std::endl;
	
	
	return 0;
}