#include "steiner_tree_factory.h"

using namespace rca::sttalgo;
using namespace rca;

template <class Container>
void AGMZSteinerTree<Container>::create_list (rca::Network& network)
{
	auto iter = network.getLinks ().begin();
	auto end = network.getLinks ().end();
	for (; iter != end; iter++) {
		this->m_links.push_back (*iter);
	}
		
	for (auto & it: this->m_links) {
		it.setValue (0);
	}	
}

template <class Container>
void AGMZSteinerTree<Container>::build (SteinerTreeObserver<Container> & sttree, 
							rca::Network & network, 
							rca::Group & g,
							Container& cg)
{

	
	std::vector<rca::Link> links = this->m_links;
		
	int pos = 0;
	//O(E)
	while (!links.empty()) {
		
		rca::Link link = links[pos];
						
		int cost = network.getCost (link.getX(), link.getY());
			
		if (network.isRemoved(link)){
			links.erase ( (links.begin () + pos) );
			continue;
		}
		
		int BAND = network.getBand (link.getX(), link.getY());
		sttree.add_edge (link.getX(), link.getY(), cost, BAND);
		links.erase ( (links.begin () + pos) );
	}
	
}

template <class Container>
void AGMZSteinerTree<Container>::update_usage (	rca::Group& g,
						rca::Network & m_network,
						STTree & st)
{

	edge_t * e = st.get_edges ().begin;
	while (e != NULL) {
	
		if (e->in) { 
		
			rca::Link l(e->x, e->y,0);
			auto link = std::find( this->m_links.begin (), this->m_links.end(), l);
			link->setValue ( link->getValue () + 1);
			
		}
		
		e = e->next;
	}
	
	std::sort(m_links.begin(), m_links.end());
	
}

/********* ------------ Shortest Path Class ------------ *********/

template <class Container>
void ShortestPathSteinerTree<Container>::build (
				SteinerTreeObserver<Container> & sttree, 
				rca::Network & network, 
				rca::Group & g,
				Container& cg)
{
	
	int source = g.getSource ();
	std::vector<int> members = g.getMembers ();
	std::vector<int> prev = all_shortest_path (source, members[0] , network);
		
	for (int m : members) {
		rca::Path path = get_shortest_path (source, m, network, prev);
		
		auto rit = path.rbegin ();
		for (; rit != path.rend()-1; rit++) {
	
			int x = *rit;
			int y = *(rit+1);
			
			rca::Link l(x, y, 0);
			
			int cost = network.getCost (l.getX(), l.getY());
			int BAND = network.getBand (l.getX(), l.getY());
			
			sttree.add_edge (l.getX(), l.getY(), cost, BAND);
		}			
	}
	
}

/** ------------------------ WildestSteinerTree ------------------- **/
template <class Container>
void WildestSteinerTree<Container>::build (
				SteinerTreeObserver<Container> & sttree, 
				rca::Network & network, 
				rca::Group & g,
				Container& cg)
{

	int source = g.getSource ();
	std::vector<int> members = g.getMembers ();
	std::vector<int> prev = inefficient_widest_path (source, 
													members[0], 
													&network);
		
	for (int m : members) {
		rca::Path path = get_shortest_path (source, m, network, prev);
		
		auto rit = path.rbegin ();
		for (; rit != path.rend()-1; rit++) {
	
			int x = *rit;
			int y = *(rit+1);
			
			rca::Link l(x, y, 0);
			
			int cost = network.getCost (l.getX(), l.getY());
			int BAND = network.getBand (l.getX(), l.getY());
			
			sttree.add_edge (l.getX(), l.getY(), cost, BAND);
		}			
	}

	
}

template class rca::sttalgo::SteinerTreeFactory<rca::EdgeContainer<rca::Comparator, rca::HCell> >; 
template class rca::sttalgo::AGMZSteinerTree<rca::EdgeContainer<rca::Comparator, rca::HCell> >;
template class rca::sttalgo::ShortestPathSteinerTree<rca::EdgeContainer<rca::Comparator, rca::HCell> >;
template class rca::sttalgo::WildestSteinerTree<rca::EdgeContainer<rca::Comparator, rca::HCell> >;