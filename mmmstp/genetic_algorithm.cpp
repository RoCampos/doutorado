#include "genetic_algorithm.h"
#include "metaheuristic.h"

void GeneticAlgorithm::run_metaheuristic (std::string instance)
{
	/*configuring the problem*/
	m_instance = instance;
	m_network = new Network;
	Reader r (m_instance);
	r.configNetwork ( m_network );
	
	/*configuring parameters - default*/
	init_parameters ();
	
	/*init population*/
	init_population ();
	
	
	
	//deallocatin of resources;
	delete m_network;
}

void GeneticAlgorithm::init_population ()
{
#ifdef DEBUG
	std::cout << "Creating Population: size = " << m_pop << std::endl;	
#endif
	for (int i=0; i < m_pop; i++) {
		m_population[i].init_rand_solution (m_network, m_groups);		
	}
	
}

void PathRepresentation::init_rand_solution (rca::Network * net, 
									  std::vector<rca::Group>& groups)
{
	int GROUPS = groups.size ();
	
	for (int i=0; i < GROUPS; i++) {
		int source = groups[i].getSource ();
		for (int d=0; d < groups[i].getSize (); i++) {
			int w = groups[i].getMember (d);
			rca::Path path = shortest_path (source, w, net);
			std::cout << path << std::endl;
		}
	}
	
}

int main (int argc, char**argv) 
{

	std::string instance = argv[1];
	MetaHeuristic<GeneticAlgorithm> algorithm;
	
	
	algorithm.run (instance);
	
}