#ifndef _HEURISTIC_MMMSTP_
#define _HEURISTIC_MMMSTP_

#include <iostream>

template <typename ConcreteMetaHeuristic>
class MetaHeuristic : ConcreteMetaHeuristic
{

	using ConcreteMetaHeuristic::run_metaheuristic;
	
public:
	
	void run () {
		run_metaheuristic ();
	}
	
};

class GeneticAlgorithm {

protected:
	
	void run_metaheuristic ();
	
}


#endif