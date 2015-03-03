#ifndef _HEURISTIC_MMMSTP_
#define _HEURISTIC_MMMSTP_

#include <iostream>

template <typename ConcreteMetaHeuristic>
class MetaHeuristic : ConcreteMetaHeuristic
{

	using ConcreteMetaHeuristic::run_metaheuristic;
	
public:
	
	void run (std::string instance) {
		run_metaheuristic (std::string instance);
	}
	
};

class GeneticAlgorithm {

protected:
	void run_metaheuristic (std::string instance);
	
}


#endif