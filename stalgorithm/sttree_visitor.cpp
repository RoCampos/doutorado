#include "sttree_visitor.h"

using namespace rca::sttalgo;
using namespace rca;

template<class Container, class SteinerType, class NetworkType>
void rca::sttalgo::prunning (SteinerType & st, Container & cont, int trequest, int band, NetworkType & net)
{

	rca::sttalgo::prunning(st, cont, trequest, band, net);

}

void rca::sttalgo::prunning (STTree & st, 
	rca::EdgeContainer<rca::Comparator, rca::HCell> & cont, 
	int trequest, 
	int band, 
	rca::Network & net) {
	
	list_leafs_t& list = st.get_leafs (); 
	leaf_t * aux = list.begin;

	while ( aux != NULL) {
		
		leaf_t * tmp = aux->next;
		
		int id = aux->id;
		st.get_node(id).decrease_degree ();
		
		if (st.get_node(id).get_degree () == 0) {
			
			edge_t * edge = st.get_node(id).remove_adjacent_vertex ();
			
			int _other = (edge->x == id ? edge->y : edge->x);
			
			//removing leaf
			list.remove ( st.get_node(id).leaf );
			
			//printf ("Removing (%d - %d)\n",edge->x, edge->y);
			//remove aresta
			if (edge != NULL) {
				edge->in = false;
				double m_cost = st.get_cost () - edge->value;
				st.set_cost (m_cost);
				
				rca::Link l (edge->x, edge->y, -1);
				if (cont.is_used (l)){
					l.setValue ( cont.value (l));
				} else {
					std::cout << l <<":"<<l.getValue() << " not removed\n";
				}
				
				int _band = net.getBand (l.getX(), l.getY());
				if (l.getValue () + trequest == _band) {
					cont.erase (l);
				} else if (l.getValue() > -1){
					cont.erase (l);
					l.setValue ( l.getValue () + trequest );
					cont.push  (l);
				}
			}
			
			edge = NULL;
			
			//considera o outro vertex
			st.get_node(_other).decrease_degree ();
			if (st.get_node(_other).get_degree () == 1 && !st.get_node(_other).is_terminal ()) {
				
				leaf_t *leaf = new leaf_t ( _other );
				list.add (leaf);
				
				st.get_node(_other).add_leaf_node (leaf);
				
				aux = list.first ();
				
				continue;
			}
			
		}
		
		aux = tmp;
		tmp = NULL;
	}
	
	aux = NULL;
}

void rca::sttalgo::prunning (steiner & st, 
	rca::EdgeContainer<rca::Comparator, rca::HCell> & cont, 
	int trequest, 
	int band, 
	rca::Network & net )
{
	
	Prune prun;
	prun.prunning (st);

	int cost = 0;
	for (auto e = prun.begin(); e != prun.end(); e++) {
		cost += (int) net.getCost (e->first, e->second);

		// updating the congestion of link
		rca::Link l (e->first, e->second, 0);
		int _band = net.getBand (l.getX(), l.getY());
		if ( cont.is_used (l) ) {
			if (cont.value (l) + trequest == _band) {
				cont.erase (l);
			} else {
				int value = cont.value (l);
				cont.erase (l);
				l.setValue ( value + trequest );
				cont.push  (l);
			}
		}
	}
	st.set_cost (st.get_cost () - cost);

}

template<class NetworkType, class SteinerType>
void rca::sttalgo::make_prunning (NetworkType& network, SteinerType& tree) {
	rca::sttalgo::make_prunning (network, tree);
}

void rca::sttalgo::make_prunning (rca::Network & network, steiner& tree) {
	Prune prun;
	prun.prunning (tree);
	int cost = 0;
	for (auto e = prun.begin(); e != prun.end(); e++) {
		cost += (int) network.getCost (e->first, e->second);
	}
	tree.set_cost (tree.get_cost () - cost);
}

void rca::sttalgo::make_prunning (rca::Network & network, STTree & tree) {
	tree.prunning ();
}

std::vector<rca::Path> rca::sttalgo::stree_to_path (STTree & st, int source, int nodes)
{
// 	st.xdotFormat ();
	std::vector<rca::Path> paths;
	
	std::vector<int> stack;
	std::vector<int> visited = std::vector<int>(nodes,0);
	visited[source] = 1;
	
	stack.push_back (source);
	int curr_node = stack[0];
	while (!stack.empty ()) {
		
		bool terminal = false;
	
		//getting next
		edge_t *e = st.get_edges().begin;
		while (e != NULL) {
		
			int next = -1;
			if (e->x == curr_node && visited[e->y] == 0) {
				stack.push_back (e->y);
				next = e->y;
			} else if (e->y == curr_node && visited[e->x] == 0) {
				stack.push_back (e->x);
				next = e->x;
			}
			
			if (next != -1) {
			
				if (st.get_node (next).terminal) {
					
					//printing the path
					rca::Path path;
					auto rit = stack.crbegin ();
					for (; rit != stack.crend(); rit++) {
						path.push (*rit);
					}
					paths.push_back (path);
					//std::cout << path << std::endl;

				} 
				visited[next] = 1;
				
				curr_node = next;
				e = st.get_edges().begin;
				continue;
			} 	
			
			e = e->next;
		}
		
		stack.pop_back ();
		
		if (stack.empty()) break;
		curr_node = stack[ stack.size () - 1];
		
	}
	
	return paths;
}

std::vector<int> rca::sttalgo::make_cut_visitor (std::vector<rca::Link>& st, 
			   int source, 
			   rca::Link & link,
			   std::vector<int> & mark, 
			   int nodes)
{
	
#ifdef DEBUG
	printf ("BEGIN ---- %s ---- BEGIN\n",__FUNCTION__);
#endif 
	
	//all vertex of the cut reachable from source
	std::vector<int> tree_nodes;
	
	//used in the depth search first
	std::vector<int> stack;
	std::vector<int> visited = std::vector<int>(nodes,0);
	
	//mark source as visited
	visited[source] = 1;
	
	//init the stack
	stack.push_back (source);
	
	while (!stack.empty ()) {

		//getting the current node to process
		int curr_node = stack[ stack.size () - 1];
		
		//erasing the curr_node from stack
		stack.pop_back ();
		
		//mark curr_node as visited
		mark[curr_node] = source;
		
		//look for neighbor of curr_node
		for (rca::Link const& e : st){
			
			if (link == e) continue;
				
			//test if x ou y is a neighbor of curr_node
			//and add it to the stack
			if (e.getX() == curr_node && visited[e.getY()] == 0) {
				stack.push_back (e.getY());
				visited[e.getY()] = 1;
// 				break;
			} else if (e.getY() == curr_node && visited[e.getX()] == 0) {
				stack.push_back (e.getX());
				visited[e.getX()] = 1;
// 				break;
			}
		}
		
		//add curr_node to the vertex set
		tree_nodes.push_back (curr_node);		
	}
	
#ifdef DEBUG
	printf ("END ---- %s  ---- END\n",__FUNCTION__);
#endif 	
	
	return tree_nodes;
}

template<class Container>
void rca::sttalgo::remove_top_edges (Container & ob, rca::Network & m_network,
					   rca::Group & group, int res) 
{
	
	auto it = ob.get_heap ().ordered_begin ();
	auto end = ob.get_heap ().ordered_end ();

	int top = -1;
	if (ob.m_heap.size () > 0)
		top = ob.top ();
	
	for ( ; it != end; it++) {
 		if (it->getValue () <= top+res) {
			m_network.removeEdge (*it);
 			if ( !is_connected (m_network, group) )
 				m_network.undoRemoveEdge (*it);
 		} else {
			break;
		}
	}
	
}

std::vector<rca::Link> rca::sttalgo::sttreeToVector (STTree & st) {
	std::vector<rca::Link> links;
	edge_t *e = st.get_edge();
	while (e != NULL) {
		if (e->in) {
			rca::Link link (e->x, e->y, 0);
			links.push_back (link);
		}
		e = e->next;
	}
	return links;
}

void rca::sttalgo::replace_edge (STTree & st, 
									rca::Link &_old, 
									rca::Link & _new, 
									rca::Network& net)
{

	std::vector<rca::Link> links = sttreeToVector (st);
	
	auto res = std::find (std::begin(links), std::end(links), _old);
	res->setX(_new.getX());
	res->setY(_new.getY());
	
	STTree new_st (st.get_num_nodes(), st.get_source (), st.get_terminals());
	for (auto l : links) {
		int cost = net.getCost (l.getX(), l.getY());
		new_st.add_edge (l.getX(), l.getY(), cost);
	}
	
	st = new_st;
	st.prunning ();
}

void cost_by_usage (std::vector<rca::Link> & m_links, 
					std::vector<STTree> & m_trees, 
					rca::Network & network)
{
	
	for (auto st : m_trees) {
	
		edge_t * e = st.get_edge ();
		while ( e != NULL) {

			if (e->in) {
				
				rca::Link link (e->x, e->y, 0);
				auto res = std::find(m_links.begin(), m_links.end(), link);
				int cost = network.getCost (link.getX(), link.getY());
				if (res == m_links.end()) {
					
					link.setValue (cost);
					m_links.push_back (link);
				} else if (res != m_links.end()) {
					
					int value = res->getValue() + cost;
					res->setValue (value);
					
				}
				
			}
			e = e->next;
		}
		
	}
	
	std::sort (m_links.begin(), m_links.end(), std::greater<rca::Link>());
	
}

void create_list (std::vector<rca::Link>& m_links, rca::Network & m_network)
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

void rca::sttalgo::improve_cost (std::vector<STTree>& m_trees, 
	rca::Network & network,
	std::vector<rca::Group>& m_groups, 
	rca::EdgeContainer<rca::Comparator, rca::HCell> & cg, int best)
{
	typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> CongestionHandle;
	typedef typename rca::sttalgo::SteinerTreeObserver<CongestionHandle, STTree> STobserver;
	
	network.clearRemovedEdges ();
	
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
	
	//removendo arestas congestionadas
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
		
		if (!network.isRemoved(*it)) {
			network.removeEdge (*it);
		
			if ( !is_connected (network, m_groups[best]) )
				network.undoRemoveEdge (*it);
		}
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

template<class SteinerType>
void rca::sttalgo::print_solution (std::vector<SteinerType>& trees)
{

	for (auto steiner_tree : trees) {
		steiner_tree.xdot_format ();
	}
	
}

template<class SteinerType>
void rca::sttalgo::print_solution2 (std::vector<SteinerType>& trees)
{
	int i=0;
	for (auto st : trees) {
		
		// edge_t * e = st.get_edge();
		// while (e != NULL) {
		// 	if (e->in) {
		for (auto &e : st.get_all_edges ()) {
				
				rca::Link l(e.first+1, e.second+1,0);
				std::cerr << l.getX()<< " - " <<l.getY() << ":" << i+1 <<";"<< std::endl;				
			// }
			// e = e->next;
		}
		i++;
	}
}

template<class SteinerType, class Container, class NetworkType>
std::vector<rca::Link> rca::sttalgo::get_available_links (SteinerType & tree, 
						  Container& cg, 
						  NetworkType& network, 
						  rca::Group& group,
						  rca::Link& old)
{
	
// 	tree.xdotFormat ();
	std::vector<rca::Link> backup_links;
	
	int NODES = network.getNumberNodes ();
	
	//O(E)
	std::vector<rca::Link> links;
	edge_t * e = tree.get_edge();
	while (e!=NULL) {
		if (e->in) {
			rca::Link link (e->x, e->y, 0);
			links.push_back (link);
		}
		e = e->next;
	}
	
	int x = old.getX(); //source of t1
	int y = old.getY(); //source of t2
	
	if (std::find ( std::begin(links), std::end(links), old ) == 
		std::end(links)) 
	{ 
		return backup_links;
	}else {
		
		std::vector<int> nodes_x(NODES, -1);
		std::vector<int> nodes_y(NODES, -1);
		
		auto T_x = make_cut_visitor(links, x, old, nodes_x, NODES);
		auto T_y = make_cut_visitor(links, y, old, nodes_y, NODES);
		
		int tk = group.getTrequest ();

		for (int i : T_x) {
			for (int j : T_y) {
			
				rca::Link link (i,j,0);
				
				int cost_link = network.getCost (link.getX(),link.getY());
				if (cost_link > 0 && link != old) {

					int value = (cg.is_used(link) ? cg.value (link) : -1);
					
					if ((value - tk) >= cg.top ()) {
						backup_links.push_back (link);
					} else if ((value - tk) > 0) {
						backup_links.push_back (link);
					} else if (value == -1) {
						backup_links.push_back (link);
					}
				}
				
			}
		}
		
		
		return backup_links;
	}
	
}

template<class SteinerType, class Container, class NetworkType>
void rca::sttalgo::print_solutions_stats (std::vector<SteinerType>& trees, 
							Container &cg, 
							NetworkType& net)
{

	int size_cg = cg.get_heap ().size ();
	
	std::cout << "Network Edges= " << net.getNumberEdges () << std::endl;
	std::cout << "Used = " << size_cg << std::endl;
	
	auto heap = cg.get_heap ();
	auto begin = heap.ordered_begin ();
	auto end = heap.ordered_end ();
	
	for (; begin != end; begin++) {
	
		int cost = net.getCost (begin->getX(), begin->getY());
		
		std::cout << *begin <<" : ";
		std::cout << begin->getValue () << " : ";		
		std::cout << cost << ": ";
		std::cout << "Trees (";
		
		int tree_id = 0;
		for (auto st : trees) {
		
			std::vector<rca::Link> treeLinks = 
				sttreeToVector (st);
				
			if (std::find (std::begin(treeLinks), 
				std::end(treeLinks), *begin) != std::end(treeLinks))
			{
				std::cout << tree_id << " ";
			}
			tree_id++;
		}
		std::cout << ")\n";
	}
	
}

bool rca::sttalgo::check_path_limit (std::vector<steiner> & solucao, 
	std::vector<rca::Group> & mgroups, int limit) 
{

	int k = 0;	
	bool condition = true;
	for (auto g : mgroups) {		
		for (int i : g.getMembers ()) {
			rca::Path path = solucao[k].get_path (i, g.getSource ());
			// cout << path << endl;
			int size = path.size () -1;
			if (size > limit) {
				condition =  false;
				#ifdef DEBUG
					cout << "Invalid Tree: " << k << endl;									
				#endif
				break;
			}
		}
		k++;
	}

	return condition;
}

bool rca::sttalgo::check_path_limit (steiner& st, 
	rca::Group & group, int limit) {

	int source = group.getSource ();
	bool condition = true;
	for (auto && m : group) {
		rca::Path path = st.get_path (m, source);
		int size = path.size () - 1;
		if (size > limit) {
			condition =  false;
			#ifdef DEBUG
				cout << "Invalid Tree"<< endl;									
			#endif
			break;
		}
	}

	return condition;
}


/** explicit instantiation of methods**/
template void rca::sttalgo::remove_top_edges<rca::EdgeContainer<rca::Comparator, rca::HCell>> 
			(rca::EdgeContainer<rca::Comparator, rca::HCell> & ob, 
			rca::Network & m_network, 
			rca::Group & group, 
			int res);

template void rca::sttalgo::prunning<rca::EdgeContainer<rca::Comparator, rca::HCell>, STTree, rca::Network>
		(STTree& st, 
		rca::EdgeContainer<rca::Comparator, rca::HCell>& cont, 
		int treq, 
		int band,
		rca::Network& net);

template void rca::sttalgo::prunning<rca::EdgeContainer<rca::Comparator, rca::HCell>, steiner, rca::Network>
		(steiner& st, 
		rca::EdgeContainer<rca::Comparator, rca::HCell>& cont, 
		int treq, 
		int band,
		rca::Network& net);

template void rca::sttalgo::make_prunning<rca::Network, steiner>(rca::Network& network, steiner& tree);
template void rca::sttalgo::make_prunning<rca::Network, STTree>(rca::Network& network, STTree& tree);

		
template void rca::sttalgo::print_solution<STTree> (std::vector<STTree> &st);
template void rca::sttalgo::print_solution2<STTree> (std::vector<STTree> &st);
template void rca::sttalgo::print_solution2<steiner> (std::vector<steiner> &st);

template void 
rca::sttalgo::print_solutions_stats<STTree,rca::EdgeContainer<rca::Comparator, rca::HCell>, rca::Network>
		(std::vector<STTree> & trees, 
		rca::EdgeContainer<rca::Comparator, rca::HCell> &cg, 
		rca::Network& net);

template 
std::vector<rca::Link> 
rca::sttalgo::get_available_links<STTree,rca::EdgeContainer<rca::Comparator, rca::HCell>, rca::Network> 
	(STTree&st, 
	rca::EdgeContainer<rca::Comparator, rca::HCell> & cg,
	rca::Network& net,
	rca::Group & group, 
	rca::Link & link );