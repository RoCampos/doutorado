#include "edgecontainer.h"

using namespace rca;

void EdgeContainer::init_congestion_matrix (int nodes)
{
	m_matrix = VMatrix(nodes);
	for (int i=0; i < nodes; i++) {
		m_matrix[i] = std::vector<int> (nodes,0);
	}
}
	
void EdgeContainer::init_handle_matrix (int nodes)
{
	m_ehandle_matrix = EHandleMatrix(nodes);
	for (int i=0; i < nodes; i++) {
		m_ehandle_matrix[i] = std::vector<HCell>(nodes);
	}

}

int EdgeContainer::connected_level (rca::Group & group, rca::Network & network) {
	
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
	
	return level; 
		
}
