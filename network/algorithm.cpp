#include "algorithm.h"
#include "heapfibonnaci.cpp"

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
		
		if (s == -1 || s >= network->getNumberNodes ())
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
