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

std::vector<int> inefficient_widest_path (int v, int w, rca::Network * network)
{
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
		distance[i] = -9999999;
		if (i == v)
			elem = queue.insert (i,999999);
		closed[i] = false;
	}

	distance[v] = 999999;
	//queue.decreaseKey (elem,0);

	double cost = 0.0;
	bool removed = false;
	while (!queue.empty()) {

		int e = queue.minimum ()->data ();
		queue.removeMinimum ();

		closed[e] = true;

		//i is defined as register variable
		for (i=0; i < NODES; ++i) {

			
			cost = network->getBand (e, i);
			
			if (cost > 0.0) {
				
				removed = network->isRemoved(rca::Link(e, i, cost));
			
				int value = std::min (cost, distance[e]);
				
				if (!closed[i] && cost > 0
					&& (distance[i] < value)
					&& !network->isVertexRemoved(i) 
					&& !removed) 
				
				{
					distance[i] = value;
					prev[i] = e;
					queue.insert (i, -1*distance[i]);
				}
			}
			
		}
		
	}

	delete elem;

	return prev;
	
}

rca::Path shortest_path (int source, int w, rca::Network & network) {

#ifdef DEBUG1
	printf ("Call: %s\n", __FUNCTION__);
#endif
	
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
		if (v.id == w) break;
		
		std::pair<c_iterator, c_iterator> neighbors;
		network.get_iterator_adjacent (v.id, neighbors);
		for (auto u=neighbors.first; u!= neighbors.second; u++) {
			
			double cost = network.getCost(v.id, *u);
			
			bool removed = network.isRemoved(rca::Link(v.id, *u, cost));
			
			if (distance[*u] > ((v.weight*-1 + cost)) 
				&& !removed) {
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
	
	int size = p.size ();
	if (p[size-1] != source) {
		return rca::Path();
	}	
	
	return p;
	
}

void shortest_path (int source,
	rca::Network & network,
	std::vector<int> & bases,
	std::vector<int> & costpath, 
	std::vector<std::vector<int>> & paths)
{

	typedef typename std::vector<int>::const_iterator c_iterator;
	int NODES = network.getNumberNodes ();
	double infty = std::numeric_limits<double>::infinity();

	costpath = std::vector<int> (NODES);
	bases = std::vector<int> (NODES);	
	paths.resize (NODES);
	std::vector<double> distance = std::vector<double> (NODES, infty);
	std::vector<int> prev = std::vector<int> (NODES, -1);
	std::vector<bool> visited= std::vector<bool> (NODES, false);

	std::vector<handle_t> handles = std::vector<handle_t> (NODES);

	boost::heap::fibonacci_heap< vertex_t, boost::heap::compare<std::less<vertex_t>> > queue;

	distance[source] = 0;
	vertex_t v (distance[source]*-1, source);
	handles[source] = queue.push (v);

	while (!queue.empty ()) {

		vertex_t curr_node = queue.top ();
		queue.pop ();
		visited[curr_node.id] = true;

		//building extra informations
		if (curr_node.id != source) {
			
			//getting the previus node
			int pv = prev.at(curr_node.id);
			std::vector<int> & ref = paths.at (curr_node.id);
			for (int & n : paths.at(pv)) {				  
				ref.push_back (n);
			}

			ref.push_back (curr_node.id);

			//getting the cost of a path
			if (ref.size () > 1) {
				int length = ref.size ();
				int v = ref.at (length-1);
				int w = ref.at (length-2);
				int edge_cost = (int) network.getCost (v, w);
				costpath[curr_node.id] += edge_cost;
			}

			//updating the bases
			if (pv == source) {
				bases[curr_node.id] = curr_node.id;
			} else {
				bases[curr_node.id] = bases[pv];
			}

		}			

		// getting the neighboors of vertex
		std::pair<c_iterator, c_iterator> neighbors;
		network.get_iterator_adjacent (curr_node.id, neighbors);
		for (auto u=neighbors.first; u!= neighbors.second; u++) {

			int next = *u;

			// //distance between curr_node and neighboor 'u'
			// int band_curr_u = network.getBand(curr_node.id, next);
			// rca::Link l = rca::Link(curr_node.id, next, band_curr_u);
			double cost = network.getCost (curr_node.id, next);
			bool removed = network.isRemoved(rca::Link(curr_node.id, *u, cost));

			//if the link is valid then go on
			if (!removed) {				
				if (distance[*u] > ((curr_node.weight*-1 + cost)) 
					&& !removed) {
					double old_u_cost = distance[*u];
					distance[*u] = (curr_node.weight*-1 + cost);
					prev[*u] = curr_node.id;
					
					// TODO update heap. How to manipulate the handle
					vertex_t t(distance[*u]*-1, *u);
					
					if ( old_u_cost == infty ) {
						handles[ t.id ] = queue.push ( t );
					} else {
						queue.increase ( handles[*u], t );
					}
				}
			}//endif is_removed??
		}//end neighboor for
	}//end while loop 
}

void best_first_search (int src,
	rca::Network const & network, 
	std::vector<int> &bases, 
	std::vector<int> &costpath, 
	std::vector<std::vector<int>> & paths)
{

	typedef typename std::vector<int>::const_iterator c_iterator;

	int NODES = network.getNumberNodes ();
	costpath = std::vector<int> (NODES);
	bases = std::vector<int> (NODES);	
	paths.resize (NODES);
	std::vector<int> distance = std::vector<int> (NODES, 0);
	std::vector<int> prev = std::vector<int> (NODES);
	std::vector<bool> visited= std::vector<bool> (NODES, false);

	std::vector<int> fila;
	fila.push_back (src);

	paths[src].push_back (src);

	while (fila.size () > 0) {

		int curr_node = fila.at(0);

		if (curr_node != src) {

			//getting the previus node
			int pv = prev.at(curr_node);
			std::vector<int> & ref = paths.at (curr_node);
			for (int & n : paths.at(pv)) {				  
				ref.push_back (n);
			}

			ref.push_back (curr_node);

			//getting the cost of a path
			if (ref.size () > 1) {
				int length = ref.size ();
				int v = ref.at (length-1);
				int w = ref.at (length-2);
				int edge_cost = (int) network.getCost (v, w);
				costpath[curr_node] += edge_cost;
			}

			//updating the bases
			if (pv == src) {
				bases[curr_node] = curr_node;
			} else {
				bases[curr_node] = bases[pv];
			}

		}

		// getting the neighboors of vertex
		std::pair<c_iterator, c_iterator> neighbors;
		network.get_iterator_adjacent (curr_node, neighbors);
		for (auto u=neighbors.first; u!= neighbors.second; u++) {
			int next = *u;
			if (!visited[next]) {
				fila.push_back (next);
				prev[next] = curr_node;
				distance[next] += distance[curr_node]+1;
				visited[next] = true;
			}
		}

		fila.erase (fila.begin());
	}
}


void widest_shortest_path (
	int source,
	rca::Network & network,
	std::vector<int> & bases,
	std::vector<int> & costpath,
	std::vector<std::vector<int>> & paths)
{

	typedef typename std::vector<int>::const_iterator c_iterator;

	int NODES = network.getNumberNodes ();

	int infty = std::numeric_limits<int>::min();

	costpath = std::vector<int> (NODES,-1);
	bases = std::vector<int> (NODES,-1);
	paths = std::vector<std::vector<int>> (NODES);
	std::vector<int> distance = std::vector<int> (NODES, infty);
	std::vector<int> prev = std::vector<int> (NODES, -1);
	std::vector<bool> visited= std::vector<bool> (NODES, false);

	std::vector<g_handle_t> handles = std::vector<g_handle_t> (NODES);

	fibonacci_greater_t queue;

	distance[source] = std::numeric_limits<int>::max();

	vertex_t v (distance[source], source);
	handles[source] = queue.push (v);

	paths[source].push_back (source);

	while (!queue.empty ()) {

		vertex_t curr_node = queue.top ();
		queue.pop ();
		visited[curr_node.id] = true;	
		
		// building extra informations
		if (curr_node.id != source) {
			
			//getting the previus node
			int pv = prev[curr_node.id];
			for (auto n : paths[pv]) {
				paths[curr_node.id].push_back (n);
			}
			paths[curr_node.id].push_back (curr_node.id);

			//getting the cost of a path
			if (paths[curr_node.id].size () > 1) {
				int length = paths[curr_node.id].size ();
				int v = paths[curr_node.id][length-1];
				int w = paths[curr_node.id][length-2];
				int edge_cost = network.getCost (v, w);
				costpath[curr_node.id] += edge_cost;
			}

			//updating the bases
			if (pv == source) {
				bases[curr_node.id] = curr_node.id;
			} else {
				bases[curr_node.id] = bases[pv];
			}

		}			

		// getting the neighboors of vertex
		std::pair<c_iterator, c_iterator> neighbors;
		network.get_iterator_adjacent (curr_node.id, neighbors);
		for (auto u=neighbors.first; u!= neighbors.second; u++) {

			int next = *u;

			//distance between curr_node and neighboor 'u'
			int band_curr_u = network.getBand(curr_node.id, next);
			rca::Link l = rca::Link(curr_node.id, next, band_curr_u);
			bool removed = network.isRemoved(l);
			//if the link is valid then go on
			if (!removed) {

				//minimum distance between current link
				// and distance until current node
				int edge_value = std::min (band_curr_u, distance[curr_node.id] );

				if (!visited[next] && distance[next] < edge_value) {
					
					int next_old_dist = distance[next];

					distance[next] = edge_value;
					prev[next] = curr_node.id;
					vertex_t t(distance[next]*-1, next);

					if (next_old_dist == infty) {
						handles[t.id] = queue.push (t);
					} else {
						queue.decrease (handles[t.id], t);
					}
				}//endif compare distance
			}//endif is_removed??
		}//end neighboor for
	}//end while loop
}



rca::Path capacited_shortest_path (int v, int w, 
								   rca::Network *network,
								   CongestionHandle *cg,
								   rca::Group &g)
{

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
				
				rca::Link l (e,i,cost);
				removed = network->isRemoved(l);
			
				//getting the current capacity
				int cap;
				if (cg->is_used(l))
					cap = cg->value (l);
				else 
					cap = INT_MIN;
				
				int band = network->getBand (l.getX(), l.getY());
				//updating the capacity if the link is used
				if (cap == INT_MIN){		
					if (band - g.getTrequest() > 0) {
						cap = 0;
					}
				}else {
					cap -= g.getTrequest();
				}
				
				if (!closed[i] && cost > 0
					&& (distance[i] > (distance[e] + cost))
					&& !network->isVertexRemoved(i) 
					&& !removed
					&& cap >= 0) 
				
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
	  
	  if (count_terminals == terminals.size () ) return true;
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


rca::Path get_shortest_path (int v, int w, rca::Network & network, std::vector<int> & prev)
{
	int NODES = network.getNumberNodes ();
	
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
		
		
		pathcost += network.getCost (path[pos],s);
		
		pos++;

	}
	path.push (s);
	return path;
}

std::vector<int> all_shortest_path (int source, int w, rca::Network & network)
{
#ifdef DEBUG1
	printf ("Call: %s\n", __FUNCTION__);
#endif
	
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
			
			bool removed = network.isRemoved(rca::Link(v.id, *u, cost));
			
			if (distance[*u] > ((v.weight*-1 + cost)) 
				&& !removed) {
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
	
	return previous;

		
}

double min_bandwidth (rca::Network& network)
{

	int NODES = network.getNumberNodes ();
	double min = INT_MAX;
	for (int i = 0; i < NODES; i++) {
		for (int j = 0; j < i; j++) {
			
			int cost = network.getCost (i,j);
			if (cost > 0) {
			
				if (network.getBand (i,j) < min) {
					min = network.getBand (i,j);
				}
			}
		}
	}
	return min;
}

int get_bottleneck (
	rca::Network& network, 
	rca::Path& path)
{
	int min = std::numeric_limits<int>::max();

	if (path.size () >=2 ) {
		for (int i = 0; i < path.size ()-1; ++i)
		{
			int x = path[i];
			int y = path[i+1];
			int band = network.getBand (x, y);

			if (band < min) {
				min = band;
			}
		}
	}
	return min;
}

std::vector<rca::Link> path_to_edges (rca::Path const& path, rca::Network * net) 
{

	std::vector<rca::Link> out;
	for (size_t i = 0; i < path.size ()-1; ++i)
	{
		rca::Link l (path.at(i), path.at(i+1), 0);

		//if a network object is passed, 
		//the links has its cost updated
		if (net != NULL) {
			l.setValue ((int)net->getCost (l.getX(), l.getY()));
		}

		out.push_back (l);

	}
	return out;
}


void voronoi_diagram (
	rca::Network const & network,
	std::vector<int> & bases,
	std::vector<int> & costpath,
	std::vector<std::vector<int>> & paths) 
{
	typedef typename std::vector<int>::const_iterator c_iterator;

	int NODES = network.getNumberNodes ();
	int source = NODES-1;

	int infty = std::numeric_limits<int>::min();

	costpath = std::vector<int> (NODES,-1);
	bases = std::vector<int> (NODES,-1);
	// paths = std::vector<std::vector<int>> (NODES);
	paths.resize (NODES);
	std::vector<int> distance = std::vector<int> (NODES, infty);
	std::vector<int> prev = std::vector<int> (NODES, -1);
	std::vector<bool> visited= std::vector<bool> (NODES, false);

	std::vector<g_handle_t> handles = std::vector<g_handle_t> (NODES);

	fibonacci_greater_t queue;

	distance[source] = std::numeric_limits<int>::max();

	vertex_t v (distance[source], source);
	handles[source] = queue.push (v);

	while (!queue.empty ()) {

		vertex_t curr_node = queue.top ();
		queue.pop ();
		visited[curr_node.id] = true;

		//building extra informations
		if (curr_node.id != source) {
			
			//getting the previus node
			int pv = prev.at(curr_node.id);
			std::vector<int> & ref = paths.at (curr_node.id);
			for (int & n : paths.at(pv)) {				  
				ref.push_back (n);
			}

			ref.push_back (curr_node.id);

			//getting the cost of a path
			if (ref.size () > 1) {
				int length = ref.size ();
				int v = ref.at (length-1);
				int w = ref.at (length-2);
				int edge_cost = (int) network.getCost (v, w);
				costpath[curr_node.id] += edge_cost;
			}

			//updating the bases
			if (pv == source) {
				bases[curr_node.id] = curr_node.id;
			} else {
				bases[curr_node.id] = bases[pv];
			}

		}			

		// getting the neighboors of vertex
		std::pair<c_iterator, c_iterator> neighbors;
		network.get_iterator_adjacent (curr_node.id, neighbors);
		for (auto u=neighbors.first; u!= neighbors.second; u++) {

			int next = *u;

			//distance between curr_node and neighboor 'u'
			int band_curr_u = network.getBand(curr_node.id, next);
			rca::Link l = rca::Link(curr_node.id, next, band_curr_u);
			bool removed = network.isRemoved(l);

			//if the link is valid then go on
			if (!removed) {

				//minimum distance between current link
				// and distance until current node
				int edge_value = std::min (band_curr_u, distance[curr_node.id] );
				if (!visited[next] && distance[next] < edge_value) {

					int next_old_dist = distance[next];

					distance[next] = edge_value;
					prev[next] = curr_node.id;
					vertex_t t(distance[next]*-1, next);

					if (next_old_dist == infty) {
						handles[t.id] = queue.push (t);
					} else {
						queue.decrease (handles[t.id], t);
					}
				}//endif compare distance
			}//endif is_removed??
		}//end neighboor for

	}//end while loop 

}

void widest_shortest_path (
	int source,
	rca::Network & network,
	std::vector<int> & bases,
	std::vector<int> & costpath,
	std::vector<int> & prev,
	std::vector<std::vector<int>> & paths) 
{
	typedef typename std::vector<int>::const_iterator c_iterator;

	int NODES = network.getNumberNodes ();

	int infty = std::numeric_limits<int>::min();

	costpath = std::vector<int> (NODES,-1);
	bases = std::vector<int> (NODES,-1);
	paths = std::vector<std::vector<int>> (NODES);
	std::vector<int> distance = std::vector<int> (NODES, infty);
	prev = std::vector<int> (NODES, -1);
	std::vector<bool> visited= std::vector<bool> (NODES, false);

	std::vector<g_handle_t> handles = std::vector<g_handle_t> (NODES);

	fibonacci_greater_t queue;

	distance[source] = std::numeric_limits<int>::max();

	vertex_t v (distance[source], source);
	handles[source] = queue.push (v);

	while (!queue.empty ()) {

		vertex_t curr_node = queue.top ();
		queue.pop ();
		visited[curr_node.id] = true;	
		
		//building extra informations
		// if (curr_node.id != source) {
			
		// 	//getting the previus node
		// 	int pv = prev[curr_node.id];
		// 	for (auto n : paths[pv]) {
		// 		paths[curr_node.id].push_back (n);
		// 	}
		// 	paths[curr_node.id].push_back (curr_node.id);

		// 	//getting the cost of a path
		// 	if (paths[curr_node.id].size () > 1) {
		// 		int length = paths[curr_node.id].size ();
		// 		int v = paths[curr_node.id][length-1];
		// 		int w = paths[curr_node.id][length-2];
		// 		int edge_cost = network.getCost (v, w);
		// 		costpath[curr_node.id] += edge_cost;
		// 	}

		// 	//updating the bases
		// 	if (pv == source) {
		// 		bases[curr_node.id] = curr_node.id;
		// 	} else {
		// 		bases[curr_node.id] = bases[pv];
		// 	}

		// }			

		// getting the neighboors of vertex
		std::pair<c_iterator, c_iterator> neighbors;
		network.get_iterator_adjacent (curr_node.id, neighbors);
		for (auto u=neighbors.first; u!= neighbors.second; u++) {

			int next = *u;

			//distance between curr_node and neighboor 'u'
			int band_curr_u = network.getBand(curr_node.id, next);
			rca::Link l = rca::Link(curr_node.id, next, band_curr_u);
			bool removed = network.isRemoved(l);
			//if the link is valid then go on
			if (!removed) {

				//minimum distance between current link
				// and distance until current node
				int edge_value = std::min (band_curr_u, distance[curr_node.id] );

				if (!visited[next] && distance[next] < edge_value) {
					
					int next_old_dist = distance[next];

					distance[next] = edge_value;
					prev[next] = curr_node.id;
					vertex_t t(distance[next]*-1, next);

					if (next_old_dist == infty) {
						handles[t.id] = queue.push (t);
					} else {
						queue.decrease (handles[t.id], t);
					}
				}//endif compare distance
			}//endif is_removed??
		}//end neighboor for
	}//end while loop

}