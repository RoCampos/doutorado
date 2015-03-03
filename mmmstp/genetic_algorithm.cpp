#include "metaheuristic.h"

void GeneticAlgorithm::run_metaheuristic (std::string instance)
{
	
	std::cout << instance << std::endl;
	
}

int main (int argc, char**argv) 
{

	std::string instance = argv[1];
	MetaHeuristic<GeneticAlgorithm> algorithm;
	
	
	
	algorithm.run (instance);
	
}