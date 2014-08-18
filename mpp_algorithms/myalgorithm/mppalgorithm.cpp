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
#ifdef DEBUG
	cout << "Run Method" << endl;
#endif

	VMatrix congestion;
	init_congestion_matrix(congestion);
	
	
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
