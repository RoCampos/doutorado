#include "breadth_search_tree.h"

void BreadthSearchTree::make_tree (rca::Group & group, rca::Network & net,
				  std::shared_ptr<SteinerTree> & st) {
  
#ifdef DEBUG
	std::cout << "Running: BreadthSearchTree." << std::endl;
#endif
	
  typedef typename std::vector<int>::const_iterator c_iterator;
  
  //número de nós
  unsigned int NODES = net.getNumberNodes ();
  
  //alocação de memória para ST
  //st = make_shared<SteinerTree> (NODES);
  
  //acessando grupo
  //Group * group = rca::g_groups[group_id].get () ;
  
  //definindo nós de Steiner
  auto it_g = group.begin();
  auto end_g = group.end();
  for (; it_g != end_g; it_g++) {
      st->setTerminal (*it_g);
  }
  st->setTerminal ( group.getSource() );
  
  //iniciado a busca
  std::queue<int> _queue;  
  _queue.push(group.getSource ());
  
  //marcador de nós
  std::vector<bool> closed(NODES);
  closed[group.getSource ()] = true;
  
  //loop para achar todos os nós
  while ( !_queue.empty () ) {
    
    //que próximo nó a ser avaliado
    int node = _queue.front ();
    _queue.pop ();
    
    //iterator sobre os vizinho de "node"
    std::pair<c_iterator, c_iterator> _pair;
    net.get_iterator_adjacent (node, _pair);
    
    auto it = _pair.first;
    auto end = _pair.second;
    
    for (; it != end; it++) {      
		//se o nó *it não está fechado então entra na busca.
		rca::Link link (node, *it, 0.0);
		if (!closed[*it] && !net.isRemoved(link)) {
			double cost = net.getCost(node,*it);
			_queue.push (*(it));
			st->addEdge (node, (*it), cost);
			closed[ (*it) ] = true;  
	  
		} 
    }
    
  }
  
  //fazendo prunning da árvore de steiner
  st->prunning ();
  
}