#include "properties.h"

std::shared_ptr<rca::Network> g_network;
std::vector<std::shared_ptr<rca::Group>> g_groups;

void init (std::string file) {
 
  //reading informations about network and groups
  Reader r(file);
  
  g_network = std::make_shared<Network> ();
  
  r.configNetwork (g_network.get());
  g_groups = r.readerGroup();
  
}