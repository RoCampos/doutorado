#include <iostream>

#include "network.h"
#include "reader.h"
#include "path.h"
#include "algorithm.h"

using namespace std;
using namespace rca;

void print_terminals (Group & group);
std::vector<Path> getPaths (Group & group, Network & net);
void print_paths (std::vector<Path> & paths);


int main (int argv, char**argc) {

    rca::Network network;
    std::vector<shared_ptr<Group>> groups;
  
    //reading informations about network and groups
    Reader r(argc[1]);
    r.configNetwork (&network);
    groups = r.readerGroup ();
    
    //printing the terminals
    print_terminals (*groups[0]);

    //printing the paths: terminals to terminals
    std::vector<Path> paths = getPaths (*groups[0], network);
    print_paths (paths);
    
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
