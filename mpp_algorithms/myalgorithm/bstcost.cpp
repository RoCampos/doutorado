#include "bstcost.h"

void BreadthSearchCost::make_tree (rca::Group& group, rca::Network & net, 
				  std::shared_ptr<SteinerTree> & st) {
	
#ifdef DEBUG
	std::cout << "BreadthSearchCost\n";
#endif
	
	priority_queue_edge queue;
	
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
			
#ifdef DEBUG
	cout << "Edge added: " << link << endl;
#endif
			
		}else if (!closed[link.getY()] && !net.isRemoved(link)) { //abrir o vértice Y caso contrário.
			
			//call to update_best_edge to update the best edges
			//for the next loop
			update_best_edge ( link.getY() , link.getValue(), queue, net);
			
			//closeing the node examined
			closed[link.getY()] = true;
			
			double value = net.getCost (link.getX(), link.getY());
			st->addEdge (link.getX(), link.getY(), value );
			
#ifdef DEBUG
	cout << "Edge added: " << link << endl;
#endif
			
		}
		
	} while (!queue.empty ());
	
	st->prunning ();
}

void BreadthSearchCost::update_best_edge (int node, 
										  double value, 
										  priority_queue_edge& queue,
										 rca::Network& net)
{
#ifdef DEBUG
	std::cout << "Updating the Queue of best edges." << std::endl;
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