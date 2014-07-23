#include "naiveshortestpath.h"

void NaiveShortestPath::make_tree (std::shared_ptr<SteinerTree> & st) {
 
  int GROUP_SIZE = g_groups.size();
  for (int i=0; i < GROUP_SIZE; i++) {
      
    std::vector<rca::Path> paths;
      
    getPaths (i, paths);
      
  }
  
}

void NaiveShortestPath::getPaths (int group_id, std::vector<rca::Path> & paths) {
  
  rca::Group * group = m_groups[group_id].get();
  
  std::vector<int> terminals = group->getMembers ();
  //addicionando fonte com terminal
  terminals.push_back ( group->getSource() );
  
  for (unsigned int i=0; i < terminals.size () -1; i++) {
    int v = terminals[i];
    int w = terminals[i+1];
    rca::Path path = shortest_path (v, w, &m_net);
    
    paths.push_back (path);
    
  }
  
}