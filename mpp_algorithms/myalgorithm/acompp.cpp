#include "acompp.h"

using namespace rca;

void AcoMPP::update_pheromone () {

	std::cout << "Updating Pheromene\n";
	
}

void AcoMPP::solution_construction () {

	std::cout << "solution_construction\n";
	
}

void AcoMPP::initialization () {

#ifdef DEBUG
	std::cout << "initialization\n";
#endif
	
	std::vector<int> visited(m_network->getNumberNodes(), -1);
	
	std::vector<Ant> pool;
	
	//creating the pool of edges
	int id = 0;
	create_ants_by_group (id, pool, visited);
	
	//getting the number of ants
	int ants = pool.size ();
	
	//while the number of ants is greater than 1(number of partitions)
	while (ants > 1) {
	
		//flag mark the join
		int join = -1;
		int in = -1;
		
		//for each ant make a moviment
		for (unsigned ant = 0; ant < pool.size (); ant++) {
			
			//current vertex
			int c_vertex = pool[ant].get_current_position();
		
			//gettting the next
			int next = m_network->get_adjacent_by_minimun_cost (c_vertex);	
			
			//if next == -1 é necessário mudar a forrmiga de lugar
			if (next != -1) {
			
				if ( visited[next] > -1 && (visited[c_vertex] != visited[next]) ) {
	
					pool[ant].move (next);
			
					rca::Link link (c_vertex, next, 0.0);
					m_network->removeEdge(link);
	
					//selecionando o id da formiga que 
					//chegou ao nó next
					join = select_ant_id (pool, visited[next]);
					in = ant;
					
					//breaking the for.
					break;
					
				} else {
					
					pool[ant].move (next);
					visited[next] = pool[ant].get_id ();
			
					rca::Link link (c_vertex, next, 0.0);
					m_network->removeEdge(link);			
				}
				
			} else {
	
				pool[ant].back ();
				
			}
			
		}//endof for
		
		if (join != -1 && join != in) {

			join_ants (pool, in, join, visited);
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
								   std::vector<rca::Ant> & pool, 
								   std::vector<int> & visited)
{
	
	//creating the ants
	for (int i = 0; i < m_groups[g_id]->getSize(); i++) {
	
		//One ant is create for each "terminal node"(source and)
		//destinations
		Ant ant (m_groups[g_id]->getMember (i));		
		pool.push_back (ant);
		
		visited[ant.get_id()];
		
	}
	
	//creating a ant to start de search from the source
	Ant ant (m_groups[g_id]->getSource ());
	pool.push_back (ant);
	
	visited[ant.get_id()];
	
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

/**
 * private methods here
 */

int AcoMPP::select_ant_id (const std::vector<Ant>& pool, const int & next_id)
{
	//seaching for the position of the ant that found next
	for (unsigned i=0; i < pool.size(); i++) {

		if (pool[i].get_id() == next_id) {
					
			return i;
		}
	}
	
	return -1;
}

void AcoMPP::join_ants (std::vector<Ant>& pool, const int& in, const int& join, std::vector<int>& visited)
{
	
	//making the join
	pool[in].join ( pool[join] );
	//marking the vertex
	auto cbegin = pool[in].nodes_begin ();
	auto cend = pool[in].nodes_end();
	for (; cbegin != cend; cbegin++) {
		visited[*cbegin] = pool[in].get_id ();
	}
	pool.erase (pool.begin () + join);
	
}