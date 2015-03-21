#include <iostream>

#include "network.h"
#include "group.h"
#include "algorithm.h"
#include "steinertree.h"

#include "acompp.h"

#include <boost/heap/fibonacci_heap.hpp>

namespace rca {

#ifndef _EDGE_CONTAINER_
#define _EDGE_CONTAINER_

//struct to compare two edges in fibonnaci heap
template<class T>
struct not_equal{
  bool operator() (const T& l1, const T& l2) const
  {
	  //return ((p < link.p) && (link != *this)) || (link != *this);
	  return (l1 > l2);
  }
};
	
//Matriz de inteiros
typedef typename std::vector<std::vector<int>> VMatrix;

//types used to control the heap_fibonnacti during algorithm execution
typedef typename boost::heap::compare<not_equal<rca::Link>> Comparator;
typedef typename boost::heap::fibonacci_heap<rca::Link, Comparator>::handle_type edge_handle;

typedef typename boost::heap::fibonacci_heap<rca::Link, Comparator> FibonnacciHeap;

//define um pair bool e um manipulador de aresta
//este manipulador indica se uma aresta está ou no heap
typedef typename std::pair<bool, edge_handle> HCell;

//vetor de manipuladores de arestas
typedef typename std::vector<std::vector< HCell >> EHandleMatrix;


/**
 * Esta classe é utilizada para manter informações sobre
 * o nível de utilização das arestas. 
 * 
 * Ela mantém um heap de fibonacci mínimo que é utilizado para
 * guardar as arestas utilizadas na construção de uma solução.
 * 
 * Quando for necessário utilizar uma arestas mais de uma vez
 * a aresta que está heap é adicionada novamente a rede.
 * 
 * @author Romerito Campos.
 */
class EdgeContainer {
	friend class AcoMPP;
	
public:
	void init_congestion_matrix (int nodes);
	
	void init_handle_matrix (int nodes);
	
	//used to prepare the nettwork to build the next tree
	//verify by connectivty between terminals
	int connected_level (rca::Group & group, rca::Network & Network);
	
	
	int connected_level_by_group (rca::Group & group, rca::Network &);
	
private:
	//this matrix holds the congestion
	VMatrix m_matrix;
	
	//this matrix holds the handles to heap elements
	EHandleMatrix m_ehandle_matrix;
	
	//this heap holds the edges form minimun congestion to max.
	FibonnacciHeap m_heap;

};

#endif

}

