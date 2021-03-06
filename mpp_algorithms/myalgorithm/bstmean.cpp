#include "bstmean.h"

void BreadthSearchMean::make_tree (rca::Group& group, rca::Network & net, 
				  std::shared_ptr<SteinerTree> & st) {
	
#ifdef DEBUG
	std::cout << __FILE__<< ":" <<__FUNCTION__ <<":"<< __LINE__ << std::endl;	
#endif
	
	priority_queue_edge queue;
	
	//------------------EXPERIMENTAL ---------------
	
	std::vector<rca::Link> links;
	remove_edges_by_mean(net, group, links);
	//----------------------------------------/
	
	//número de nós
	unsigned int NODES = net.getNumberNodes ();
	
	//definindo nós de Steiner
	auto it_g = group.begin();
	auto end_g = group.end();
	for (; it_g != end_g; it_g++) {
		st->setTerminal (*it_g);
	}
	st->setTerminal ( group.getSource() );
	
	//marcador de nós
	std::vector<bool> closed(NODES);
	
	//nó fonte marcado como visitado.
	int node = group.getSource ();
	closed[node] = true;
	
	//adding the first node of the search
	update_best_edge ( node , 0, queue, net);
	
	do {
		
		rca::Link link (queue.top ());
		queue.pop ();
		
		//se o vértice x não está fechado então
		//deve-se abrílo
		if (!closed[link.getX()] && !net.isRemoved(link) ) {
			
			//call to update_best_edge to update the best edges
			//for the next loop
			update_best_edge ( link.getX() , link.getValue(), queue, net);
			
			//closeing the node examined
			closed[link.getX()] = true;
			
			double value = net.getCost (link.getX(), link.getY());
			st->addEdge (link.getX(), link.getY(), value );
			
			
		}else if (!closed[link.getY()] && !net.isRemoved(link)) { //abrir o vértice Y caso contrário.
			
			//call to update_best_edge to update the best edges
			//for the next loop
			update_best_edge ( link.getY() , link.getValue(), queue, net);
			
			//closeing the node examined
			closed[link.getY()] = true;
			
			double value = net.getCost (link.getX(), link.getY());
			st->addEdge (link.getX(), link.getY(), value );
			
		}
		
	} while (!queue.empty ());
	
	st->prunning ();
	
	
	//removendo nós da lista de custos
	auto it = links.begin ();
	auto end = links.end ();
	
#ifdef DEBUG
	std::cout << "Size of Links: " << links.size () << std::endl;
#endif
	for (; it != end; it++) {
		net.undoRemoveEdge (*it);
	}
	
#ifdef DEBUG
	std::cout << "Network:removed edges after undo remove edges by mean\n";
	net.showRemovedEdges ();
#endif
	
}

void BreadthSearchMean::update_best_edge (int node, 
										  double value, 
										  priority_queue_edge& queue,
										 rca::Network& net)
{
#ifdef DEBUG
	std::cout << __FILE__<< ":" <<__FUNCTION__ <<":"<< __LINE__ << std::endl;
#endif
	
	//iterator sobre os vizinho de "node"
    std::pair<c_iterator, c_iterator> _pair;

	net.get_iterator_adjacent (node, _pair);
	
	//adding the first edges to start the search
	for ( ; _pair.first != _pair.second; _pair.first++) {
	
		double cost = net.getCost (node, (*_pair.first) );
		rca::Link link(node, (*_pair.first) , cost + value);
		
		queue.push (link);
		
	}
	
}

void BreadthSearchMean::remove_edges_by_mean (rca::Network & net, 
											  rca::Group &gp,
											  std::vector<rca::Link> & links)
{
	
#ifdef DEBUG
	std::cout << __FILE__<< ":" <<__FUNCTION__ <<":"<< __LINE__ << std::endl;
#endif
	unsigned i = 0;
	
	//getting the maximun and minimun edges
	double max = std::numeric_limits<double>::min ();
	double min = std::numeric_limits<double>::max ();
	for (; i < net.getNumberNodes (); i++) {
			
		for (unsigned j = 0; j < i; j++) {
		
			double cost = net.getCost (i,j);
			if (cost > 0) {
				
				if (cost > max) {
					max = cost;
				} 
				
				if (cost < min) {
					min = cost;
				}
			}
			
		}
	}

	//calculating the mean
	double mean =  ((max + min)/2)* m_mean_modificator;
#ifdef DEBUG
	std::cout << "\tMax: " << max << std::endl;
	std::cout << "\tMin: " << min << std::endl;
	std::cout << std::fixed << std::endl;
	std::cout << "\tMean: " << std::setprecision(2) << mean << std::endl;
	
	std::cout << "\tRemoving edges!\n";
#endif
	
	//removing the edges that has values upon mean
	i=0;
	for (; i < net.getNumberNodes (); i++) {
			
		for (unsigned j = 0; j < i; j++) {
		
			double cost = net.getCost (i,j);
			if (cost > mean) {
				
				rca::Link link (i,j,net.getCost (i,j));
				
				if (!net.isRemoved(link)) {
					//net.removeEdge (link);
					//if (net.isRemoved(link)) {
#ifdef DEBUG
	std::cout << "\t"<<link << std::endl;
#endif
						
						links.push_back (link);
					//}
				}
				
				
			}
			
		}
	}
	
#ifdef DEBUG
	std::cout << "\tNumber of removed edges: " << links.size ()<<std::endl;
#endif
	
	std::sort (links.begin (), links.end(), std::greater<rca::Link>());
	for (int i=0; i < links.size ()/2; i++) {
		net.removeEdge (links[i]);
	}
	
	
	//testing if the graph is connected
	if (!is_connected (net, gp)) {
#ifdef DEBUG
	std::cout << "\tNot Connected after remotions" << std::endl;	
#endif
		auto it = links.begin ();
		auto end = links.end ();
		
		for (; it != end; it++) {
			net.undoRemoveEdge (*it);
		}
	}
	
}