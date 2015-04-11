#include "algorithm.h"

rca::Path shortest_path(int v, int w, rca::Network * network) {
	typedef FibonacciHeapNode<int,double> Element; //todo VErificar se é double ou int 1

	int NODES = network->getNumberNodes();

	vector< int > prev = vector< int >( NODES );
	vector< double >distance = vector< double >( NODES ); //acho que tinha um erro aqui
	vector< bool > closed = vector< bool >( NODES );

	FibonacciHeap < int, double > queue; // todo VErificar se é double ou int 2
	Element* elem = NULL;
	int register i;
	for (i=0; i < NODES; i++) {
		prev[i] = -1;
		distance[i] = 9999999;
		if (i == v)
			elem = queue.insert (i,9999999);
		closed[i] = false;
	}

	distance[v] = 0;
	queue.decreaseKey (elem,0);

	double cost = 0.0;
	bool removed = false;
	while (!queue.empty()) {

		int e = queue.minimum ()->data ();
		queue.removeMinimum ();

		closed[e] = true;

		//i is defined as register variable
		for (i=0; i < NODES; ++i) {

			
			cost = network->getCost (e, i);
			
			if (cost > 0.0) {
				
				removed = network->isRemoved(rca::Link(e, i, cost));
			
				if (!closed[i] && cost > 0
					&& (distance[i] > (distance[e] + cost))
					&& !network->isVertexRemoved(i) 
					&& !removed) 
				
				{
					distance[i] = distance[e] + cost;
					prev[i] = e;
					queue.insert (i, distance[i]);
				}
			}
			
		}
		
	}

	delete elem;

	rca::Path path;
	int pos = 0;
	double pathcost = 0.0;
	int s = w;
	while (s != v) {
		path.push (s);
		s = prev[s];
		
		if (s == -1 || s >= NODES)
		{
			rca::Path path2;
			return path2; //se não há caminho
		}
		
		
		pathcost += network->getCost (path[pos],s);
		
		pos++;

	}
	path.push (s);
	path.setCost (pathcost); //definindo o custo
	
	return path;
}

rca::Path shortest_path (int source, int w, rca::Network & network) {

	typedef typename std::vector<int>::const_iterator c_iterator;
	
	int NODES = network.getNumberNodes ();
	double infty = std::numeric_limits<double>::infinity();
	
	std::vector<double> distance = std::vector<double>(NODES,infty);
	std::vector<int> previous = std::vector<int> (NODES,-1);
	std::vector<handle_t> handles = std::vector<handle_t> (NODES);
	
 	boost::heap::fibonacci_heap< vertex_t, boost::heap::compare<std::less<vertex_t>> > queue;
	
	distance[source] = 0;
	vertex_t v( distance[source]*-1, source );
	handles[source] = queue.push ( v );
	
	while (!queue.empty ()) {
	
		vertex_t v = queue.top ();
		queue.pop ();
		
		std::pair<c_iterator, c_iterator> neighbors;
		network.get_iterator_adjacent (v.id, neighbors);
		for (auto u=neighbors.first; u!= neighbors.second; u++) {
			
			double cost = network.getCost(v.id, *u);
			if (distance[*u] > ((v.weight*-1 + cost)) ) {
				double old_u_cost = distance[*u];
				distance[*u] = (v.weight*-1 + cost);
				previous[*u] = v.id;
				
				// TODO update heap. How to manipulate the handle
				vertex_t t(distance[*u]*-1, *u);
				
				if ( old_u_cost == infty ) {
					handles[ t.id ] = queue.push ( t );
				} else {
					queue.increase ( handles[*u], t );
				}
			}
		}
	}
	
	rca::Path p;
	int u = w;
	for (; u != -1; u = previous[u]) {
		p.push (u);
	}	
	
	return p;
	
}

bool is_connected (rca::Network & network, rca::Group & group)
{
  
typedef typename std::vector<int>::const_iterator c_iterator;
#define WHITE 1
#define GREY 2
#define BLACK 3

  std::vector<int> terminals = group.getMembers ();
  terminals.push_back ( group.getSource () );
  
  std::vector<int> nodes ( network.getNumberNodes () , 1);
  
  int count_terminals = 0;
  
  std::stack<int> pilha;
  
  pilha.push (group.getSource ());
  
  while ( !pilha.empty () ) {
    
    int current_node = pilha.top ();
    pilha.pop ();
    nodes[current_node] = GREY;
    
    if (group.isMember (current_node) || group.getSource () == current_node) {
      count_terminals++;
    }
    
    std::pair<c_iterator, c_iterator> iterators;
    network.get_iterator_adjacent (current_node, iterators);
    
    for (; iterators.first != iterators.second; iterators.first++) {
     
      int adjacent = *iterators.first;
      
      rca::Link l(current_node, adjacent, 0);
      if ( !network.isRemoved(l) && nodes[adjacent] == WHITE) {
	
		nodes[adjacent] = GREY;
		pilha.push (adjacent);
	
      }
      
    }
    
  }
  
  //std::cout << count_terminals << std::endl;
  return (count_terminals == terminals.size () );
}
