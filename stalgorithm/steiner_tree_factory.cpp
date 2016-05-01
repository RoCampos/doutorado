#include "steiner_tree_factory.h"

using namespace rca::sttalgo;
using namespace rca;

template <class Container, class SteinerRepr>
void AGMZSteinerTree<Container, SteinerRepr>::create_list (rca::Network& network)
{

	this->m_links.clear ();

	auto iter = network.getLinks ().begin();
	auto end = network.getLinks ().end();
	for (; iter != end; iter++) {
		this->m_links.push_back (*iter);
	}
		
	for (auto & it: this->m_links) {
		it.setValue (0);
	}	
}

template <class Container, class SteinerRepr>
void AGMZSteinerTree<Container, SteinerRepr>::build (SteinerTreeObserver<Container, SteinerRepr> & sttree, 
							rca::Network & network, 
							rca::Group & g,
							Container& cg)
{

	
	std::vector<rca::Link> links = this->m_links;
		
	int pos = 0;
	
	int trequest = g.getTrequest ();

	//O(E)
	while (!links.empty()) {
		
		rca::Link link = links[pos];
						
		int cost = network.getCost (link.getX(), link.getY());
			
		if (network.isRemoved(link)){
			links.erase ( (links.begin () + pos) );
			continue;
		}
		
		int BAND = network.getBand (link.getX(), link.getY());

		// sttree.add_edge (link.getX(), link.getY(), cost, BAND);
		sttree.add_edge (link.getX(), link.getY(), cost, trequest, BAND);
		links.erase ( (links.begin () + pos) );
	}
	
}

template <class Container, class SteinerRepr>
void AGMZSteinerTree<Container, SteinerRepr>::update_usage (rca::Group& g,
						rca::Network & m_network,
						SteinerRepr & st)
{


	int trequest = g.getTrequest ();

	for (std::pair<int,int> e : st.get_all_edges()) {		
		rca::Link l (e.first, e.second, 0);
		int cost = m_network.getCost (l.getX(), l.getY());
		l.setValue (cost);

		auto link = std::find( this->m_links.begin (), this->m_links.end(), l);
		link->setValue ( link->getValue () + trequest);
		
	}

	
	std::sort(m_links.begin(), m_links.end());
	
}

/********* ------------ Shortest Path Class ------------ *********/

template <class Container, class SteinerRepr>
void ShortestPathSteinerTree<Container, SteinerRepr>::build (
				SteinerTreeObserver<Container, SteinerRepr> & sttree, 
				rca::Network & network, 
				rca::Group & g,
				Container& cg)
{
	
	// int source = g.getSource ();
	std::vector<int> members = g.getMembers ();
	members.push_back (g.getSource ());
	
	int pos = rand () % members.size ();
	int source = members[pos];
	members.erase (members.begin () + pos);

	std::vector<int> prev = all_shortest_path (source, members[0] , network);
		
	int trequest = g.getTrequest ();

	for (int m : members) {
		rca::Path path = get_shortest_path (source, m, network, prev);
		
		auto rit = path.rbegin ();
		for (; rit != path.rend()-1; rit++) {
	
			int x = *rit;
			int y = *(rit+1);
			
			rca::Link l(x, y, 0);
			
			int cost = network.getCost (l.getX(), l.getY());
			int BAND = network.getBand (l.getX(), l.getY());
			
			//sttree.add_edge (l.getX(), l.getY(), cost, BAND);			
			sttree.add_edge (l.getX(), l.getY(), cost, trequest, BAND);
		}			
	}
	
}

/** ------------------------ WildestSteinerTree ------------------- **/
template <class Container, class SteinerRepr>
void WildestSteinerTree<Container, SteinerRepr>::build (
				SteinerTreeObserver<Container, SteinerRepr> & sttree, 
				rca::Network & network, 
				rca::Group & g,
				Container& cg)
{

	int source = g.getSource ();
	std::vector<int> members = g.getMembers ();
	std::vector<int> prev = inefficient_widest_path (source, 
													members[0], 
													&network);
	int trequest = g.getTrequest ();
	for (int m : members) {
		rca::Path path = get_shortest_path (source, m, network, prev);
		
		auto rit = path.rbegin ();
		for (; rit != path.rend()-1; rit++) {
	
			int x = *rit;
			int y = *(rit+1);
			
			rca::Link l(x, y, 0);
			
			int cost = network.getCost (l.getX(), l.getY());
			int BAND = network.getBand (l.getX(), l.getY());
			
			// sttree.add_edge (l.getX(), l.getY(), cost, BAND);
			sttree.add_edge (l.getX(), l.getY(), cost, trequest, BAND);
		}			
	}
	
}

template <class Container, class SteinerRepr>
void WildestSteinerTree<Container, SteinerRepr>::update_band (rca::Group & g, 
												rca::Network& network,
												STTree & st)
{

	edge_t *e = st.get_edge ();
	while (e != NULL) {
		
		if (e->in) {
		
			int tk = g.getTrequest ();
			int band = network.getBand(e->x, e->y);
			network.setBand(e->x, e->y, band - tk);
			network.setBand(e->y, e->x, band - tk);
		}
		
		e = e->next;
	}
	
}

// ---------------------- INTERATIVE DEEPING DEPTH FIRST SEARCH ------------------ 
template <class Container, class SteinerRepr>
void LimitedBreadthSearchFirst<Container, SteinerRepr>::build (
				SteinerTreeObserver<Container, SteinerRepr> & sttree, 
				rca::Network & network, 
				rca::Group & g,
				Container& cg)
{

	int NODES = network.getNumberNodes ();
	int trequest = g.getTrequest ();

	//getting all members nodes
	std::vector<int> members = g.getMembers ();
	//members.push_back (g.getSource ());
	
	//defining a 'source' of the search
	int pos = rand () % members.size ();
	//int source = members[pos];
	//members.erase (members.begin () + pos);
	int source = g.getSource ();

	std::vector<int> marked = std::vector<int> (NODES, 0);
	m_pred = std::vector<int> (NODES,0);

	std::vector<int> counter = std::vector<int> (NODES, 0);
	counter[source] = 0;

	//starting...	
	std::queue<int> queue;
	queue.push (source);

	m_pred[source] = -1;

	int term_count = 0;
	while (!queue.empty () || (term_count == members.size () - 1) ) {

		int curr_node = queue.front ();

		marked[curr_node] = 1;

		auto neighbors = network.get_neighboors (curr_node);

		std::random_shuffle (neighbors.begin (), neighbors.end ());

		auto begin = neighbors.begin ();
		auto end = neighbors.end ();
		for ( ; begin != end; begin++) {

			int next_node = *begin;
			rca::Link l (curr_node, next_node, 0);

			int limit = counter[next_node];				
			if (!network.isRemoved (l) && marked[next_node] == 0 && limit < this->LIMIT) {
				
				//add link to tree
				int cost = network.getCost (l.getX(), l.getY());
				int BAND = network.getBand (l.getX(), l.getY());
				sttree.add_edge (l.getX(), l.getY(), cost, trequest, BAND);

				//add node to queue.
				queue.push (next_node);
				marked[next_node] = 2;
				m_pred[next_node] = curr_node;
				counter[next_node] = counter[curr_node] + 1;			

				if (sttree.get_steiner_tree ().is_terminal (next_node) ) {
					term_count++;
				}

			}

		}

		queue.pop ();

	}

}

template <class Container, class SteinerRepr>
int LimitedBreadthSearchFirst<Container, SteinerRepr>::get_path_length (
	int member, int source)
{

	int next = member;
	int count = 0;
	do {

		next = m_pred[next];
		count++;
	} while (next != -1);

	return count;
}





template class rca::sttalgo::SteinerTreeFactory<rca::EdgeContainer<rca::Comparator, rca::HCell>, STTree >; 
template class rca::sttalgo::AGMZSteinerTree<rca::EdgeContainer<rca::Comparator, rca::HCell>, STTree >;
template class rca::sttalgo::ShortestPathSteinerTree<rca::EdgeContainer<rca::Comparator, rca::HCell>, STTree >;
template class rca::sttalgo::WildestSteinerTree<rca::EdgeContainer<rca::Comparator, rca::HCell>, STTree >;

template class rca::sttalgo::ShortestPathSteinerTree<rca::EdgeContainer<rca::Comparator, rca::HCell>, steiner >;
template class rca::sttalgo::AGMZSteinerTree<rca::EdgeContainer<rca::Comparator, rca::HCell>, steiner >;
template class rca::sttalgo::SteinerTreeFactory<rca::EdgeContainer<rca::Comparator, rca::HCell>, steiner >; 
template class rca::sttalgo::LimitedBreadthSearchFirst<rca::EdgeContainer<rca::Comparator, rca::HCell>, steiner>;