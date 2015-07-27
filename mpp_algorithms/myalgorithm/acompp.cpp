#include "acompp.h"

using namespace rca;

void AcoMPP::build_tree (int id, 
						 STTree & st, 
						 EdgeContainer<Comparator,HCell> & ec) 
{
#ifdef DEBUG1
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
		
		int next = next_component (c_vertex, toRemove, ec);
			
		//if next == -1 é necessário mudar a forrmiga de lugar
		if (next != -1) {
			
			if ( (visited[next] > -1) && (visited[c_vertex] != visited[next]) ) {
				//moving the ant
				pool[ant].move (next);
			
				rca::Link link (c_vertex, next, 0.0);
				//m_network->removeEdge(link);
				toRemove.push_back (link);
				
				//adding the edges to st structure
				st.add_edge (link.getX(), link.getY(), 
				(int)m_network->getCost(link.getX(), link.getY()) );
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
					
				st.add_edge (link.getX(), link.getY(), 
					(int)m_network->getCost(link.getX(), link.getY()) );
				
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
	
	st.prunning ();

#ifdef VIEWER
	viewer.draw ("../teste");	
	std::cout << "drawing tree\n";
	getchar ();
	viewer.clear ();
#endif
	
#ifdef DEBUG1
	std::cout << "------------------------------" << std::endl;
#endif
	
}

void AcoMPP::run (va_list & arglist) {

#ifdef DEBUG1
	std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
#endif
	
	int iterations = va_arg(arglist, int);
	m_alpha = va_arg (arglist, double);
	m_betha = va_arg (arglist, double);
	m_phe_rate = va_arg (arglist, double);
	m_heuristic_prob = va_arg (arglist, double);
	m_local_upd = va_arg(arglist,double);
	m_ref = va_arg (arglist, double);
	
	m_budget = va_arg (arglist, double);
	if (m_budget == 0) {
		m_budget = INT_MAX;
	}
	
	//novos parâmetros
	double upd1 = va_arg (arglist, double);
	double upd2 = va_arg (arglist, double);
	double upd3 = va_arg (arglist, double);

	double m_flip = va_arg (arglist, double);
	
	m_res = va_arg (arglist, int);
//  	std::cout << m_res << std::endl;
	
	std::vector<STTree> bestNLinks;
	
	m_test = true;
	int count_iter = iterations*m_flip;
// 	int count_iter = iterations*0.025;
// 	int aux = m_budget*1.15;
	
	time_elapsed.started ();
	for (int iter =0; iter < iterations; iter++) {
		
		count_iter--;
		if (count_iter == 0) {
			
			if (!m_test) {
				
				m_test = true;
 			} else {				
				m_test = false;
 			}
			
			count_iter = iterations*m_flip;
		
		}		
		
		//initialization of the strutctures that suppor congestion evaluation
		EdgeContainer<Comparator,HCell> ec;
		ec.init_congestion_matrix (m_network->getNumberNodes ());
		ec.init_handle_matrix (m_network->getNumberNodes ());
	
		std::vector<STTree> solutions;
		solutions.resize (m_groups.size (), STTree());
		
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
		int CONT = 0;
		for (int i : ggg) {
			
			STTree st(m_network->getNumberNodes (),
					  m_groups[i]->getSource(),
					  m_groups[i]->getMembers());
		
			//verify the graph for connectivity
#ifdef CONG
			ec.connected_level ( *m_groups[i].get() , *m_network);
#endif
			
#ifdef RES_CAP
			ec.connected_level_by_group ( *m_groups[i].get() , *m_network);
#endif
			
			//building the tree
			build_tree (i, st, ec);
			
			solutions[i] = st;
			
			int trequest = m_groups[i]->getTrequest ();
			update_congestion (st, ec, cost, congestion, trequest, CONT);
			CONT++;
			
		
//showing the tree in debug mode
#ifdef DEBUG1
	std::cout << st.getCost () << std::endl;
    std::string file ="/home/romeritocampos/workspace/Doutorado/inst_test/aco-test";
    file = file+"/saida.xdot";
    std::cout << file << std::endl;
    st.xdotToFile (file);
    std::string cmd ("xdot");
    cmd += " " + file;
	system (cmd.c_str ());
	getchar ();
#endif
	
		}//end for in solutioon construction
		

/*-------------------------------------------------------*/			
	bool improve = true;
	int tmp_cong = ec.top();
	
	std::vector<STTree> stts = solutions;
	sttalgo::ChenReplaceVisitor c(&stts);
	c.setNetwork (m_network);
	c.setMulticastGroups (m_groups);
	c.setEdgeContainer (ec);
	
//  	while (improve) {
// 				
// 		this->accept (&c);
// 				
// 		int temp_cost = 0;
// 		for (auto st : stts) {
// 			temp_cost += (int)st.getCost ();
// 		}
// 		
// 		if (ec.top () > tmp_cong) {
// 			congestion = ec.top ();
// 			tmp_cong = congestion;
// 			cost = temp_cost;
// 			
// 			solutions.clear ();
// 			solutions = stts;
// 		} else {
// 			improve = false;
// 		}
// 		
//  	}
	
	double r = (double) (rand() % 100 +1)/100.0;
	
	if (r < this->m_ref) {
		c.visitByCost ();
		int tt = 0.0;
		for (auto st : stts) {
			tt += (int)st.getCost ();
		}
		solutions = stts;
		cost = tt;
	}
/*-------------------------------------------------------*/
	
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
		
		/*used for residual capacity*/
		bool upd_cg = congestion > m_bcongestion;
		bool upd_cost = (congestion == m_bcongestion && cost < m_bcost);
		bool upd_lim = (cost <= m_budget);
//  		std::cout << congestion << "\t" << cost << "\t" << iter <<std::endl;
		if ( (upd_cg || upd_cost) && upd_lim )
		{
			
			m_bcost = cost;
			m_bcongestion = congestion;
			//updating the pheromene
			m_best_iter = iter;
			bestNLinks.clear ();
			
			bestNLinks = solutions;
// 			update_pheromone_matrix (ec);
 			X (bestNLinks, m_best_trees, ec, upd1);
			
		} else if (upd_cg) {
		

 			X (solutions, m_best_trees, ec, upd2);
			
		} else if (upd_cost) {
			
			m_bcost = cost;
			m_bcongestion = congestion;
			//updating the pheromene
			m_best_iter = iter;
			bestNLinks.clear ();
			
			bestNLinks = solutions;
// 			update_pheromone_matrix (ec);
 			X (bestNLinks, m_best_trees, ec, upd3);

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
  	std::cout << m_seed << "\n";
	
	int g=0;	
	auto it = bestNLinks.begin ();
	for (; it != bestNLinks.end(); it++) {
// 		Edge * e = (*it).listEdge.first();
		edge_t * e = (*it).get_edge ();
		
		while (e != NULL) {
			//printf ("%d - %d:%d;\n", e->i+1,e->j+1,g+1);
			if (e->in)
// 				std::cerr << e->x+1 << " - " << e->y+1 << ":" << g+1 << std::endl;
			e = e->next;
		}
		g++;
	}

	
}

void AcoMPP::configurate (std::string m_instance) 
{

#ifdef DEBUG1
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
	//my_random = Random(1428412957609586079,1, 10);	
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
	
#ifdef DEBUG1
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
	
#ifdef DEBUG1
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
	
#ifdef DEBUG1
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
	
#ifdef DEBUG1
	std::cout << "------------------------------" << std::endl;
#endif
	
}

/**
 * private methods here
 */

int AcoMPP::select_ant_id (const std::vector<Ant>& pool, const int & next_id)
{

#ifdef DEBUG1
	std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
#endif 
	
	//seaching for the position of the ant that found next
	for (unsigned i=0; i < pool.size(); i++) {

		if (pool[i].get_id() == next_id) {
					
			return i;
		}
	}
	
	
	#ifdef DEBUG1
		std::cout << "------------------------------" << std::endl;
	#endif
	return -1;
	
}

void AcoMPP::join_ants (std::vector<Ant>& pool, 
						const int& in, 
						const int& join, 
						std::vector<int>& visited)
{
#ifdef DEBUG1
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
	
#ifdef DEBUG1
	std::cout << "------------------------------" << std::endl;
#endif
	
}

void AcoMPP::update_congestion (STTree& st,
							EdgeContainer<Comparator, HCell> & ec, 
							double&m_cost,
							double& m_congestion,
							int trequest,
							int CONT
   							)
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
#endif 
	
// 	Edge * e = st.listEdge.head;
	edge_t *e =st.get_edge ();
	while (e != NULL) {
	
		//aresta removida(prunning)
		if (!e->in) {
			e = e->next;
			continue;
		}
		
		//removing the edges from network
		//m_network->removeEdge ();
	
		//NÃO ESTÁ FUNCIONANDO!!!
		//TENHO QUE PROVER OUTRO MEIO PARA MANTER AS ARESTAS EM NÍVEIS
		rca::Link link (e->x, e->y, 0);
		int x = link.getX ();
		int y = link.getY ();
		
		//computing the edge usage(cost)
		m_cost += (int)m_network->getCost (link.getX(), link.getY());
		
		if (ec.m_ehandle_matrix[x][y].first == true) {
			
			int valor = ec.value (link);
#ifdef CONG
			(*((ec.m_ehandle_matrix[x][y]).second)).setValue (valor+1);
#endif
			
#ifdef RES_CAP
			link.setValue ( valor - 1 );
#endif
			
			ec.update (link);

#ifdef CONG			
 			if (valor+1 > m_congestion) {
 				m_congestion = (valor + 1);
 			}
#endif 
		
#ifdef RES_CAP
			m_congestion = ec.top ();
#endif
			
			
		} else {
			
#ifdef CONG
			link.setValue (1);
#endif
			
#ifdef RES_CAP
			int band = m_network->getBand (x,y);
			link.setValue ( band - trequest);
#endif
			
			ec.push (link);
			
		}
		
		e = e->next;
	}	
	e = NULL;
	
	//TODO ISTO É APENAS PARA CASO DA CAPACIDADE RESIDUAL COM TK=1f
 	if (m_congestion == INT_MAX) {
		m_congestion = ec.top ();//ec.m_heap.ordered_begin ()->getValue ();
	}
	
#ifdef DEBUG1
	std::cout << "------------------------------" << std::endl;
#endif
	
// 	if (!test)
	if (CONT >=1 ) {
		
		int res = ec.top ();
		
//  		std::cout << res << " " << res + m_res << "res:" << m_res<< std::endl;
		
		auto it = ec.get_heap ().ordered_begin ();
		auto end = ec.get_heap ().ordered_end ();
		for ( ; it != end && it->getValue () <= (res+m_res); it++) {
			this->m_network->removeEdge (*it);
		}
	}
	
}


void AcoMPP::update_pheromone_matrix (EdgeContainer<Comparator,HCell> & ec)
{
	
#ifdef DEBUG1
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

void AcoMPP::X (std::vector<STTree>& trees, 
				std::vector<double>& trees_cost,
				EdgeContainer<Comparator,HCell> & ec, 
				double scale)
{

 	double max_res = m_groups.size ();
	
	auto it = ec.get_heap ().ordered_begin();
	auto end = ec.get_heap ().ordered_end();
	
	for ( ; it != end; it++) {
		
// 		double phe =  (1/ (max_res - it->getValue()));
		double phe = (it->getValue() - ec.top())/max_res;
		double phe_cost = 0.0;
		
		int count_usage = 0;
		
		for (size_t j = 0; j < trees.size (); j++) {
			auto st = trees[j];
			
			edge_t * e = st.get_edge ();
			while (e != NULL) {
				if (e->in) {
					rca::Link l(e->x, e->y, 0);
					if (l == *it) {
						
						double cost = (int)this->m_network->getCost (l.getX(), 
																	 l.getY());							
						if (st.getCost () < trees_cost[j]) {
							count_usage++;
							phe_cost += (10/(double)cost);
						} /*else {
							phe_cost += (1/(double)cost);
						}*/
					}
				}
				e = e->next;
			}			
		}
		
		phe_cost /= count_usage;

		m_pmatrix[ it->getX()][ it->getY() ] += scale*phe + phe_cost; 
	
	}
	
	int i=0;
	for (auto st : trees) {
		if (st.getCost () < trees_cost[i]) {
			trees_cost[i];
		}			
		i++;
	}
	
}

void AcoMPP::local_update (STTree & st) 
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
#endif

// 	Edge * e = st.listEdge.head;
	edge_t *e = st.get_edge ();
	while (e != NULL) {
	
		rca::Link link (e->x, e->y, 0.0);
		double value = m_pmatrix[link.getX()][link.getY()];
		m_pmatrix[link.getX()][link.getY()] += value * m_local_upd;
		
		e = e->next;
	}
	
	
#ifdef DEBUG1
	std::cout << "------------------------------" << std::endl;
#endif
	
}

int AcoMPP::next_component (int c_vertex, 
							std::vector<rca::Link>& toRemove,
							EdgeContainer<Comparator,HCell> & ec)
{

#ifdef DEBUG1
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
 					
					returned = *begin;
				}
			}
			
			begin++;
		}
// 		getchar ();
#ifdef DEBUG1
	std::cout << " Using Heuristic Informations\n";
#endif
		
		return returned;
	} else {
		#ifdef DEBUG1
			std::cout << " Using greedy procedure\n";
		#endif
		
		if (m_test) {
			typedef std::vector<int>::const_iterator adj_iter;
			std::pair<adj_iter, adj_iter> iters;
			std::vector<rca::Link> list;
			
			this->m_network->get_iterator_adjacent(c_vertex, iters);
			auto begin = iters.first;
			for ( ; begin != iters.second; begin++) {
				int x = c_vertex;
				int y = *begin;
				rca::Link l (x,y,0);
				if (!this->m_network->isRemoved(l)) {
					if (ec.is_used (l)) {
						l.setValue (ec.value (l));
					} else {
						l.setValue (this->m_groups.size ());
					}

					list.push_back (l);
				}
			}
			std::sort (list.begin (), list.end(), std::greater<rca::Link>());

			return ( (list.size () > 0) ? (list[0].getX() == c_vertex ? list[0].getY() : list[0].getX()) : -1);
		} else {
			return m_network->get_adjacent_by_minimun_cost (c_vertex, toRemove);
		}
	}
	
#ifdef DEBUG1
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
