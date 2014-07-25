#include "naiveshortestpath.h"

void NaiveShortestPath::make_tree (int group_id, std::shared_ptr<SteinerTree> & st) {
 
  std::vector<rca::Path> paths;    
  getPaths (group_id, paths);
  create_steiner_tree (group_id, paths, st);  
}

void NaiveShortestPath::getPaths (int group_id, std::vector<rca::Path> & paths) {
  
  //obtém um ponteiro para o group Group.
  rca::Group * group = rca::g_groups[group_id].get();
  
  //obtém os terminais do Grupo group_id
  std::vector<int> terminals = group->getMembers ();
  //addicionando fonte com terminal
  terminals.push_back ( group->getSource() );
  
  //Computando todos os caminhos entre os pares de terminais
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

  //obtém o número de nós entre os terminais
  unsigned int NODES = rca::g_network->getNumberNodes();
  
  //cria um smart_pointer do tipo SteinerTree
  st = std::make_shared<SteinerTree> (NODES);
  
  //obtém um ponteiro do tipo SteinerTree
  SteinerTree * st_tmp = st.get ();
  
  //criar vetor objetivo
  //std::vector<double> _objec(1);
  
  //criar vetor de elementos do tipo SteinerNode
  //std::vector<SteinerNode> _nodes(NODES, SteinerNode(0,0,false));
  //for (unsigned int i=0; i < NODES; i++) {
   // _nodes[0].setIndex (i);
  //}
  
  //adiciona as estruturas temporárias ao SteinerTree
  //st_tmp->setTempStructures (_objec,_nodes);
  
  //obtém ponteiro para o Grupo group_id
  Group *group = rca::g_groups[group_id].get();
  
  //Adiciona os terminais
  for (int i=0; i < group->getSize(); i++) {
    st_tmp->setTerminal (group->getMember (i));
  }
  st_tmp->setTerminal (group->getSource());
  
  //disjoint set used to avoid circles
  DisjointSet2 ds_edges (NODES);
  
  for (unsigned int i=0; i < paths.size (); i++) {
    //rca::Path path = paths[i];
    
    auto it = paths[i].begin ();
    auto end = paths[i].end ()-1;
    //for (unsigned int j=0; j < paths[i].size ()-1; j++) {
    for (; it != end; it++) {
      int v = *it;
      int w = *(it+1);
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