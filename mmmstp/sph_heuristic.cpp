#include <iostream>
#include <stdio.h>

#include "reader.h"
#include "network.h"
#include "algorithm.h"
#include "steiner_tree_observer.h"
#include "edgecontainer.h"
#include "rcatime.h"
#include "mpp_visitor.h"

#include "sttree_local_search.h"

using namespace rca;

typedef std::vector<int> previous_t;
typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> CongestionHandle;
typedef typename rca::sttalgo::SteinerTreeObserver<CongestionHandle, STTree> STobserver;
typedef typename rca::sttalgo::ChenReplaceVisitor ChenReplaceVisitor;

typedef rca::sttalgo::cycle_local_search<CongestionHandle> CycleLocalSearch;

typedef std::tuple<int,int> Result;

previous_t shortest_path2 (int v, int w, rca::Network & network);

rca::Path get_path (int v, int w, rca::Network & network, previous_t & prev){
	
	int NODES = network.getNumberNodes ();
	
	rca::Path path;
	int pos = 0;
	double pathcost = 0.0;
	int s = w;
	while (s != v) {
		path.push (s);
		s = prev[s];
		
		if (s == -1 || s >= NODES)
		{
			rca::Path path2;
			return path2; //se não há caminho
		}
		
		
		pathcost += network.getCost (path[pos],s);
		
		pos++;

	}
	path.push (s);
	return path;
}

void remove_top_edges (CongestionHandle & ob, Network & m_network, Group & group, int res) {
	
	auto it = ob.get_heap ().ordered_begin ();
	auto end = ob.get_heap ().ordered_end ();
	
	for ( ; it != end; it++) {
 		if (it->getValue () <= ob.top()+res) {
			m_network.removeEdge (*it);
// 			if ( !is_connected (m_network, group) )
// 				m_network.undoRemoveEdge (*it);
 		} else {
			break;	
		}
	}
	
}

void intensification (std::vector<STTree> & steiner_trees, 
					  CongestionHandle & cg,
					  Network& m_network, 
					  std::vector<Group>& m_groups,
					  Result & result)
{
	
	int cost = 0;
	int res = 0;
 	std::tie(res, cost) = result;
	
	ChenReplaceVisitor chen(&steiner_trees);	
	chen.setEdgeContainer (cg);
	chen.setMulticastGroups (m_groups);
	chen.setNetwork (&m_network);
	
	double c = cost;
	chen.visitByCost (c);

	cost = c;
	CycleLocalSearch cls;
	cls.local_search (steiner_trees, m_network, 
					m_groups, cg, cost);
	
	res = cg.top ();
	
	result = std::make_tuple<int,int> ((int)res, (int)cost);
}

int main (int argc, char** argv) 
{
	
	rca::Network m_network;
	std::vector<std::shared_ptr<rca::Group>> g;
	std::vector<rca::Group> m_groups;
	std::string file(argv[1]);
	
	int res = atoi(argv[2]);
	int option = atoi(argv[3]);
	
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
 	
	rca::elapsed_time _time_;	
	_time_.started ();
	
	int ii=0;
	for (Group g : m_groups) {
				
		int source = g.getSource ();
		std::vector<int> members = g.getMembers();
		
		STTree steiner_tree(NODES, source, g.getMembers ());		
		ob.set_steiner_tree (steiner_tree, NODES);
		
 		if (is_connected (m_network, g)) {
 			remove_top_edges (cg, m_network, g, res);
			
			if (!is_connected (m_network, g))
				m_network.clearRemovedEdges ();
		} else {
			m_network.clearRemovedEdges ();
		}
		
		previous_t prev = all_shortest_path (source, members[0] ,m_network);
		
		for (int m : members) {
			rca::Path path = get_path (source, m, m_network, prev);
			
			auto rit = path.rbegin ();
			for (; rit != path.rend()-1; rit++) {
		
				int x = *rit;
				int y = *(rit+1);
				
				rca::Link l(x, y, 0);
				
				int cost = m_network.getCost (l.getX(), l.getY());
				
				ob.add_edge (l.getX(), l.getY(), cost, GROUP_SIZE);
			}			
		}
		
		steiner_trees.push_back (ob.get_steiner_tree());
		
		cost += ob.get_steiner_tree ().get_cost ();
	}
 	_time_.finished ();

	Result result( std::make_tuple<int,int>(cg.top(), (int)cost) );
	
	if (option == 1)
		intensification (steiner_trees, cg, m_network, m_groups, result);	
	
	_time_.finished ();
	
	std::cout << get<0>(result) << " " << get<1>(result);
	std::cout << " " << _time_.get_elapsed ()<< " \n";
	
	
	return 0;
}

previous_t shortest_path2 (int source, int w, rca::Network & network) {

#ifdef DEBUG1
	printf ("Call: %s\n", __FUNCTION__);
#endif
	
	typedef typename std::vector<int>::const_iterator c_iterator;
	
	int NODES = network.getNumberNodes ();
	double infty = std::numeric_limits<double>::infinity();
	
	std::vector<double> distance = std::vector<double>(NODES,infty);
	previous_t previous = previous_t (NODES,-1);
	std::vector<handle_t> handles = std::vector<handle_t> (NODES);
	
 	boost::heap::fibonacci_heap< vertex_t, boost::heap::compare<std::less<vertex_t>> > queue;
	
	distance[source] = 0;
	vertex_t v( distance[source]*-1, source );
	handles[source] = queue.push ( v );
	
	while (!queue.empty ()) {
	
		vertex_t v = queue.top ();
		queue.pop ();
// 		if (v.id == w) break;
		
		std::pair<c_iterator, c_iterator> neighbors;
		network.get_iterator_adjacent (v.id, neighbors);
		for (auto u=neighbors.first; u!= neighbors.second; u++) {
			
			double cost = network.getCost(v.id, *u);
			
			bool removed = network.isRemoved(rca::Link(v.id, *u, cost));
			
			if (distance[*u] > ((v.weight*-1 + cost)) 
				&& !removed) {
				double old_u_cost = distance[*u];
				distance[*u] = (v.weight*-1 + cost);
				previous[*u] = v.id;
				
				// TODO update heap. How to manipulate the handle
				vertex_t t(distance[*u]*-1, *u);
				
				if ( old_u_cost == infty ) {
					handles[ t.id ] = queue.push ( t );
				} else {
					queue.increase ( handles[*u], t );
				}
			}
		}
	}
	
	return previous;
	
}