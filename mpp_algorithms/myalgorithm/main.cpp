#include <iostream>

#include "network.h"
#include "group.h"
#include "reader.h"

#include <memory>

int main (int argv, char **argc) {
  
  rca::Network network;
  std::vector<std::shared_ptr<rca::Group>> groups;
  
  //reading informations about network and groups
  Reader r(argc[1]);
  r.configNetwork (&network);
  groups = r.readerGroup ();
  
  for (int i=0;i < 1000; i++) {
    groups[0]->getMembers();
  }
  
  return 0;
}