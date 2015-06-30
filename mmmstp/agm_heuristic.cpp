#include <iostream>
#include <stdio.h>

#include "reader.h"
#include "network.h"
#include "algorithm.h"
#include "steiner_tree_observer.h"
#include "edgecontainer.h"
#include "rcatime.h"

using namespace rca;

typedef std::vector<int> previous_t;
typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> CongestionHandle;
typedef typename rca::SteinerTreeObserver<CongestionHandle> STobserver;

void remove_top_edges (CongestionHandle & ob, Network & m_network, Group & group, int res) {
	
	auto it = ob.get_heap ().ordered_begin ();
	auto end = ob.get_heap ().ordered_end ();
	
	for ( ; it != end; it++) {
 		if (it->getValue () <= ob.top()+res) {
			m_network.removeEdge (*it);
 		} else {
			break;
		}
	}
}

void agm_heuristic (STobserver * ob, std::vector<rca::Link>&, 
					Network & m_network,int);

void create_list (std::vector<rca::Link>&, Network & m_network);

void update_usage (std::vector<rca::Link>&, STTree & st);

void print_solution (std::vector<STTree>& m_trees) {

	int i =0;
	for (auto st : m_trees) {
		edge_t *e = st.get_edges ().begin;
		while (e != NULL) {
		
			if (e->in)
				std::cerr <<  e->x+1 << " - " << e->y+1;
				std::cerr << ":" << i+1 << std::endl;			
			e = e->next;
		}
		i++;
	}	
	
}


void local_search (std::vector<STTree>&, 
				   int tree, 
				   rca::Network&, 
				   std::vector<rca::Group>&, 
				   CongestionHandle &cg);


std::vector<rca::Link> getCircle (std::vector<rca::Link>&, 
				rca::Group& group, 
				rca::Link&, 
				rca::Network & m_network);

int main (int argc, char** argv) 
{

	rca::Network m_network;
	std::vector<std::shared_ptr<rca::Group>> g;
	std::vector<rca::Group> m_groups;
	std::string file(argv[1]);
	
	int res = atoi(argv[2]);
	
	MultipleMulticastReader reader(file);
	reader.configure_unit_values (&m_network, g);
	for (std::shared_ptr<rca::Group> i : g) {
		m_groups.push_back (*i);
	}
	
	int NODES = m_network.getNumberNodes ();
	int GROUP_SIZE = m_groups.size ();
	
	CongestionHandle cg;
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);
	
	STobserver ob;
	ob.set_container (cg);
	
	std::vector<STTree> steiner_trees;
	double cost = 0.0;
 	
	std::vector<rca::Link> m_links;
	create_list (m_links, m_network);
	
	rca::elapsed_time time_elapsed;
	time_elapsed.started ();
	
	for (Group g : m_groups) {
	
		int source = g.getSource ();
		STTree steiner_tree(NODES, source, g.getMembers ());
		
		ob.set_steiner_tree (steiner_tree, NODES);
		
		agm_heuristic (&ob, m_links, m_network, GROUP_SIZE);
		
		ob.prune (1, GROUP_SIZE);
		steiner_trees.push_back (ob.get_steiner_tree ());
		
		cost += ob.get_steiner_tree ().getCost ();
		
		update_usage (m_links, ob.get_steiner_tree ());
	}
	
	//printing all steiner tree to evaluate corrections of results	
	cost = 0.0;
	for (int i=0; i < m_groups.size (); i++) {		
		improve_cost (steiner_trees, m_network, m_groups, cg, i);		
		cost += steiner_trees[i].getCost ();
	}
	time_elapsed.finished ();
	
	std::vector<int> idx(m_groups.size(), 0);
	std::iota(idx.begin(), idx.end(), 0);
	
	int improve;
	do {
		improve = cost;
		cost = 0;
		std::random_shuffle (idx.begin(), idx.end());
		for (int i: idx) {
			local_search (steiner_trees, i, m_network, 
 			   m_groups, cg);		
			cost += steiner_trees[i].getCost ();
		}

	}while (cost < improve);
	
	std::cout << cg.top () << " " << cost << " \n";
// 	print_solution (steiner_trees);
	return 0;
}

void update_usage (std::vector<rca::Link>& m_links, STTree & sttree)
{
	edge_t * e = sttree.get_edges ().begin;
	while (e != NULL) {
	
		if (e->in) { 
		
			rca::Link l(e->x, e->y,0);
			auto link = std::find(m_links.begin (), m_links.end(), l);
			link->setValue ( link->getValue () + 1);
			
		}
		
		e = e->next;
	}
	
	std::sort(m_links.begin(), m_links.end());	
}

void agm_heuristic (STobserver * ob, 
					std::vector<rca::Link>& m_links,
					Network & m_network, 
					int GROUPS_SIZE)
{
	std::vector<rca::Link> links = m_links;
		
	int pos = 0;
	//O(E)
	while (!links.empty()) {
		
		rca::Link link = links[pos];
						
		int cost = m_network.getCost (link.getX(), link.getY());
			
		if (m_network.isRemoved(link)){
			links.erase ( (links.begin () + pos) );
			continue;
		}
		
		ob->add_edge (link.getX(), link.getY(), cost, GROUPS_SIZE);
		links.erase ( (links.begin () + pos) );
	}
}

void local_search (std::vector<STTree>& m_trees,
				   int tree, 
				   rca::Network& m_network, 
				   std::vector<rca::Group>& m_groups, 
				   CongestionHandle &cg)
{
	
	int NODES = m_network.getNumberNodes ();
	int GSIZE = m_groups.size ();
	
	int tcost = m_trees[tree].getCost ();
	int old = tcost;
	
	std::set<int> vertex;	
	std::vector<rca::Link> m_links;
	
	edge_t * e = m_trees[tree].get_edge ();
	while (e != NULL) {
		if (e->in) {
			
			rca::Link link (e->x, e->y, 0);
			
			if (cg.value (link) == (GSIZE-1) ) {
				cg.erase (link);
			} else {
				int value = cg.value (link) + 1;
				cg.erase (link);
				link.setValue (value);
				cg.push (link);
			}
			
			vertex.insert (link.getX());
			vertex.insert (link.getY());
			
			m_links.push_back (link);
			
		}
		e = e->next;
	}
	
	for (auto v : vertex) {
		for (auto w: vertex) {
			
			if (v < w) {
				
				if (m_network.getCost (v,w) > 0) {
					
					
					rca::Link link (v,w, (int) m_network.getCost (v,w));
					
					if (cg.is_used(link)) {
						
						if (cg.value (link) <= cg.top ()) continue;
						
					}
					
					
					if (std::find(m_links.begin(), m_links.end(), link) 
						== m_links.end()) {
#ifdef DEBUG						
						std::string s('-', 60);
						std::cout << s << std::endl;
						
						std::cout << "Circle Induced by : " << link << std::endl;
#endif
						std::vector<rca::Link> toRemove;
						toRemove = getCircle (m_links, 
										m_groups[tree], 
										link, m_network);
						
						//adding the links an remove others
						Group g = m_groups[tree];
						for (auto l : toRemove) {
#ifdef DEBUG				
							std::cout <<"Removing "<<l << std::endl;
#endif				
							int source = g.getSource ();
							STTree steiner_tree(NODES, source, g.getMembers ());
							for (auto ll : m_links) {
							
								if (ll != l) {
									int cost = m_network.getCost (ll.getX(), 
																  ll.getY());
									steiner_tree.add_edge (ll.getX(), 
														   ll.getY(),
														   cost);
								}
								
							}
							int cost = m_network.getCost(link.getX(), link.getY());
							steiner_tree.add_edge (link.getX(), link.getY(), cost);
							
							steiner_tree.prunning ();
#ifdef DEBUG
							steiner_tree.xdotFormat ();
#endif
							
#ifdef DEBUG
							std::cout << tcost << " ";
							std::cout << steiner_tree.getCost () << std::endl;
#endif			
							if (tcost > steiner_tree.getCost ()) {
								m_trees[tree] = steiner_tree;
								tcost = steiner_tree.getCost ();
							}
#ifdef DEBUG							
 							getchar ();
#endif
						}
						
#ifdef DEBUG
						std::cout << s << std::endl;					
						std::cout << std::endl;
#endif
					}
					
				}
			}
			
		}
	}
	
#ifdef DEBUG
	std::cout << old <<" "<< m_trees[tree].getCost () << std::endl;
#endif
	
	e = m_trees[tree].get_edge ();
	while ( e != NULL) {
			
		if (e->in) {
			
			rca::Link link (e->x, e->y, 0);
			
			if (cg.is_used (link)) {
				
				int value = cg.value (link) - 1;
				
				cg.erase (link);
				link.setValue (value);
				cg.push (link);
				
			} else {
				link.setValue (GSIZE - 1); 
				cg.push (link);
			}
		}
		
		e = e->next;
	}
	
}

std::vector<rca::Link> getCircle (std::vector<rca::Link>& m_links, 
							rca::Group &group, rca::Link & link, 
							rca::Network & m_network)
{
	
#define WHITE 0
#define GREY 1
#define BLACK 2
	
	int x = link.getX();
	int y = link.getY();
	
	int NODES = m_network.getNumberNodes ();
	
	std::vector<int> predecessor(NODES, -1);
	std::vector<int> visited(NODES, 0);
	std::vector<int> color(NODES, WHITE);
	std::vector<int> time(NODES, 0);
	
	std::vector<int> stack;
	
	stack.push_back (x);
	
	int tempo = 0;
	
	while ( !stack.empty() ) {
		
		int current_node = stack[ stack.size() -1 ];
		
		if (current_node == y) break;
		
		tempo++;
		time[current_node] = tempo;
		color[current_node] = GREY;
		
		bool found = false;
		for (auto link : m_links) {
			
			int xx = link.getX();
			int yy = link.getY();
			if (xx == current_node && color[yy] == WHITE) {
			
				predecessor [yy] = current_node;
				stack.push_back (yy);
				found = true;
				break;
			} 
			
			if (yy == current_node && color[xx] == WHITE) {
			
				predecessor [xx] = current_node;
				stack.push_back (xx);
				found = true;
				break;
			} 
			
		}
		
		if (found) {
			continue;
		}
		
		color[current_node] = BLACK;
		tempo++;
		time[current_node] = tempo;
		
		stack.erase ( stack.begin() + (stack.size () - 1) );
		
	}
	
	std::vector<rca::Link> toRemove;
	
	int source = group.getSource ();
	for (int i=stack.size () - 1; i >=1; i--) {
		
		int x = stack[i];
		int y = stack[i-1];
		
		rca::Link l(x,y,0);
		
		if ( !group.isMember(x) && !group.isMember(y) && source != x && source != y)
			toRemove.push_back (l);
	}
	
	return toRemove;
	
}