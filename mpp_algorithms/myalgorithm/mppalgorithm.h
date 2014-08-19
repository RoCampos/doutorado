#ifndef _MYCHENALGORITHM_
#define _MYCHENALGORITHM_

#include <iostream>
#include <string>
#include <iomanip>
#include <memory>

#include "network.h"
#include "reader.h"
#include "strategy_make_tree.h"
#include "breadth_search_tree.h"
#include <boost/heap/fibonacci_heap.hpp>

//struct to compare two edges in fibonnaci heap
template<class T>
struct not_equal{
  bool operator() (const T& l1, const T& l2) const
  {
	  //return ((p < link.p) && (link != *this)) || (link != *this);
	  return (l1 > l2);
  }
};

//Tipos Network e Vector de Groups
typedef typename std::vector<std::shared_ptr<rca::Group>> MulticatGroups;
typedef typename std::shared_ptr<rca::Network> RCANetwork;

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
 * Classe que representa meu algoritmo
 * Esta classe contém as rotinas utilizadas para resolver
 * o Multicast Packing Problem com um objetivo.
 * 
 * @author Romerito Campos.
 * @version 0.1 06/08/2014
 */
template<typename TreeStrategy>
class MPPAlgorithm {

public:
	MPPAlgorithm (const RCANetwork &, const MulticatGroups &);
	
	void init_strategy (TreeStrategy & strategy);
	
	void run ();

private:
	void init_congestion_matrix (VMatrix &m);
	
	void init_handle_matrix (EHandleMatrix &);

	int connected_level (int, FibonnacciHeap & fheap);
	
	void update_congestion (FibonnacciHeap&, EHandleMatrix&, 
							std::shared_ptr<SteinerTree> & st);
	
	double evaluate_cost (FibonnacciHeap&);
	
private:
	
	TreeStrategy * m_strategy;
	RCANetwork m_network;
	MulticatGroups m_groups;
	
	double m_congestion;
	double m_cost;
    
};

#endif