#include <iostream>
#include <memory>


#include "properties.h"
#include "steinertree.h"
#include "breadth_search_tree.h"
#include <boost/heap/fibonacci_heap.hpp>

template<class T>
struct not_equal{
  bool operator() (const T& l1, const T& l2) const
  {
	  //return ((p < link.p) && (link != *this)) || (link != *this);
	  return (l1 > l2);
  }
};

//typedef typename std::pair<rca::Link, int> p_edge;
typedef typename boost::heap::compare<not_equal<rca::Link>> Comparator;
typedef typename boost::heap::fibonacci_heap<rca::Link, Comparator>::handle_type edge_handle;
typedef typename std::pair<bool, edge_handle> e_cell;

using namespace std;

int main (int argv, char **argc) {
	
	//method from properties to init the 
	//variables
	rca::init_properties (argc[1]);
    
	BeadthSearchTree tree_maker;  
	TreeAlgorithm algorithm(tree_maker);
  
	int NODES = rca::g_network->getNumberNodes();
	std::vector<std::vector<int>> congestion (NODES);
	for (int i=0; i < NODES; i++) {
		congestion[i] = std::vector<int> (NODES,0);
	}
  
	//iniciando manipulador de arestas
	//-------------------------------------------------------------------
	boost::heap::fibonacci_heap<rca::Link, Comparator> congestion_level;
	std::vector<std::vector< e_cell>> used_edges(NODES);
	for (int i=0; i < NODES; i++) {
		used_edges[i] = std::vector<e_cell>(NODES);
	}
  
	//used_edges[0][0].second = congestion_level.push (rca::Link(0,0,10));
	//used_edges[0][0].first = true;
	//used_edges[0][1].second = congestion_level.push (rca::Link(0,1,11));
	//used_edges[0][1].first = true;
  
	//if (used_edges[0][0].first == true) {
		//std::cout << (*((used_edges[0][0]).second)).getValue() << std::endl;
	//}
  
	//(*used_edges[0][1].second).setValue (5);
	//congestion_level.update (used_edges[0][1].second);
	//std::cout << congestion_level.top () << std::endl;
	//-------------------------------------------------------------------
  
	int level = 0;
	std::set<rca::Link, rca::not_equal_link<rca::Link>> edges_used;
	std::shared_ptr<SteinerTree> st;
	//O(G)
	for (unsigned int i=0; i < rca::g_groups.size(); i++) {
		
#ifdef MDEBUG
	cout << "__BUILDING__ST__:" << 	i << endl;
#endif
	
#ifdef MDEBUG
	auto begin = congestion_level.begin ();
	auto end = congestion_level.end ();
	
	int tmp = 0;
	for ( ; begin != end; begin++) {
					
		if (begin->getValue () > tmp) {
			tmp = begin->getValue ();
			cout << "Level: " << tmp << endl;  
		}				
	}
	
#endif
		int level = 1;
		while (!is_connected (*rca::g_network.get(), *rca::g_groups[i].get() )) {
      
			//std::cout << "undo Removing Level: " << level << endl;
      
			//iterator removendo de um level
			auto begin = congestion_level.begin ();
			auto end = congestion_level.end ();
	
			for ( ; begin != end; begin++) {
				//cout << *begin << ":" <<begin->getValue ();
				
				if (rca::g_network->isRemoved (*begin) && begin->getValue() == level) {
					//cout << " is removed\n";
					rca::g_network->undoRemoveEdge (*begin);  
				}
				
			}	
			level++;
			
		}
    
		//O(E+V)
		algorithm.execute (i,st);
		//char c = i;
		//std::string str = "../graph_"+ std::to_string(i)+".xdot";
		//cout << str << endl;
		//st->xdotToFile (str);
		//st->xdotFormat ();
      
		//O(E)
		Edge * e = st->listEdge.head;
		while (e != NULL) {
	
			rca::g_network->removeEdge (rca::Link(e->i,e->j, 0.0));
	
			//NÃO ESTÁ FUNCIONANDO!!!
			//TENHO QUE PROVER OUTRO MEIO PARA MANTER AS ARESTAS EM NÍVEIS
			rca::Link link (e->i, e->j, 0);
			int x = link.getX ();
			int y = link.getY ();
			if (used_edges[x][y].first == true) {
				//std::cout << (*((used_edges[x][y]).second)).getValue() << std::endl;
				int valor = (*((used_edges[x][y]).second)).getValue();
				(*((used_edges[x][y]).second)).setValue (valor+1);
				congestion_level.update ((used_edges[x][y]).second);
			} else {
				used_edges[x][y].first = true;
				link.setValue (1);
				used_edges[x][y].second = congestion_level.push (link);
			}
		
			e = e->next;
		}	
		e = NULL;
      
	}
    
	auto begin = congestion_level.ordered_begin ();
	auto end = congestion_level.ordered_end ();
		
	level = 0;
	for ( ; begin != end; begin++) {
		
		if (begin->getValue () > level) {
			level = begin->getValue ();
		}
	}
    
	cout << level << endl;
    
	return 0;
}