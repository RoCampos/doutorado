#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include <vector>
#include <stack>

#include "path.h"
#include "network.h"
#include "group.h"
#include "heapfibonnaci.h"
#include <boost/heap/fibonacci_heap.hpp>
#include <boost/heap/binomial_heap.hpp>
#include "edgecontainer.h"


typedef struct vertex_t {
	
	int weight;
	int id;
	bool used;
	
	vertex_t (int _w, int _id) : weight(_w), id(_id){}
	
	bool operator < (const vertex_t & ref) const{
		return weight < ref.weight;
	}
	
	bool operator > (const vertex_t & ref) const{
		return weight > ref.weight;
	}
	
}vertex_t;

typedef typename boost::heap::fibonacci_heap<vertex_t,boost::heap::compare<std::less<vertex_t>>>::handle_type handle_t;
typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> CongestionHandle;


rca::Path shortest_path (int v, int w, rca::Network * network);

rca::Path shortest_path (int v, int w, rca::Network & network);

std::vector<int> inefficient_widest_path (int v, int w, rca::Network * network);

rca::Path capacited_shortest_path (int v, int w, 
								   rca::Network *network,
								   CongestionHandle *cg,
								   rca::Group &g);

bool is_connected (rca::Network & network, rca::Group & group);

/**
 * This method is used to build the path from v to w after
 * apply shortest_path algorithm.
 * 
 */
rca::Path get_shortest_path (int v, int w, rca::Network & network, std::vector<int> & prev);

#endif