#ifndef _MPP_ACO_HANDLEs_
#define _MPP_ACO_HANDLEs_

#include <iostream>
#include <string>
#include <vector>

#include "network.h"
#include "reader.h"
#include "group.h"

typedef typename std::vector<rca::Group> VGroup;

namespace rca {

/**
 * This classes implements the methods
 * need to ACO.
 * 
 * the methods of the metaheuristic ACO 
 * are implemented here: construction, pheromene
 * update and so on.
 * 
 * @author Romerito Campos
 * @date 29/08/2014
 */
class AcoMPP {
	
protected:
	void update_pheromone () const;
	void solution_construction () const;
	void initialization () const;
	void configurate (std::string file) const;

public:
	rca::Network m_network;
	VGroups m_groups;
	
};

}

#endif /*_MPP_ACO_HANDLEs_*/