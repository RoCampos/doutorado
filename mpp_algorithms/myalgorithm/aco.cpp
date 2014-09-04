#include "aco.h"

using namespace rca;

template<typename AcoHandle>
void ACO<AcoHandle>::run () {

	std::cout << "Running: " << m_instance << std::endl;
	
	//configurating the data
	configurate (m_instance);
	
	//running the algorithm
	run (m_iterations);
}