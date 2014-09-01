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

#ifdef DEBUG	
	std::cout << "initialization\n";
#endif
	
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
	
#ifdef DEBUG1
		std::cout << "Number of Ants : " << ants << std::endl;
		std::cout << "pool size: " << pool.size () <<std::endl;
#endif
		//flag mark the join
		int join = -1;
		int in = -1;
		
		//for each ant make a moviment
		for (unsigned ant = 0; ant < pool.size (); ant++) {
	
#ifdef DEBUG1
			std::cout << "Origem :" << pool[ant].get_id ();
			std::cout << " Examinig: " << pool[ant].get_current_position();
#endif
			
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

#ifdef DEBUG1
			std::cout << " " << next << std::endl;
#endif		
			//TODO O QUE FAZE SE PEGAR UM VERTEX BLOQUADO?
			
			//if next == -1 é necessário mudar a forrmiga de lugar
			if (next != -1) {

#ifdef DEBUG1				
				std::cout << "from: " << c_vertex << " id: " << visited[c_vertex];
				std::cout << " >> to: " << next << " id: " << visited[next] << std::endl;
#endif
				
				//TODO check with to groups has been joined
				if ( visited[next] > -1 && (visited[c_vertex] != visited[next]) ) {
	
#ifdef DEBUG1
					std::cout << "Moving from " << c_vertex << " to " << next << "\n";
#endif 
					pool[ant].move (next);
			
					rca::Link link (c_vertex, next, 0.0);
					m_network->removeEdge(link);
	
#ifdef DEBUG1
					std::cout << link << std::endl;
					std::cout << "join: " << pool[ant].get_id () << ":" << visited[next] << "\n";
#endif
					
					for (unsigned i=0; i < pool.size(); i++) {
						
						if (pool[i].get_id() == visited[next]) {
							
							join = i;
							in = ant;
							
							break;
						}
					}
					break;
					
				} else {
		
#ifdef DEBUG1
					std::cout << "Moving from " << c_vertex << " to " << next << "\n";
#endif
					
					pool[ant].move (next);
					visited[next] = pool[ant].get_id ();
			
					rca::Link link (c_vertex, next, 0.0);
					m_network->removeEdge(link);
#ifdef DEBUG1	
					std::cout << link << std::endl;
#endif
					
				}
				
			} else {
	
#ifdef DEBUG1
				std::cout << c_vertex << " has non neighboor: " << next << std::endl;
				std::cout << pool[ant].get_current_position () << std::endl;
#endif	
				pool[ant].back ();

#ifdef DEBUG1
				std::cout << pool[ant].get_current_position () << std::endl;
#endif
				
			}
			
		}//endof for
		
		if (join != -1 && join != in) {

#ifdef DEBUG1
			//in representa a formiga que encontra com o "join"(outra formiga)
			std::cout <<"-------------------\n"<< std::endl;
			
			std::cout << pool[in] << std::endl;
			std::cout << "----\n";
			
			std::cout << pool[join] << std::endl;
			std::cout << "----\n";
#endif
			pool[in].join ( pool[join] );
			
			//marking the vertex
			auto cbegin = pool[in].nodes_begin ();
			auto cend = pool[in].nodes_end();
			for (; cbegin != cend; cbegin++) {
				visited[*cbegin] = pool[in].get_id ();
			}
			

#ifdef DEBUG1
			std::cout << "----RESULTADO\n";
			std::cout << pool[in] << std::endl;
			

			std::cout << "Visited after join\n";
			for (unsigned i = 0; i < visited.size (); i++) {
				std::cout << i << ":" << visited[i] << "| ";
			}
			std::cout << "\n";
			getchar ();
#endif			
			pool.erase (pool.begin () + join);			
			ants--;
			
			
		} 
		
	}//endof while
	
	
#ifdef DEBUG
	//std::cout << pool[0] << std::endl;
	SteinerTree st(m_network->getNumberNodes ());
	
	st.setTerminal (m_groups[0]->getSource());
	for (int i = 0; i < m_groups[0]->getSize (); i++) {
		st.setTerminal (m_groups[0]->getMember (i));
	}
	
	auto link = pool[0].links_begin ();
	for (; link != pool[0].links_end(); link++) {
		
		st.addEdge (link->getX(), link->getY(), 
					m_network->getCost (link->getX(), link->getY()) );
	}
	
	st.prunning ();
	std::cout << st.getCost () << std::endl;
	st.xdotFormat ();
#endif
	
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