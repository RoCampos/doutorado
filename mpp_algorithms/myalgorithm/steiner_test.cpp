#include <iostream>

#include "network.h"
#include "reader.h"
#include "path.h"
#include "algorithm.h"
#include "steinertree.h"

using namespace std;
using namespace rca;

void print_terminals (Group & group);
std::vector<Path> getPaths (Group & group, Network & net);
void print_paths (std::vector<Path> & paths);

SteinerTree create_steiner_tree (std::vector<Path> & paths, 
				 Network & net, Group & group);

int main (int argv, char**argc) {

    rca::Network network;
    std::vector<shared_ptr<Group>> groups;
  
    //reading informations about network and groups
    Reader r(argc[1]);
    r.configNetwork (&network);
    groups = r.readerGroup ();
    
    //printing the terminals
    //print_terminals (*groups[0]);

    //printing the paths: terminals to terminals
    for (int i=0; i < groups.size(); i++) {
      std::vector<Path> paths = getPaths (*groups[i], network);
      //print_paths (paths);
    
      SteinerTree st = create_steiner_tree (paths, network, *groups[i]); 
      cout << "Cost After Prunning: " << st.getCost() << endl;
      st.xdotFormat ();  
    }
    
    //Edge *begin = st.listEdge.head;
    //double cost = 0.0;
    /*while (begin != NULL) {
	cost += begin->cost;
	begin = begin->next;
    }
    */
      
    return 0;
}

void print_terminals (Group & group) {
  
    cout << "Terminals:";
    cout << group.getSource () << " "; 
    for (int i = 0; i < group.getSize(); i++) {
	cout << group.getMember(i) << " ";
    }
    cout << endl;
  
}

std::vector<Path> getPaths (Group & group, Network & net) {
 
  std::vector<int> terminals = group.getMembers ();
  //addicionando fonte com terminal
  terminals.push_back ( group.getSource() );
  
  std::vector<Path> paths;
  
  for (unsigned int i=0; i < terminals.size () -1; i++) {
    int v = terminals[i];
    int w = terminals[i+1];
    Path path = shortest_path (v, w, &net);
    
    paths.push_back (path);
    
  }
  return paths;
}

void print_paths (std::vector<Path> & paths) {
  
  cout << "Paths Terminal to Terminal\n";
  for (auto it = paths.begin (); it != paths.end (); it++) {
      cout << *it << endl;
  }
  
}

SteinerTree create_steiner_tree (std::vector<Path> & paths, 
				 Network & net, Group & group) {
  
  cout << "Building Steiner Tree for Group: " << group.getId () << endl; 
  
  std::vector<double> objs(1);
  
  unsigned int NODES = net.getNumberNodes();
  
  std::vector<SteinerNode> _nodes(NODES, 
				  SteinerNode(0,0,false));
  
  for (unsigned int i=0; i < NODES; i++) {
    _nodes[0].setIndex (i);
  }
  
  SteinerTree st(1, NODES );
  st.setTempStructures (objs,_nodes);
  
  for (int i=0; i < group.getSize(); i++) {
    st.setTerminal (group.getMember (i));
  }
  st.setTerminal (group.getSource());
  
  //disjoint set used to avoid circles
  DisjointSet2 ds_edges (NODES);
  
  for (int i=0; i < paths.size (); i++) {
    Path path = paths[i];
    
    for (int i=0; i < path.size ()-1; i++) {
      int v = path[i];
      int w = path[i+1];
      Link link (v,w, net.getCost(v,w));
      
      //testing if two vertex are in the same disjointSet
      if (ds_edges.find(v) != ds_edges.find(w)) {
	
	st.addEdge (link.getX(), link.getY(), link.getValue() );
	ds_edges.simpleUnion (v,w);
      }
    }    
  }
  
  cout << "Cost Before Prunning: " << st.getCost () << endl;
  st.prunning ();
  
  return st;
}
