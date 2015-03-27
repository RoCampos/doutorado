#include "acompp.h"

using namespace rca;

void AcoMPP::build_tree (int id, 
						 std::shared_ptr<SteinerTree> & st, 
						 EdgeContainer<Comparator,HCell> & ec) 
{
#ifdef DEBUG
	std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
#endif 

	//container that marks the partions created by ants
	std::vector<int> visited(m_network->getNumberNodes(), -1);
	
	//container that stores the ants 
	std::vector<Ant> pool;
	
	//creating the pool of edges	
	create_ants_by_group (id, pool, visited);
	
	//getting the number of ants
	int ants = pool.size ();
	
	std::vector<rca::Link> toRemove(m_network->getRemovedEdges());
	
	AntRandom a( my_random.get_engine() , 0, (int)pool.size()-1 );
	//while the number of ants is greater than 1(number of partitions)	
	while (ants > 1) {
	
		//flag mark the join
		int join = -1;
		int in = -1;
		
		a.reset_interval (0, (int)pool.size()-1);		
		int ant = a.rand ();
		
		//current vertex
		int c_vertex = pool[ant].get_current_position();
			
		//gettting the next
		//TODO sem implementar
		//int next = m_network->get_adjacent_by_minimun_cost (c_vertex, toRemove);
		//std::cout << "next" << next << std::endl;
		//std::cout << "Choosed:" << next_component (c_vertex, toRemove) << std::endl;
		
		int next = next_component (c_vertex, toRemove);
			
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
				//break;
				
#ifdef VIEWER
				/* just for test */
				rca::Link l (c_vertex, next,0);
				viewer.addLink (l);
				viewer.draw ("../teste");
#endif
				
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
				
#ifdef VIEWER
				/* just for test */
				rca::Link l (c_vertex, next,0);
				viewer.addLink (l);
				viewer.draw ("../teste");
#endif
				
			} else {
					
				rca::Link l(c_vertex, next, 0.0);
				toRemove.push_back (l);
					
				pool[ant].back ();
			}
				
		} else {
					
			pool[ant].back ();
								
		}
		
		if (join != -1 && join != in) {

			join_ants (pool, in, join, visited);
			ants--;
		} 
		
	}//endof while
	
	st->prunning ();

#ifdef VIEWER
	viewer.draw ("../teste");	
	std::cout << "drawing tree\n";
	getchar ();
	viewer.clear ();
#endif
	
#ifdef DEBUG
	std::cout << "------------------------------" << std::endl;
#endif
	
}

void AcoMPP::run (va_list & arglist) {

#ifdef DEBUG
	std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
#endif
	
	int iterations = va_arg(arglist, int);
	m_alpha = va_arg (arglist, double);
	m_betha = va_arg (arglist, double);
	m_phe_rate = va_arg (arglist, double);
	m_heuristic_prob = va_arg (arglist, double);
	m_local_upd = va_arg(arglist,double);

	std::vector<SteinerTree> bestNLinks;
	
	time_elapsed.started ();
	for (int iter =0; iter < iterations; iter++) {
		//initialization of the strutctures that suppor congestion evaluation
		EdgeContainer<Comparator,HCell> ec;
		ec.init_congestion_matrix (m_network->getNumberNodes ());
		ec.init_handle_matrix (m_network->getNumberNodes ());
	
		std::vector<SteinerTree> solutions;
		double cost = 0.0;
		
#ifdef RES_CAP
		double congestion = INT_MAX;
#endif
		
#ifdef CONG
		double congestion = 0.0;
#endif	
		
		//TODO COLOCAR CRIAÇÃO FORA E FAZER SHUFFLE AQUI
		std::vector<int> ggg = std::vector<int>(m_groups.size ());
		for (int i=1; i <(int)ggg.size (); i++) {
			ggg[i] = ggg[i-1] + 1;
		}
		std::random_shuffle (ggg.begin (), ggg.end());
		//for (unsigned i = 0; i < m_groups.size (); i++) {
		for (int i : ggg) {
			
			
			
			//initialization of steiner tree
			ptr_SteinerTree 
				st = std::make_shared<SteinerTree> (m_network->getNumberNodes (), 
													m_groups[i]->getSource(), 
													m_groups[i]->getMembers ()
													);
		
			//verify the graph for connectivity
#ifdef CONG
			ec.connected_level ( *m_groups[i].get() , *m_network);
#endif
			
#ifdef RES_CAP
			ec.connected_level_by_group ( *m_groups[i].get() , *m_network);
#endif
			
			//building the tree
			build_tree (i, st, ec);
			
			//if the i-th tree is the best found util now
			//update the edges locally
			if (m_best_trees[i] > st->getCost ()) {
				m_best_trees[i] = st->getCost ();
				
				local_update (st.get());
			}
			
			solutions.push_back (*st.get());
			
			//updating congestion heap
			//update_congestion (st, ec, cost, congestion);
			
			int trequest = m_groups[i]->getTrequest ();
			update_congestion (st, ec, cost, congestion, trequest);
		
//showing the tree in debug mode
#ifdef DEBUG1
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
	
		}//end for in solutioon construction
	
	
		/*used for congestion*/
#ifdef CONG
		if ((cost < m_bcost && congestion <= m_bcongestion) ||
			(cost <= m_bcost && congestion < m_bcongestion)) 
		{
			m_bcost = cost;
			m_bcongestion = congestion;
			//updating the pheromene
			update_pheromone_matrix (ec);
			
			m_best_iter = iter;
			
			bestNLinks = solutions;
		}
#endif
		
#ifdef RES_CAP

// 		printf ("Current iter(%d)\n", iter);
// 		printf ("\tcurrent cost (%lf) Cap(%lf)\n", cost, congestion);
// 		printf ("\tcurrent Best_cost (%lf) BestCap(%lf)\n", m_bcost, m_bcongestion);
		
		/*used for residual capacity*/
		if (congestion > m_bcongestion || 
			(congestion == m_bcongestion && cost < m_bcost))
		{
			m_bcost = cost;
			m_bcongestion = congestion;
			//updating the pheromene
			update_pheromone_matrix (ec);
			
			m_best_iter = iter;
			
			bestNLinks = solutions;

		//	printf ("ITER (%d)",iter);
 		//	printf ("\tUPDATE - Best (%lf), Cost (%lf)\n",m_bcongestion,m_bcost);
			
		}
#endif
		//clean the network
		m_network->clearRemovedEdges ();
	
	}
	
	time_elapsed.finished ();
	
#ifdef DEBUG1
	std::cout << "------------------------------" << std::endl;
#endif
	
	std::cout << m_bcongestion << "\t";
	std::cout << m_bcost << "\t";
	std::cout << m_best_iter << "\t";
	std::cout << time_elapsed.get_elapsed () << "\t";
	std::cout << m_seed << "\t";
	
	int g=0;
	auto it = bestNLinks.begin ();
	for (; it != bestNLinks.end(); it++) {
		Edge * e = (*it).listEdge.first();
		int i=0;
		while (e != NULL) {
			i++;
			//printf ("%d - %d:%d;\n", e->i+1,e->j+1,g+1);
			e = e->next;
		}
		g++;
		//std::cout << i << "\t";
	}
	std::cout << std::endl;
	
	
}

void AcoMPP::configurate (std::string m_instance) 
{

#ifdef DEBUG
	std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
#endif 
	
	MultipleMulticastReader r(m_instance);
	
	//creating the network object
	m_network = new rca::Network;
	
	//configuring the network object
	//r.configNetwork (m_network);
	//creating the groups
	//m_groups = r.readerGroup ();
	
#ifdef MODEL_REAL	
	r.configure_real_values (m_network, m_groups);	
#endif
	
	/*Atribui valor 1 para traffic request (tk) e 
	 a capacidade das arestas é igual ao tamanho do grupo.*/
#ifdef MODEL_UNIT
	r.configure_unit_values (m_network,m_groups);
#endif
	
	
	//matrix of Pheromene
	int NODES = m_network->getNumberNodes();
	m_pmatrix = PheromenMatrix(NODES);
	for (unsigned i = 0; i < m_pmatrix.size (); i++) {
		m_pmatrix[i] = std::vector<double>(NODES, 0.5);
	}
	
	//initialization of pheromene rate
	m_phe_rate = 0.9;
	m_alpha = 0.8;
	m_betha = 0.2;
	
	//initialization of random number genarator
	m_seed = rca::myseed::seed();
	my_random = Random(m_seed,1, 10);	
	
	//used to register the best values of each tree
	double max = std::numeric_limits<double>::max();
	
#ifdef RES_CAP
	double min = std::numeric_limits<int>::min();
#endif
	
	//store the best steiner tree found during the search
	m_best_trees = std::vector<double> (m_groups.size(),max) ;
	
	//initilize the congestion and the cost with max double values
	m_bcost = max;
	
#ifdef RES_CAP	
	m_bcongestion = min;
#endif

#ifdef CONG
	m_bcongestion = max;
#endif	

	//store the best iteratios, where the best solution was found
	m_best_iter = 0;
	
#ifdef DEBUG
	std::cout << "------------------------------" << std::endl;
#endif
	
	
	/* just for test*/ 
	viewer = TreeBuildViewer (m_network);
	//viewer.draw ("");
}

void AcoMPP::create_ants_by_group (int g_id, 
								   std::vector<rca::Ant> & pool, 
								   std::vector<int> & visited)
{
	
#ifdef DEBUG
	std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
#endif 
	
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

void AcoMPP::configurate2 (std::string file)
{
	
#ifdef DEBUG
	std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
#endif 
	
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
	
	//matrix of Pheromene
	int NODES = m_network->getNumberNodes();
	m_pmatrix = PheromenMatrix(NODES);
	for (unsigned i = 0; i < m_pmatrix.size (); i++) {
		m_pmatrix[i] = std::vector<double>(NODES, 0.5);
	}
	
	//initialization of pheromene rate
	m_phe_rate = 0.9;
	m_alpha = 0.8;
	m_betha = 0.2;
	
	//initialization of random number genarator
	long seed = rca::myseed::seed();
	my_random = Random(seed,0.0, 1.0);
	
	//used to register the best values of each tree
	double max = std::numeric_limits<double>::max();
	m_best_trees = std::vector<double> (m_groups.size(),max) ;
	m_bcost = max;
	m_bcongestion = max;
	
#ifdef DEBUG
	std::cout << "------------------------------" << std::endl;
#endif
	
}

/**
 * private methods here
 */

int AcoMPP::select_ant_id (const std::vector<Ant>& pool, const int & next_id)
{

#ifdef DEBUG
	std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
#endif 
	
	//seaching for the position of the ant that found next
	for (unsigned i=0; i < pool.size(); i++) {

		if (pool[i].get_id() == next_id) {
					
			return i;
		}
	}
	
	
	#ifdef DEBUG
		std::cout << "------------------------------" << std::endl;
	#endif
	return -1;
	
}

void AcoMPP::join_ants (std::vector<Ant>& pool, 
						const int& in, 
						const int& join, 
						std::vector<int>& visited)
{
#ifdef DEBUG
	std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
#endif 
	
	//making the join
	pool[in].join ( pool[join] );
	//marking the vertex
	auto cbegin = pool[in].nodes_begin ();
	auto cend = pool[in].nodes_end();
	for (; cbegin != cend; cbegin++) {
		visited[*cbegin] = pool[in].get_id ();
	}
	pool.erase (pool.begin () + join);
	
#ifdef DEBUG
	std::cout << "------------------------------" << std::endl;
#endif
	
}

void AcoMPP::update_congestion (std::shared_ptr<SteinerTree>& st,
							EdgeContainer<Comparator, HCell> & ec, 
							double&m_cost,
							double& m_congestion,
							int trequest)
{
#ifdef DEBUG
	std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
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
#ifdef CONG
			(*((ec.m_ehandle_matrix[x][y]).second)).setValue (valor+1);
#endif
			
#ifdef RES_CAP
			(*((ec.m_ehandle_matrix[x][y]).second)).setValue (valor-trequest);
#endif
			
			ec.m_heap.update ((ec.m_ehandle_matrix[x][y]).second);

#ifdef CONG			
 			if (valor+1 > m_congestion) {
 				m_congestion = (valor + 1);
 			}
#endif 
		
#ifdef RES_CAP
			if ( (valor-trequest) < m_congestion) {
 				m_congestion = (valor - trequest);
 			}
#endif
			
			
		} else {
			ec.m_ehandle_matrix[x][y].first = true;
			
#ifdef CONG
			link.setValue (1);
#endif
			
#ifdef RES_CAP
			int band = m_network->getBand (x,y);
			link.setValue ( band - trequest);
#endif
			
			ec.m_ehandle_matrix[x][y].second = ec.m_heap.push (link);
			
			m_cost += m_network->getCost (link.getX(), link.getY());
			
		}
		
		e = e->next;
	}	
	e = NULL;
	
	//TODO ISTO É APENAS PARA CASO DA CAPACIDADE RESIDUAL COM TK=1f
 	if (m_congestion == INT_MAX) {
		m_congestion = ec.m_heap.ordered_begin ()->getValue ();
	}
	
#ifdef DEBUG
	std::cout << "------------------------------" << std::endl;
#endif
	
}


void AcoMPP::update_pheromone_matrix (EdgeContainer<Comparator,HCell> & ec)
{
	
#ifdef DEBUG
	std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
#endif 
	
	auto it = ec.m_heap.ordered_begin ();
	for (; it != ec.m_heap.ordered_end(); it++) {
	
		//aqui soma 10 do valor de feromônio ao 
		//valor atual
		double v = m_pmatrix[it->getX()][it->getY()];
		m_pmatrix[it->getX()][it->getY()] += (1-m_phe_rate)*v;
		
	}
	
}

void AcoMPP::local_update (SteinerTree * st) 
{
#ifdef DEBUG
	std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
#endif

	Edge * e = st->listEdge.head;
	while (e != NULL) {
	
		rca::Link link (e->i, e->j, 0.0);
		double value = m_pmatrix[link.getX()][link.getY()];
		m_pmatrix[link.getX()][link.getY()] += value * m_local_upd;
		
		e = e->next;
	}
	
	
#ifdef DEBUG
	std::cout << "------------------------------" << std::endl;
#endif
	
}

int AcoMPP::next_component (int c_vertex, std::vector<rca::Link>& toRemove)
{

#ifdef DEBUG
	std::cout << __FUNCTION__ << ":" << __LINE__;
#endif 
	
	typedef typename std::vector<int>::const_iterator c_iterator;
		
	//double r = (double)my_random.rand ()/10.0;
	double r = (double) (rand() % 100 +1)/100.0;
	
	double best = std::numeric_limits<double>::min ();
	int returned = c_vertex;
	
	if (r <= m_heuristic_prob) {
		//TODO develop the code no qual a solução com mais feromônio.
		std::pair<c_iterator, c_iterator> iters;
		m_network->get_iterator_adjacent (c_vertex, iters);
		
		auto begin = iters.first;
		
		//this takes O(adj(c_vertex)) == O(V)
		while (begin != iters.second) {
			
			//this it has the value of the c_{c_vertex}^{j} component
			rca::Link link(c_vertex, *begin, 0.0);
			
			//se o link candiato não estiver removido verifica a chance de escolhe-lo
			//this takes O(E)
			if (std::find (toRemove.begin (), toRemove.end(), link) == toRemove.end()) 
			{
				double value = 0.0;
				
				//sum_{c_vertex}^{k} for all permited
				auto perc = iters.first;
				for (; perc != iters.second; perc++) {
			
					rca::Link lk (c_vertex, *perc, 0.0);
					
					//pulando nó c_ij = c_ik
					if (*begin == *perc) {
						continue;
					} else if (std::find (toRemove.begin (), toRemove.end(), lk) == toRemove.end()){
						
						
						double heur = m_network->getCost (lk.getX(), lk.getY());
						//valor heurístico igual para todos
						value = value +  (m_pmatrix[lk.getX()][lk.getY()] * (1/heur));
						
					}
				}
				
				//calculating the prob for c_vertex, begins
				double phe = m_pmatrix[link.getX()][link.getY()];

				//getting the cost of edge to used as a heuristic information
				double heur = m_network->getCost (link.getX(), link.getY());
					
				double denominador = pow( phe , m_alpha) * pow( (1/heur) ,m_betha);
				
				if ( ((double)denominador/value) > best) {
					best = ((double)denominador/value);
					//std::cout << "Updating best: " << std::fixed << best << std::endl;
					returned = *begin;
				}
			}
			
			begin++;
		}
		
#ifdef DEBUG
	std::cout << " Using Heuristic Informations\n";
#endif
		
		return returned;
	} else {
		#ifdef DEBUG
			std::cout << " Using greedy procedure\n";
		#endif
		return m_network->get_adjacent_by_minimun_cost (c_vertex, toRemove);
	}
	
#ifdef DEBUG
	std::cout << "------------------------------" << std::endl;
#endif
	
}

void AcoMPP::print_results () {

	std::cout << m_bcongestion << " ";
	std::cout << m_bcost << " ";
	std::cout << m_best_iter << "\n";
	
	/*
	auto it = m_best_trees.begin ();
	for ( ; it != m_best_trees.end(); it++) {
		std::cout << *it << "\n";
	}*/
	
	
}
