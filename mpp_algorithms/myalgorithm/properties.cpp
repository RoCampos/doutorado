#include "properties.h"

namespace rca {

  
std::shared_ptr<rca::Network> g_network;
std::vector<std::shared_ptr<rca::Group>> g_groups;

void init_properties (std::string file) {
 
  //reading informations about network and groups
  Reader r(file);
  
  rca::g_network = std::make_shared<Network> ();
  
  r.configNetwork (rca::g_network.get());
  rca::g_groups = r.readerGroup();
  
};

}