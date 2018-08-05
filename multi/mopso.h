#ifndef __MOPSO_H_
#define __MOPSO_H_

#include "link.h"
#include "multiobjective.h"

struct particle_t {

	//each particle has a vector of index associated
	//with the edges of the topology.

	//so the priority is used to check if a edge should be used.

	//maybe some normalization should be performed


};

struct order_by_node
{
	bool operator() (rca::Link const &l1, rca::Link const&l2) const
	{
		return l1.getX() < l2.getX();
	}
};

struct order_by_node1
{
	bool operator() (rca::Link const &l1, rca::Link const&l2) const
	{
		return l1.getX() < l2.getX() && l1.getY() < l2.getY();
	}
};






#endif