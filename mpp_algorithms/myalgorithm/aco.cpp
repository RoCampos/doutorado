#include "aco.h"

using namespace rca;

template<typename AcoHandle>
void ACO<AcoHandle>::run (std::string instance, int iter, ...) {
	
	//configurating the data
	configurate2 (instance);
	
	//running the algorithm
	run (iter);
}