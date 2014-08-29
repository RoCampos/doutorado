#include "aco.h"

using namespace rca;

template<typename AcoHandle>
void ACO<AcoHandle>::run () const{

	std::cout << "Running: " << m_instance << std::endl;
	
	configurate (m_instance);
	
	initialization ();
	int i = 0;
	while (++i < 10) {
	
		solution_construction ();
		update_pheromone ();
		
	}
}