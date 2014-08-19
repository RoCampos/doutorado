#include "mppalgorithm.h"
#include "rcatime.h"

using namespace rca;
using namespace std;

template <typename TreeStrategy>
MPPAlgorithm<TreeStrategy>::MPPAlgorithm (const RCANetwork & net,const MulticatGroups & vgroup) 
{

	m_network = net;
	m_groups = std::move(vgroup);
	
}

template <typename TreeStrategy> 
void MPPAlgorithm<TreeStrategy>::init_strategy (TreeStrategy & strategy)
{
	m_strategy = &strategy;
}

template <typename TreeStrategy>
void MPPAlgorithm<TreeStrategy>::run ()
{
	typedef unsigned int uint;
	
#ifdef DEBUG1
	cout << "Run Method" << endl;
#endif
	
	VMatrix congestion;
	EHandleMatrix ehandles;
	init_congestion_matrix (congestion);
	init_handle_matrix (ehandles);
	
	rca::time::elapsed_time time;
		
	//heap that stores the edges by level of usage
	FibonnacciHeap heap;
	//std::shared_ptr<SteinerTree> s_tree;
	int NODES = m_network->getNumberNodes ();
	
	//storing the trees for post-processing
	std::vector<std::shared_ptr<SteinerTree>> vtrees; 
	
	time.started ();
	for (uint id = 0; id < m_groups.size (); id++) 
	{
		
		//prepare the network to build next steiner tree
		connected_level(id, heap);
		vtrees.push_back( std::make_shared<SteinerTree> (NODES) );
		//s_tree = std::make_shared<SteinerTree> (NODES);
		m_strategy->make_tree ( *m_groups[id].get(),
							   *m_network.get(),
								vtrees[id]);
		//						s_tree)
							  

		//update_congestion (heap, ehandles, s_tree);		
		update_congestion (heap, ehandles, vtrees[id]);
		
#ifdef DEBUG1
		cout << id << endl;
		cout << *m_groups[id] << endl;
		//s_tree->xdotFormat ();
#endif
		
	}
	time.finished ();
	
#ifdef DEBUG
	
	for(uint i = 0; i < vtrees.size (); i++) {
		std::cout << "adsf\n";
		vtrees[i]->xdotFormat ();
	}
	
#endif
	
	std::cout << evaluate_cost (heap) << " ";
	std::cout << time.get_elapsed () << std::endl;
	
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
int MPPAlgorithm<TreeStrategy>::connected_level (int group_id, FibonnacciHeap & fheap) {

	int level = 1;
	while (!is_connected (*m_network.get(), *m_groups[group_id] )) {
     
		//iterator removendo de um level
		auto begin = fheap.begin ();
		auto end = fheap.end ();
			for ( ; begin != end; begin++) {
			//cout << *begin << ":" <<begin->getValue ();
			
			if (m_network->isRemoved (*begin) && begin->getValue() == level) {
				//cout << " is removed\n";
				m_network->undoRemoveEdge (*begin);  
			}			
		}	
		level++;			
	}	
	return level;
}

template <typename TreeStrategy>
void MPPAlgorithm<TreeStrategy>::update_congestion (FibonnacciHeap& heap, 
													EHandleMatrix& ehandles,
													std::shared_ptr<SteinerTree> & st)
{

	Edge * e = st->listEdge.head;
	while (e != NULL) {
	
		m_network->removeEdge (rca::Link(e->i,e->j, 0.0));
	
		//NÃO ESTÁ FUNCIONANDO!!!
		//TENHO QUE PROVER OUTRO MEIO PARA MANTER AS ARESTAS EM NÍVEIS
		rca::Link link (e->i, e->j, 0);
		int x = link.getX ();
		int y = link.getY ();
		if (ehandles[x][y].first == true) {
			//std::cout << (*((used_edges[x][y]).second)).getValue() << std::endl;
			int valor = (*((ehandles[x][y]).second)).getValue();
			(*((ehandles[x][y]).second)).setValue (valor+1);
			heap.update ((ehandles[x][y]).second);
		} else {
			ehandles[x][y].first = true;
			link.setValue (1);
			ehandles[x][y].second = heap.push (link);
		}
		
		e = e->next;
	}	
	e = NULL;
	
}

template <typename TreeStrategy>
double MPPAlgorithm<TreeStrategy>:: evaluate_cost (FibonnacciHeap & heap)
{
	
	auto begin = heap.ordered_begin ();
	auto end = heap.ordered_end ();
	
	double cost = 0.0;
	while (begin != end) {
		cost += m_network->getCost (begin->getX(), begin->getY());
		begin++;
	}
	
	return cost;
}



