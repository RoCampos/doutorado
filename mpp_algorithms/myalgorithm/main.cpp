#include <iostream>

#include "mppalgorithm.h"

int main (int argv, char **argc) {

  MPPAlgorithm mpp("MPathPacking", "MPacking", 1, argc[1]);
  
  rca::Network network;
  Reader r(argc[1]);
  r.configNetwork (&network);
  
  //initialization of the algorithm
  mpp.init (network);
  
  return 0;
}