#include <iostream>

#include "network.h"
#include "group.h"
#include "reader.h"

#include "mppalgorithm.h"
#include "mppalgorithm.cpp"
#include <memory>
#include "breadth_search_tree.h"

int main (int argv, char **argc) {
  
	//reading informations about network and groups
	Reader r(argc[1]);
	
	std::shared_ptr<rca::Network> network = make_shared<rca::Network>();
	std::vector<std::shared_ptr<rca::Group>> groups;
  
	r.configNetwork (network.get());
	groups = r.readerGroup ();

	BreadthSearchTree bst;
	
	MPPAlgorithm<TreeAlgorithmInterface> 
			myalgorithm (network, groups);
  
	myalgorithm.init_strategy (bst);
	myalgorithm.run ();
			
  return 0;
}