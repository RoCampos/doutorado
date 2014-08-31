#ifndef _MPP_ACO_HANDLEs_
#define _MPP_ACO_HANDLEs_

#include <iostream>
#include <string>
#include <vector>

#include "reader.h"
#include "network.h"
#include "reader.h"
#include "group.h"

typedef typename std::vector<std::shared_ptr<rca::Group>> MGroups;

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
	void update_pheromone ();
	void solution_construction ();
	void initialization ();
	
	/**
	 * This method is used to configurate
	 * the informations of the problem.
	 * 
	 * It receives a string as parameter. This
	 * string is the instance tha must be read.
	 * 
	 * inside this method all attributes are instatiated.
	 * 
	 * @author Romerito Campos
	 * @date 29/08/2014
	 */
	void configurate (std::string file);

public:
	
	rca::Network * m_network;
	
	MGroups m_groups;
	
	std::vector<Ant> ant_pool;
	
};

}

#endif /*_MPP_ACO_HANDLEs_*/