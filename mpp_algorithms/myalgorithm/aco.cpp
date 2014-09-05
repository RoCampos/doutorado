#include "aco.h"

using namespace rca;

template<typename AcoHandle>
void ACO<AcoHandle>::run (int numArgs, std::string instance, ...) {
	
	std::va_list vl;
	va_start (vl, numArgs);
	
	//configurating the data
	configurate2 (instance);

	
	//running the algorithm
	run (vl);
}