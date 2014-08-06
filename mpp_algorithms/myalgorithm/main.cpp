#include <iostream>

#include "network.h"
#include "group.h"
#include "reader.h"

#include "mppalgorithm.h"

#include <memory>

int main (int argv, char **argc) {
  
  //reading informations about network and groups
  Reader r(argc[1]);
  
  std::shared_ptr<Network> network;
  std::vector<std::shared_ptr<rca::Group>> groups;
  
  r.configNetwork (network.get());
  groups = r.readerGroup ();
  
  
  
  MPPAlgorithm<TreeAlgorithmInterface> 
			myalgorithm (network, groups);
  
  return 0;
}