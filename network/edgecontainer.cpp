#include "edgecontainer.h"

using namespace rca;

template <class Comp, class Handle>
void EdgeContainer<Comp,Handle>::init_congestion_matrix (int nodes)
{
	
	m_matrix = VMatrix(nodes);
	for (int i=0; i < nodes; i++) {
		m_matrix[i] = std::vector<int> (nodes,0);
	}
}
	
template <class Comp, class Handle>
void EdgeContainer<Comp,Handle>::init_handle_matrix (int nodes)
{
	m_ehandle_matrix = std::vector<std::vector< Handle >>(nodes);
	for (int i=0; i < nodes; i++) {
		m_ehandle_matrix[i] = std::vector<Handle>(nodes);
	}

}

template <class Comp, class Handle>
int EdgeContainer<Comp,Handle>::connected_level (rca::Group & group, rca::Network & network) {
	
#ifdef DEBUG1
	std::cout << __FUNCTION__ << ": ";
#endif
	
	int level = 1;
	while ( !is_connected (network, group) ) {
     
		//iterator removendo de um level
		auto begin = m_heap.begin ();
		auto end = m_heap.end ();
		for ( ; begin != end; begin++) {
			
			if ( network.isRemoved (*begin) && (begin->getValue() == level) ) {
				//cout << " is removed\n";
				network.undoRemoveEdge (*begin);
			}	
			
		}	
		
		level++;			
	}	
#ifdef DEBUG1
	if (level > 1) {
		std::cout << "Executed\n";
	}else {
		std::cout << std::endl;
	}
#endif
	
	return level; 
		
}

template <class Comp, class Handle>
int EdgeContainer<Comp,Handle>::connected_level_by_group (rca::Group & group, 
											 rca::Network &network)
{
#ifdef DEBUG1
std::cout << __FUNCTION__ << ": ";
#endif
	int level = 1;//group.getSize ();
	while ( !is_connected (network, group) ) {
     
		//iterator removendo de um level
		auto begin = m_heap.ordered_begin ();
		auto end = m_heap.ordered_end ();
		for ( ; begin != end; begin++) {
			if ( network.isRemoved (*begin) && (begin->getValue() - level >= 0 ) ) {
				//cout << " is removed\n";
				network.undoRemoveEdge (*begin);
			}	
			
		}	
		//network.clearRemovedEdges ();
		level--;
	}	
#ifdef DEBUG1
	if (level > 1) {
		std::cout << "Executed\n";
	}else {
		std::cout << std::endl;
	}
#endif
	
	
	
	return level; 
}

template class rca::EdgeContainer<Comparator, HCell>;
template class rca::EdgeContainer<ComparatorReverse, HCellRev>;