#include "acompp.h"

using namespace rca;

void AcoMPP::update_pheromone () {

	std::cout << "Updating Pheromene\n";
	
}

void AcoMPP::solution_construction () {

	std::cout << "solution_construction\n";
	
}

void AcoMPP::initialization () {
	
	typedef typename std::vector<int>::const_iterator c_iterator;
	
	std::cout << "initialization\n";
	
	std::vector<int> visited(m_network->getNumberNodes(), -1);
	
	std::vector<Ant> pool;
	
	//creating the pool of edges
	int id = 0;
	create_ants_by_group (id, pool);
	
	//marking the partitions
	for (unsigned ant = 0; ant < pool.size (); ant++) {
		
		int id = pool[ant].get_id();		
		visited[ id ] = id;
	}
	
	//getting the number of ants
	int ants = pool.size ();
	
	//while the number of ants is greater than 1(number of partitions)
	while (ants > 1) {
		
		std::cout << "Number of Ants : " << ants << std::endl;
		std::cout << "pool size: " << pool.size () <<std::endl;
		
		//flag mark the join
		int join = -1;
		int in = -1;
		
		//for each ant make a moviment
		for (unsigned ant = 0; ant < pool.size (); ant++) {
		
			std::cout << "Origem :" << pool[ant].get_id ();
			std::cout << " Examinig: " << pool[ant].get_current_position();
			
			//current vertex
			int c_vertex = pool[ant].get_current_position();
		
			//getting the next vertex by the minimum cost
			std::pair<c_iterator, c_iterator> iters;
			m_network->get_iterator_adjacent (c_vertex, iters);
			auto it = iters.first;
			double cost = std::numeric_limits<double>::max();
			int next = -1;
			for (; it != iters.second; ++it) {
			
				rca::Link link (c_vertex, *it, 0.0);
				if ( !m_network->isRemoved( link )  ) {
					if (cost > m_network->getCost (c_vertex, *it)) {
						cost = m_network->getCost (c_vertex, *it);
						next = *it;
					}
				}
			}
			
			std::cout << " " << next << std::endl;
			
			//TODO O QUE FAZE SE PEGAR UM VERTEX BLOQUADO?
			
			//if next == -1 é necessário mudar a forrmiga de lugar
			if (next != -1) {
				
				//TODO check with to groups has been joined
				if ( visited[next] > -1 && (visited[c_vertex] != visited[next]) ) {
					
					std::cout << "Moving from " << c_vertex << " to " << next << "\n";
					pool[ant].move (next);
			
					rca::Link link (c_vertex, next, 0.0);
					m_network->removeEdge(link);
				
					std::cout << link << std::endl;
					
					std::cout << "join: " << pool[ant].get_id () << ":" << next << "\n";
					
					for (int i=0; i < pool.size(); i++) {
						
						if (pool[i].get_id() == visited[next]) {
							
							//update visisted
							for (int j=0; j < visited.size(); j++) {
								if (visited[j] == visited[next] ) {
									visited[j] = pool[ant].get_id ();
								}
							}
							
							//marcando a remoção
							join = i;
							in = ant;
							
							ants--;
							break;
						}
					}
					break;
					
				} else {
					
					std::cout << "Moving from " << c_vertex << " to " << next << "\n";
					pool[ant].move (next);
					visited[next] = pool[ant].get_id ();
			
					rca::Link link (c_vertex, next, 0.0);
					m_network->removeEdge(link);
				
					std::cout << link << std::endl;
					
				}
			} else {
				std::cout << c_vertex << " has non neighboor: " << next << std::endl;
				
				std::cout << pool[ant].get_current_position () << std::endl;
				pool[ant].back ();
				std::cout << pool[ant].get_current_position () << std::endl;
			}
			
		}//endof for
		
		if (join != -1) {
			
			//in representa a formiga que encontra com o "join"(outra formiga)
			std::cout <<"-------------------\n"<< pool[in] << std::endl;
			
			std::cout << pool[join] << std::endl;
			
			pool[in].join ( pool[join] );
			
			std::cout << pool[in] << std::endl;
			pool.erase (pool.begin () + join);
			
			//getchar ();
			
		} 
		
		if (ants == 4) {
			std::cout << std::endl;
			for (int antts=0; antts < pool.size(); antts++) {
				//std::cout << pool[antts] << std::endl;
			}
			
		}
		
	
	}//endof while
	
	/*
	auto its = pool.begin ();
	for (; its != pool.end(); its++) {
		std::cout << *its << std::endl;
	}
	
	for (int i=0;i < visited.size (); i++) {
		std::cout << i << ":" << visited[i] << std::endl;
	}*/
	
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

void AcoMPP::configurate2 (std::string file){
	
	SteinerReader reader (file);
	
	m_network = new rca::Network;
	
	reader.configNetwork (m_network);
	
	Group group = reader.terminalsAsGroup ();
	
	Group * gg = new Group(0, group.getSource(), 0);
	for (int i=0; i < group.getSize (); i++) {
		gg->addMember (group.getMember(i));
	}
	std::cout << group << std::endl;
	std::shared_ptr<rca::Group> g(gg);
	m_groups.push_back (g);
	
}