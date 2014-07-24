#include "naiveshortestpath.h"

void NaiveShortestPath::make_tree (int group_id, std::shared_ptr<SteinerTree> & st) {
 
  std::vector<rca::Path> paths;    
  getPaths (group_id, paths);
  create_steiner_tree (group_id, paths, st);  
}

void NaiveShortestPath::getPaths (int group_id, std::vector<rca::Path> & paths) {
  
  rca::Group * group = rca::g_groups[group_id].get();
  
  std::vector<int> terminals = group->getMembers ();
  //addicionando fonte com terminal
  terminals.push_back ( group->getSource() );
  
  for (unsigned int i=0; i < terminals.size () -1; i++) {
    int v = terminals[i];
    int w = terminals[i+1];
    rca::Path path = shortest_path (v, w, rca::g_network.get() );    
    paths.push_back (path);    
  }
  
}

void NaiveShortestPath::create_steiner_tree  (int group_id, 
				       std::vector<rca::Path> & paths, 
				       std::shared_ptr<SteinerTree> & st) {
  
  unsigned int NODES = rca::g_network->getNumberNodes();
  st = std::make_shared<SteinerTree> (1,NODES);
  
  SteinerTree * st_tmp = st.get ();
  
  std::vector<double> _objec(1);
  std::vector<SteinerNode> _nodes(NODES, SteinerNode(0,0,false));
  for (unsigned int i=0; i < NODES; i++) {
    _nodes[0].setIndex (i);
  }
  st_tmp->setTempStructures (_objec,_nodes);
  
  Group *group = rca::g_groups[group_id].get();
  
  for (unsigned int i=0; i < group->getSize(); i++) {
    st_tmp->setTerminal (group->getMember (i));
  }
  st_tmp->setTerminal (group->getSource());
  
  //disjoint set used to avoid circles
  DisjointSet2 ds_edges (NODES);
  
  for (unsigned int i=0; i < paths.size (); i++) {
    rca::Path path = paths[i];
    
    for (unsigned int i=0; i < path.size ()-1; i++) {
      int v = path[i];
      int w = path[i+1];
      rca::Link link (v,w, rca::g_network->getCost(v,w));
      
      //testing if two vertex are in the same disjointSet
      if (ds_edges.find(v) != ds_edges.find(w)) {
	
	st_tmp->addEdge (link.getX(), link.getY(), link.getValue() );
	
	ds_edges.simpleUnion (v,w);
      }
    }    
  }
  
  //cout << "Cost Before Prunning: " << st.getCost () << endl;
  st_tmp->prunning ();
  
}