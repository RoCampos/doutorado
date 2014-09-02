#include "acompp.h"

using namespace rca;

void AcoMPP::update_pheromone () {

	std::cout << "Updating Pheromene\n";
	
}

void AcoMPP::solution_construction () {

	std::cout << "solution_construction\n";
	
}

void AcoMPP::build_tree (int id, 
						 std::shared_ptr<SteinerTree> & st, 
						 EdgeContainer & ec) 
{

	//container that marks the partions created by ants
	std::vector<int> visited(m_network->getNumberNodes(), -1);
	
	//container that stores the ants 
	std::vector<Ant> pool;
	
	//creating the pool of edges	
	create_ants_by_group (id, pool, visited);
	
	//getting the number of ants
	int ants = pool.size ();
	
	std::vector<rca::Link> toRemove(m_network->getRemovedEdges());
	
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
			//TODO sem implementar
			int next = m_network->get_adjacent_by_minimun_cost (c_vertex, toRemove);
			
			//if next == -1 é necessário mudar a forrmiga de lugar
			if (next != -1) {
			
				if ( (visited[next] > -1) && (visited[c_vertex] != visited[next]) ) {
	
					//moving the ant
					pool[ant].move (next);
			
					rca::Link link (c_vertex, next, 0.0);
					//m_network->removeEdge(link);
					toRemove.push_back (link);
					
					//adding the edges to st structure
					st->addEdge (link.getX(), link.getY(), 
						m_network->getCost(link.getX(), link.getY()) );
	
					//selecionando o id da formiga que 
					//chegou ao nó next
					join = select_ant_id (pool, visited[next]);
					in = ant;
					
					//breaking the for.
					break;
					
					//added to avoid cicle
				} else if (visited[c_vertex] != visited[next]){
					
					//moving the ant
					pool[ant].move (next);
					
					//joining the next to the ant group of vertex
					visited[next] = pool[ant].get_id ();
			
					rca::Link link (c_vertex, next, 0.0);
					toRemove.push_back (link);
					//m_network->removeEdge(link);
					
					st->addEdge (link.getX(), link.getY(), 
								m_network->getCost(link.getX(), link.getY()) );
				} else {
					pool[ant].back ();	
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
	
	st->prunning ();
	
}

void AcoMPP::initialization () {

#ifdef DEBUG
	std::cout << "initialization\n";
#endif
	
	//initialization of the strutctures that suppor congestion evaluation
	EdgeContainer ec;
	ec.init_congestion_matrix (m_network->getNumberNodes ());
	ec.init_handle_matrix (m_network->getNumberNodes ());
	
	m_cost = 0.0;
	m_congestion = 0.0;
	
	for (unsigned i = 0; i < m_groups.size (); i++) {
		
		std::shared_ptr<SteinerTree> 
				st = std::make_shared<SteinerTree> (m_network->getNumberNodes());
		
		ec.connected_level ( *m_groups[i].get() , *m_network);
		
		//steiner tree initializatioon
		for (int j=0; j < m_groups[i]->getSize(); j++) {
			st->setTerminal (m_groups[i]->getMember (j));
		}		
		st->setTerminal (m_groups[i]->getSource ());
			
		build_tree (i, st, ec);
		
		update_congestion (st, ec);
		
#ifdef DEBUG
		std::cout << st->getCost () << std::endl; 
		std::string file ="/home/romeritocampos/workspace/Doutorado/inst_test/aco-test";
		file = file+"/saida.xdot";
		std::cout << file << std::endl;
		st->xdotToFile (file);
		std::string cmd ("xdot");
		cmd += " " + file;
		
		system (cmd.c_str ());
		
		getchar ();		
#endif		
	
	}
	
	std::cout << m_congestion << " " << m_cost << std::endl;
	
}

void AcoMPP::configurate (std::string m_instance) {

#ifdef DEBUG	
	std::cout << "configuring data\n";
#endif
	
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
		
		visited[ant.get_id()] = ant.get_id();
		
	}
	
	//creating a ant to start de search from the source
	Ant ant (m_groups[g_id]->getSource ());
	pool.push_back (ant);
	
	visited[ant.get_id()] = ant.get_id();
	
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

void AcoMPP::join_ants (std::vector<Ant>& pool, 
						const int& in, 
						const int& join, 
						std::vector<int>& visited)
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

void AcoMPP::update_congestion (std::shared_ptr<SteinerTree>& st,
							rca::EdgeContainer & ec)
{
#ifdef DEBUG
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	
	Edge * e = st->listEdge.head;
	while (e != NULL) {
	
		//removing the edges from network
		m_network->removeEdge (rca::Link(e->i,e->j, 0.0));
	
		//NÃO ESTÁ FUNCIONANDO!!!
		//TENHO QUE PROVER OUTRO MEIO PARA MANTER AS ARESTAS EM NÍVEIS
		rca::Link link (e->i, e->j, 0);
		int x = link.getX ();
		int y = link.getY ();
		if (ec.m_ehandle_matrix[x][y].first == true) {
			
			int valor = (*((ec.m_ehandle_matrix[x][y]).second)).getValue();
			(*((ec.m_ehandle_matrix[x][y]).second)).setValue (valor+1);
			ec.m_heap.update ((ec.m_ehandle_matrix[x][y]).second);
			
			if (valor+1 > m_congestion) {
				m_congestion = (valor + 1);
			}
			
		} else {
			ec.m_ehandle_matrix[x][y].first = true;
			link.setValue (1);
			ec.m_ehandle_matrix[x][y].second = ec.m_heap.push (link);
			
			m_cost += m_network->getCost (link.getX(), link.getY());
			
		}
		
		e = e->next;
	}	
	e = NULL;
	
}