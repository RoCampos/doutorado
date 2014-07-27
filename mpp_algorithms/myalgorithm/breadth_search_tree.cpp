#include "breadth_search_tree.h"

void BeadthSearchTree::make_tree (int group_id,
				  std::shared_ptr<SteinerTree> & st) {
  
  unsigned int NODES = rca::m_network->getNumberNodes ();
  st = make_shared<SteinerTree> (NODES);
  
  Group * group = rca::m_groups[group_id].get () ;
  
  std::queue<int> _queue;  
  //_queue.enqueue();
  
}