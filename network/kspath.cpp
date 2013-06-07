#include "kspath.h"

KShortestPath::KShortestPath (rca::Network * net) {
	
	network = net;
}

/**
 * Create the first path.
 * 
 */
void KShortestPath::init (int v, int w) { 

	rca::Path path = shortest_path (v,w, network)	;
	
	all_paths.push_back (path);
	
	int index = all_paths.size () - 1;
	
	PathNode node;
	node.cost = path.getCost ();
	node.index = index;
	
	m_candidate_paths.insert (node);
	m_derivation_paths[index] = v;
	
}

bool KShortestPath::hasNext () {
		
	return (m_candidate_paths.size () > 0 ? true : false);
}

rca::Path KShortestPath::next () {
	
	auto pt = m_candidate_paths.begin ();
	int current_path = pt->index;
	m_candidate_paths.erase (pt);
	
	m_kpaths.push_back (current_path);
	
	int count = m_kpaths.size ();
	
	int derivation_vertex = m_derivation_paths[current_path];
	
	std::vector<int> current_path_derivation;
	
	all_paths[current_path].subpath (derivation_vertex, 
									 current_path_derivation);
	
	int subpath_size = current_path_derivation.size ();
	
	//removing some arcs
	//cout << "Removing arcs function" << endl;
	for (int i=0; i < count-1; i++) {
		
		std::vector<int> sub_path_derivation;
		
		int idx = m_kpaths[i];
		
		//cout << all_paths[idx] << endl;
		if (! all_paths[idx].subpath(derivation_vertex, sub_path_derivation)) {
			continue;
		}
		
		if ( subpath_size != sub_path_derivation.size () ) {
			continue;
		}
		
		bool is_equal = true;
		for (int i=subpath_size-1; i >=0; i--) {
			if (current_path_derivation[i] != sub_path_derivation[i]) {
				is_equal = false;
				break;
			}
		}
		
		if (!is_equal) continue;
		
		int vertex_sucessor = all_paths[current_path][subpath_size+1];
		//cout << "VS: " << vertex_sucessor << endl;
		
		double cost = network->getCost (derivation_vertex,
										  vertex_sucessor);
		
		rca::Link link(derivation_vertex,vertex_sucessor,cost);
		//cout << "Removing: " << link << endl;
		network->removeEdge (link);
		
	}
	
	//removing the current path
	int path_length = all_paths[current_path].size ();
	for (int i=path_length-1; i >= 1; --i) {
		
		int v = all_paths[current_path][i];
		int w = all_paths[current_path][i-1];
		double cost = network->getCost (v,w);
		
		//cout << "removing: "<< v << "--" << w << endl;
		network->removeEdge (rca::Link(v,w,cost) );
		network->removeVertex (v);
	}
	
	bool is_done = false;
	for (int i=1; i < path_length; i++) {
		
		//cout << "DV:" << all_paths[current_path][i] << endl;
		
		int new_derivation = all_paths[current_path][i];
		network->unremoveVertex (new_derivation);
		
		if (new_derivation == derivation_vertex) is_done = true;
		
		int destination = all_paths[current_path][0];
		
		rca::Path path;		
		path = shortest_path (new_derivation,destination,
							  network);

		if (path.size () > 0) {
			
			rca::Path prePath;
			double cost = 0.0;
			
			for (int j = 0; j < path.size () ; j++) { 
				int v = path[j];
				prePath.push (v);
				if (prePath.size () > 1) {
					int w = prePath[j-1];
					cost += network->getCost (v,w);
				}
			}
			
			std::vector<int> vet;
			for (int j = path_length - 1; j >= 0; j--) {
				
				int v = all_paths[current_path][j];
				if (v != new_derivation) {
					
					//prePath.push (v);
					vet.push_back (v);
					if (prePath.size () > 1) {
						int w = all_paths[current_path][j-1];
						cost += network->getCost (v,w);
					}
				} else {
					break;
				}
			}
			
			for (int j=vet.size ()-1; j >= 0; j--) {
				prePath.push(vet[j]);
			}
			
			prePath.setCost (cost);
			//cout << prePath << endl;
			
			if (std::find (all_paths.begin (), 
				all_paths.end(),prePath) ==
				all_paths.end())
			{
				
				int idx = all_paths.size ();
				all_paths.push_back (prePath);
				
				PathNode node;
				node.index = idx;
				node.cost = prePath.getCost ();
				
				m_derivation_paths[idx] = new_derivation;
				m_candidate_paths.insert (node);
				
			}
						
			//int vtx_cpath = all_paths[current_path][i-1];
			//rca::Link link (new_derivation,vtx_cpath,
							//network->getCost(new_derivation,vtx_cpath));
			
		}//endif
		int vtx_cpath = all_paths[current_path][i-1];
			rca::Link link (new_derivation,vtx_cpath,
							network->getCost(new_derivation,vtx_cpath));
		network->undoRemoveEdge (link);

	}//endfor
	
	network->clearRemovedEdges ();
	network->clearRemovedVertex (); //garantia
	
	return all_paths[current_path];
}

