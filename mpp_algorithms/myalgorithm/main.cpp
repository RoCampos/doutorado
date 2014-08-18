#include <iostream>

#include "network.h"
#include "group.h"
#include "reader.h"

#include "mppalgorithm.h"
#include "mppalgorithm.cpp"
#include "breadth_search_tree.h"
#include "breadth_search_tree.cpp"

#include <memory>

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
  
	myalgorithm.run ();
			
  return 0;
}