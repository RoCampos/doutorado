#include "mppalgorithm.h"

using namespace rca;
using namespace std;

template <typename TreeStrategy>
MPPAlgorithm<TreeStrategy>::MPPAlgorithm (const RCANetwork & net,const MulticatGroups & vgroup) 
{

	m_network = net;
	m_groups = std::move(vgroup);
	
}

template <typename TreeStrategy> 
void MPPAlgorithm<TreeStrategy>::init_strategy (const TreeStrategy & strategy)
{
	m_strategy = strategy;
}

template <typename TreeStrategy>
void MPPAlgorithm<TreeStrategy>::run ()
{
	typedef unsigned int uint;
	
#ifdef DEBUG
	cout << "Run Method" << endl;
#endif
	

	VMatrix congestion;
	EHandleMatrix ehandles;
	init_congestion_matrix (congestion);
	init_handle_matrix (ehandles);
	
	//heap that stores the edges by level of usage
	boost::heap::fibonacci_heap<rca::Link, Comparator> congestion_level;

	int level = 0;
	for (uint i = 0; i < m_groups.size (); i++) {
		
	}	
	
}

template <typename TreeStrategy>
void MPPAlgorithm<TreeStrategy>::init_congestion_matrix (VMatrix &matrix) {
	int NODES = m_network->getNumberNodes ();
	VMatrix congestion (NODES);
	for (int i=0; i < NODES; i++) {
		congestion[i] = std::vector<int> (NODES,0);
	}
	matrix = std::move(congestion);
}

template <typename TreeStrategy>
void MPPAlgorithm<TreeStrategy>::init_handle_matrix (EHandleMatrix &matrix) {
	
	int NODES = m_network->getNumberNodes ();
	EHandleMatrix used_edges(NODES);
	for (int i=0; i < NODES; i++) {
		used_edges[i] = std::vector<HCell>(NODES);
	}

	matrix = std::move(used_edges);	
}

template <typename TreeStrategy>
void MPPAlgorithm<TreeStrategy>::connected_levle (FibonnacciHeap & fheap) {

	
	
}


