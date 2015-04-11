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

// typedef struct greater {
// 
// 	bool operator () (const vertex_t& v1, const vertex_t& v2) const
// 	{
// 		return v1.weight < v2.weight;
// 	}
// 	
// }g;

typedef typename boost::heap::fibonacci_heap<vertex_t>::handle_type handle_t;
//typedef typename boost::heap::binomial_heap<vertex_t>::handle_type handle_t;

rca::Path shortest_path (int v, int w, rca::Network * network);

rca::Path shortest_path (int v, int w, rca::Network & network);

bool is_connected (rca::Network & network, rca::Group & group);

//explit instantiation
//template rca::Path shortest_path<0> (int v, int w, rca::Network * network);

#endif