#include "steiner_tree_observer.h"

using namespace rca::sttalgo;
using namespace rca;

template<typename ContainerType, typename SteinerRepr>
SteinerTreeObserver<ContainerType, SteinerRepr>::SteinerTreeObserver(){
	dset = NULL;
}

template<typename ContainerType, typename SteinerRepr>
SteinerTreeObserver<ContainerType, SteinerRepr>::SteinerTreeObserver(ContainerType & ec,
														SteinerRepr & st,
														int nodes)
{
	m_ec = &ec;
	this->set_steiner_tree (st, nodes);
	
}

template<typename ContainerType, typename SteinerRepr>
SteinerTreeObserver<ContainerType, SteinerRepr>::SteinerTreeObserver(ContainerType & ec, 
									SteinerRepr & st, 
									rca::Network& net, 
									int nodes){

	m_ec = &ec;
	m_network = &net;
	this->set_steiner_tree (st, nodes);


}

template<typename ContainerType, typename SteinerRepr>
void SteinerTreeObserver<ContainerType, SteinerRepr>::set_network (rca::Network & net) {
	this->m_network = &net;
}

template<typename ContainerType, typename SteinerRepr>
void SteinerTreeObserver<ContainerType, SteinerRepr>::set_steiner_tree (SteinerRepr & st, int nodes)
{
	m_st = NULL;
	m_st = &st;
	
	if (dset != NULL)
		delete dset;
	
	dset = new DisjointSet2 (nodes);
}

template<typename ContainerType, typename SteinerRepr>
SteinerRepr & SteinerTreeObserver<ContainerType, SteinerRepr>::get_steiner_tree ()
{
	return *m_st;
}

template<typename ContainerType, typename SteinerRepr>
void SteinerTreeObserver<ContainerType, SteinerRepr>::set_container (ContainerType & ec) 
{
	m_ec = &ec;
}

template<typename ContainerType, typename SteinerRepr>
ContainerType & SteinerTreeObserver<ContainerType, SteinerRepr>::get_container () 
{
	return *m_ec;
}

template<typename ContainerType, typename SteinerRepr>
bool SteinerTreeObserver<ContainerType, SteinerRepr>::add_edge (int x, 
												   int y, 
												   int cost, 
												   int band_usage)
{
	
	if ( dset->find(x) != dset->find(y) ) {
	
		dset->simpleUnion (x, y);
		rca::Link l (x,y, cost);
		
		//modificar add_edge in STTree
		//para verificar se a arestas já foi adicionada
		//verificar com debug se o dset já evita isto
		m_st->add_edge ( l.getX(), l.getY(), cost );
		

		//TODO UPDATE USAGE, IT IS USING THE COST		
		if (m_ec->is_used (l)) {
			int value = m_ec->value (l);
			l.setValue (value - 1);
			m_ec->update (l);
		} else {
			l.setValue (band_usage - 1);
			m_ec->push ( l );
		}
		
		return true;
	}
	
	return false;	
}

template<typename ContainerType, typename SteinerRepr>
bool SteinerTreeObserver<ContainerType, SteinerRepr>::add_edge (int x, 
												   int y, 
												   int cost,
												   int trequest, 
												   int band_usage)
{

	//reultilization of add_edge that consumes 1 unit of bandwith
	bool result = this->add_edge (x, y, cost, band_usage);
	rca::Link l (x, y, 0);
	if (result) {

		if ( m_ec->is_used (l) ) {
			int value = m_ec->value (l);

			//updating the usage of the link
			l.setValue (value - (trequest - 1));
			m_ec->update (l);

			return true;
		}

	}

	return false;

}

template<typename ContainerType, typename SteinerRepr>
std::vector<rca::Link> SteinerTreeObserver<ContainerType, SteinerRepr>::getTreeAsLinks () const
{
	std::vector<rca::Link> links;
	
	// edge_t * perc = m_st->get_edge ();
	// while (perc != NULL) {
		
	// 	rca::Link l(perc->x, perc->y, perc->value);		
	// 	if (perc->in)
	// 		links.push_back ( l );
	// 	perc = perc->next;
		
	// }	

	for (auto e : m_st->get_all_edges ()) {
		int cost = m_network->getCost (e.first, e.second);
		rca::Link l(e.first, e.second, cost);		
		links.push_back (l);
	}

	
	return links;
}

template<typename ContainerType, typename SteinerRepr>
void SteinerTreeObserver<ContainerType, SteinerRepr>::prune (int rest, int band)
{
	
	prunning<ContainerType, SteinerRepr>(*m_st, *m_ec, rest, band, *m_network);	

}

template class rca::sttalgo::SteinerTreeObserver<EdgeContainer<Comparator, HCell>, STTree>;
template class rca::sttalgo::SteinerTreeObserver<EdgeContainer<Comparator, HCell>, steiner>;