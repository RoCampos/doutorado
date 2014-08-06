#include "mppalgorithm.h"

using namespace rca;
using namespace std;

template <typename TreeStrategy>
MPPAlgorithm<TreeStrategy>::MPPAlgorithm (RCANetwork & net, MulticatGroups & vgroup) 
{

	m_network = net;
	m_groups = std::move(vgroup);
	
}

template <typename TreeStrategy> 
void MPPAlgorithm<TreeStrategy>::init_strategy (const TreeStrategy & strategy)
{
	m_strategy(strategy);
}

template <typename TreeStrategy>
void MPPAlgorithm<TreeStrategy>::run ()
{

	cout << "Printig a message" << endl;
	
}
