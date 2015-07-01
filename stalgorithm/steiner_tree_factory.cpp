#include "steiner_tree_factory.h"

template <class Container>
void AGMZSteinerTree<Container>::build (rca::SteinerTreeObserver<Container> & sttree, 
							rca::Network & network, 
							rca::Group & g,
							Container& cg)
{

	std::cout << __FUNCTION__ << std::endl;
	
}

template class SteinerTreeFactory<rca::EdgeContainer<rca::Comparator, rca::HCell> >; 
template class AGMZSteinerTree<rca::EdgeContainer<rca::Comparator, rca::HCell> >;