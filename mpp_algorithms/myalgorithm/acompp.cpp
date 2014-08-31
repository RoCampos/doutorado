#include "acompp.h"

using namespace rca;

void AcoMPP::update_pheromone () {

	std::cout << "Updating Pheromene\n";
	
}

void AcoMPP::solution_construction () {

	std::cout << "solution_construction\n";
	
}

void AcoMPP::initialization () {
	
	std::cout << "initialization\n";
	
}

void AcoMPP::configurate (std::string m_instance) {

	std::cout << "configuring data\n";
	Reader r(m_instance);
	
	//creating the network object
	m_network = new rca::Network;
	
	//configuring the network object
	r.configNetwork (m_network);
	
	//creating the groups
	m_groups = r.readerGroup ();
	
}

void AcoMPP::create_ants_by_group (int g_id, 
								   std::vector<rca::Ant> & pool)
{
	
	//creating the ants
	for (int i = 0; i < m_groups[g_id]->getSize(); i++) {
	
		//One ant is create for each "terminal node"(source and)
		//destinations
		Ant ant (m_groups[g_id]->getMember (i));
		pool.push_back (ant);
		
	}
	
	//creating a ant to start de search from the source
	Ant ant (m_groups[g_id]->getSource ());
	pool.push_back (ant);
	
}