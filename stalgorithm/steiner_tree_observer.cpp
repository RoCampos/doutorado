#include "steiner_tree_observer.h"

using namespace rca;

template<typename ContainerType>
SteinerTreeObserver<ContainerType>::SteinerTreeObserver(){
	dset = NULL;
}

template<typename ContainerType>
SteinerTreeObserver<ContainerType>::SteinerTreeObserver(ContainerType & ec,
														STTree & st,
														int nodes)
{
	m_ec = &ec;
	this->set_steiner_tree (st, nodes);
	
}

template<typename ContainerType>
void SteinerTreeObserver<ContainerType>::set_steiner_tree (STTree & st, int nodes)
{
	m_st = NULL;
	m_st = &st;
	
	if (dset != NULL)
		delete dset;
	
	dset = new DisjointSet2 (nodes);
}

template<typename ContainerType>
STTree & SteinerTreeObserver<ContainerType>::get_steiner_tree ()
{
	return *m_st;
}

template<typename ContainerType>
void SteinerTreeObserver<ContainerType>::set_container (ContainerType & ec) 
{
	m_ec = &ec;
}

template<typename ContainerType>
ContainerType & SteinerTreeObserver<ContainerType>::get_container () 
{
	return *m_ec;
}

template<typename ContainerType>
bool SteinerTreeObserver<ContainerType>::add_edge (int x, 
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

template<typename ContainerType>
std::vector<rca::Link> SteinerTreeObserver<ContainerType>::getTreeAsLinks () const
{
	std::vector<rca::Link> links;
	
	edge_t * perc = m_st->get_edge ();
	while (perc != NULL) {
		
		rca::Link l(perc->x, perc->y, perc->value);		
		if (perc->in)
			links.push_back ( l );
		perc = perc->next;
		
	}	
	
	return links;
}

template<typename ContainerType>
void SteinerTreeObserver<ContainerType>::prune (int rest, int band)
{
	prunning<ContainerType>(*m_st, *m_ec, rest, band); 
}

template class SteinerTreeObserver<EdgeContainer<Comparator, HCell>>;