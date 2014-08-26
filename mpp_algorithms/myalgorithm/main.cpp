#include <iostream>

#include "network.h"
#include "group.h"
#include "reader.h"

#include "mppalgorithm.h"
#include "mppalgorithm.cpp"
#include <memory>

#ifdef BSTTREE
#include "breadth_search_tree.h"
#endif

#ifdef BSTCOST
#include "bstcost.h"
#endif

#ifdef BSTMEAN
#include "bstmean.h"
#endif

int main (int argv, char **argc) {
  
	//reading informations about network and groups
	Reader r(argc[1]);
	
	std::shared_ptr<rca::Network> network = make_shared<rca::Network>();
	std::vector<std::shared_ptr<rca::Group>> groups;
  
	r.configNetwork (network.get());
	groups = r.readerGroup ();

#ifdef BSTTREE
	BreadthSearchTree bst;
#endif 
	
#ifdef BSTCOST
	BreadthSearchCost bst;
#endif
	
#ifdef BSTMEAN
	BreadthSearchMean bst;
#endif

	MPPAlgorithm<TreeAlgorithmInterface> 
			myalgorithm (network, groups);
			
	myalgorithm.init_strategy (bst);
	myalgorithm.run ();
			
  return 0;
}