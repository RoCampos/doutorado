#include <iostream>

#include "mppalgorithm.h"

int main (int argv, char **argc) {

  MPPAlgorithm mpp("MPathPacking", "MPacking", 1, argc[1]);
  
  rca::Network network;
  std::vector<shared_ptr<Group>> groups;
  
  //reading informations about network and groups
  Reader r(argc[1]);
  r.configNetwork (&network);
  groups = r.readerGroup ();
  
  //initialization of the algorithm
  mpp.init (network, groups);
  
  return 0;
}