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

void improve_cost (std::vector<STTree>& m_trees, 
				   Network & network, 
				   std::vector<rca::Group>& m_groups, 
				   CongestionHandle & cg, int);

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
	
	time_elapsed.finished ();
	
	std::cout << cg.top () << " " << cost << " ";
	std::cout << time_elapsed.get_elapsed () <<std::endl;
	
	//printing all steiner tree to evaluate corrections of results
	for (int i=0; i < m_groups.size (); i++) {
		improve_cost (steiner_trees, m_network, m_groups, cg, i);
	}
	print_solution (steiner_trees);
	
	
	return 0;
}

void create_list (std::vector<rca::Link>& m_links, Network & m_network)
{
	auto iter = m_network.getLinks ().begin();
	auto end = m_network.getLinks ().end();
	for (; iter != end; iter++) {
		m_links.push_back (*iter);
	}
		
	for (auto & it: m_links) {
		it.setValue (0);
	}
	
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

void improve_cost (std::vector<STTree>& m_trees, 
				   Network & network,
				   std::vector<rca::Group>& m_groups, 
				   CongestionHandle& cg, int best)
{

	int GSIZE = m_groups.size ();
	edge_t * e = m_trees[best].get_edge ();
	while (e != NULL) {
		
		if (e->in) {
			//updating usage
			
			rca::Link l (e->x, e->y,0);
			if (cg.value (l) == (GSIZE-1) ) {
				cg.erase (l);
			} else {
				int value = cg.value (l) + 1;
				cg.erase (l);
				l.setValue (value);
				cg.push (l);
			}
			
		}
		
		e = e->next;
	}
	
	network.clearRemovedEdges ();
	
	remove_top_edges<CongestionHandle> (cg, network, m_groups[best], 0);
	
	int  NODES = network.getNumberNodes ();
	int source = m_groups[best].getSource ();
	STTree steiner_tree(NODES, source, m_groups[best].getMembers ());
		
	STobserver ob;
	ob.set_container (cg);
	ob.set_steiner_tree (steiner_tree, NODES);

	std::vector<rca::Link> m_links;
	create_list (m_links, network);
	for (auto &	 l : m_links) {
		int cost = network.getCost (l.getX(), l.getY());
		l.setValue (cost);
	}
	
	std::sort (m_links.begin(), m_links.end());
	
	auto it = m_links.rbegin ();
	auto end = m_links.rend();
	
	for ( ; it != end; it++) {
		network.removeEdge (*it);
 		if ( !is_connected (network, m_groups[best]) )
			network.undoRemoveEdge (*it);
	}
	
	while (!m_links.empty()) {
	
		rca::Link l = m_links[0];
		
		if (network.isRemoved(l)) {
		
			m_links.erase (m_links.begin());
			continue;
			
		}
		
		ob.add_edge (l.getX(), l.getY(), l.getValue(), GSIZE);
		m_links.erase ( m_links.begin () );
		
	}
	
	ob.prune (1, GSIZE);
	
	m_trees[best] = ob.get_steiner_tree();
}