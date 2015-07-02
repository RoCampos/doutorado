#include <iostream>
#include "edgecontainer.h"
#include "sttree.h"
#include "disjointset2.h"
#include "sttree_visitor.h"
#include "link.h"

namespace rca {
	
namespace sttalgo {

#ifndef _STEINER_TREE_OBSERVER_
#define _STEINER_TREE_OBSERVER_
	
template<typename ContainerType>
class SteinerTreeObserver {

public:
	SteinerTreeObserver();
	SteinerTreeObserver(ContainerType & ec, STTree & st, int);
	
	void set_steiner_tree (STTree &st, int);
	STTree & get_steiner_tree ();
	
	void set_container (ContainerType & ec);
	ContainerType & get_container ();
	
	bool add_edge (int, int, int, int);
	
	std::vector<rca::Link> getTreeAsLinks () const;
	
	void prune (int, int);

private:
	ContainerType * m_ec;
	STTree * m_st;	
	DisjointSet2 * dset;
	
};

#endif

}

}

