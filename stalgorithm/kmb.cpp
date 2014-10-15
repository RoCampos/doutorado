#include "kmb.h"

void KMB::heuristic (rca::Network & net, SteinerTree & st, rca::Group & g) {

	printf ("%s\n", "Running KMB heuristic!");
	
	std::vector<rca::Path> paths;
	
	std::vector<int> nodes(net.getNumberNodes(),1);
	nodes[0] = 0;
	std::partial_sum (nodes.begin (), nodes.end(), nodes.begin(), std::plus<int>());
	
	//defining the source as terminal
	st.setTerminal (g.getSource ());
	
	auto i = g.begin ();
	for ( ;i != g.end (); i++) {
		
		//setting the terminal node
		st.setTerminal (*i);
	
		rca::Path ps = shortest_path (*i, g.getSource (), &net);
		
		auto j = i+1;
		for ( ; j != g.end (); j++) {
		
			if (*j == *i) {
				continue;
			}
			
			rca::Path p = shortest_path (*i, *j, &net);
			
			paths.push_back (p);
			
			//printf ("%d-%d: ", *i, *j);
			//std::cout << p << ":"<< p.getCost ()<< std::endl;			
			
		}
		
		//printf ("%d-%d: ", *i, g.getSource() );
		//std::cout << ps << ":"<< ps.getCost ()<< std::endl;
		paths.push_back (ps);
	}
	
	std::sort (paths.begin (), paths.end());
	
	for (unsigned i = 0; i < paths.size (); i++) {
		
		int v = paths[i][ 0 ];
		int w = paths[i][ paths[i].size () -1 ];
		
		//std::cout << paths[i] << std::endl;
		//cout << nodes [v] << "===" << nodes[w] << std::endl;
		if ( nodes[v] != nodes[w] ) {
			
			int _w = nodes[w];
			for (unsigned j = 0; j < nodes.size (); j++) {
				
				if ( nodes [j] == _w ) {
					 nodes [j] = nodes [v];
				}
			}
			
			nodes [w] = nodes [v];
			//printf ("%d - %d\n", v, w);			
			//std::cout << paths[i] << std::endl;
			auto it = paths[i].begin ();
			for ( ; it != paths[i].end()-1; ++it) {
				
				rca::Link l (*it, *(it+1), net.getCost(*it, *(it+1) ) );
				st.addEdge (l.getX(), l.getY(), l.getValue() );
				
			}
			
		}
		
	}
	
	st.prunning ();
	//st.xdotFormat ();	
	
}