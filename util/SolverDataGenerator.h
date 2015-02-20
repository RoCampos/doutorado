#ifndef _SOLVER_DATA_GENERATOR_H_
#define _SOLVER_DATA_GENERATOR_H_

#include <iostream>
#include <ostream>

#include "group.h"
#include "network.h"
#include "reader.h"
#include "steinerReader.h"

template <typename Model>
class DataGenerator : private Model
{
	using Model::generate;
	
public:
	
	void run (rca::Network * network, rca::Group *g) {		
		generate (network, g);
		
	}
	
	void run (rca::Network * network, std::vector<std::shared_ptr<Group>>& g) {
		generate (network, g);
	}
	
	
};


class MultiCommidityFormulation 
{
	
public:
	
	void generate (rca::Network * network, rca::Group *g) ;
	
};

class MultipleMulticastCommodityFormulation
{

public:
	void generate (rca::Network * network, 
					std::vector<std::shared_ptr<Group>>&);
	
};

#endif