#ifndef _HEURISTIC_MMMSTP_
#define _HEURISTIC_MMMSTP_

#include <iostream>
#include <string>

template <typename ConcreteMetaHeuristic>
class MetaHeuristic : ConcreteMetaHeuristic
{

	using ConcreteMetaHeuristic::run_metaheuristic;
	
public:
	
	void run (std::string instance) {
		run_metaheuristic (instance);
	}
	
};

#endif