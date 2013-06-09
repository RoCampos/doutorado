#include "transgenetic.h"

void Transgenetic::initialization () {

	//função definida em heuristic.h
	initPopulation (m_popsize, m_population);
	m_cost = 99999;

}

void Transgenetic::createPopulation () {

	for (unsigned int i=0; i < m_population.size (); i++) {
	
		//função definida em heuristic.h
		createSolution (&m_population[i]);
		if (m_population[i].getCost () < m_cost)  {
			m_cost = m_population[i].getCost ();
			m_best = i;
		}
	}
}

void Transgenetic::printPop () {
	
	for (unsigned int i=0; i < m_population.size (); i++) {
			
		std::cout << m_population[i].getCost () << endl;

	}

}

bool Transgenetic::update (int pos, SteinerTree & st) {

	if (st.getCost () < m_cost)  {
		m_cost = st.getCost ();
		m_best = pos;
		m_population[pos] = st;

		hasImproved++;

		return true;
	} else if (st.getCost () < m_population[pos].getCost () ) {
		m_population[pos] = st;

		return true;
	}

	return false;
}

/**
*
*  Este plasmidio consiste em sortear 2 terminais em seguida
* sortear um caminho entre os dois terminais. Este caminho é
* inserido na solução.
*
*
**/
void Transgenetic::plasmidio (int pos) {

	int t1 = rand () % m_terminals->getSize ();
	int t2 = rand () % m_terminals->getSize ();

	while (t1 == t2) {
		t2 = rand () % m_terminals->getSize ();	
	}

	int path = rand () % m_host[t1][t2].size ();

	std::vector<rca::Link> edges;

	//getting the edges from individual pos
	getEdgesFromIndividual (m_population[pos], edges);

	//getting edges from the path		
	getEdgesFromPaths (t1, t2, path, edges);
		
	std::sort (edges.begin (), edges.end());
	
	SteinerTree st ( 1 , m_network->getNumberNodes() );
	initSolution (&st);
	
	//this functions is in heuristic files.
	createSolution (st, edges);	

	//verify it the new solution is the best so far,
	//or better than his "father".
	update (pos,st);
	
}

/**
* Este plamidio é semelhante ao anterior.
* Sorteia-se dois terminais. Em seguida, aplica-se
* todos os caminhos entre estes dois terminais a um 
* indivíduo.
*/
void Transgenetic::plasmidio2 (int pos) {

	//getting the paths
	int t1 = rand () % m_terminals->getSize ();
	int t2 = rand () % m_terminals->getSize ();

	while (t1 == t2) {
		t2 = rand () % m_terminals->getSize ();	
	}

	for ( unsigned int path_i=0; path_i <  m_host[t1][t2].size (); path_i++) {
			
		std::vector<rca::Link> edges;

		//getting the edges from individual pos from m_population
		getEdgesFromIndividual (m_population[pos], edges); //from heuristic.h
		
		//adiciona novas aresatas do caminho selecionado
		getEdgesFromPaths (t1, t2, path_i, edges); //from heuristic.h
		
		std::sort (edges.begin (), edges.end());
	
		SteinerTree st ( 1 , m_network->getNumberNodes() );
		initSolution (&st); //from heuristic.h
	
		createSolution (st, edges); //from heuristic.h
	
		update ( pos, st );
		
	}		
}


/**
*		
*	Igual ao 2, mas guarda os melhores caminhos.
*
*
*
*/
void Transgenetic::plasmidio3 (int pos) {

	int t1 = rand () % m_terminals->getSize ();
	int t2 = rand () % m_terminals->getSize ();

	while (t1 == t2) {
		t2 = rand () % m_terminals->getSize ();	
	}

	int path = rand () % m_host[t1][t2].size ();

	std::vector<rca::Link> edges;

	//getting the edges from individual pos
	getEdgesFromIndividual (m_population[pos], edges); //from heuristic.h

	//getting edges from the path		
	getEdgesFromPaths (t1, t2, path, edges); //from heuristic.h
		
	std::sort (edges.begin (), edges.end());
	
	SteinerTree st ( 1 , m_network->getNumberNodes() );
	initSolution (&st); //from heuristic.h
	
	createSolution (st, edges);	//from heuristic.h
	
	if ( update ( pos, st) )
		m_best_paths.insert ( m_host[t1][t2][path] );

}

/*
* Cria um pedaço de solução da seguinte forma:
* Escolhe um vértice de Steiner e dois terminais.
* Cria-se dois caminhos entre o vértice de Steiner
* e os dois terminais. Um caminho saindo vértice para cada
* um dos terminais. Este caminho são juntados e adicionados
* a solução.
*/
void Transgenetic::plasmidioRecombinado (int pos) {


	int node = rand () % m_network->getNumberNodes ();
	
	while (_nodes[node].isTerminal ()) 
		node = rand () % m_network->getNumberNodes ();

	int t1 = rand () % m_terminals->getSize ();
	int t2 = rand () % m_terminals->getSize ();

	while (t1 == t2) {
		t2 = rand () % m_terminals->getSize ();	
	}

	rca::Path path1 = shortest_path (node, t1, m_network);
	rca::Path path2 = shortest_path (node, t2, m_network);
	
	std::set<rca::Path> paths;
	paths.insert (path1);
	paths.insert (path2);

	std::vector <rca::Link> edges;	
	getPathsFromSet (paths, edges); //from heuristic.h

	//getting the edges from individual pos
	getEdgesFromIndividual (m_population[pos], edges); //from heuristic.h
		
	std::sort (edges.begin (), edges.end());
	
	SteinerTree st ( 1 , m_network->getNumberNodes() );
	initSolution (&st); //from heuristic.h
		
	createSolution (st, edges);	 //from heuristic.h
	
	update ( pos, st);
	
}

void Transgenetic::run (int generation) {

	
	createPopulation ();
	while (generation-- > 0) {

		for (unsigned int i=0; i < m_population.size (); i++) {
			plasmidio (i);
		}

	}

}

void Transgenetic::run2 (int generation) {

	
	createPopulation ();
	while (generation-- > 0) {
		
		for (unsigned int i=0; i < m_population.size (); i++)
			plasmidio2 (i);

	}
}

//need parameter
void Transgenetic::run3 (int generation, double prob) {

	cout << prob << endl;	

	createPopulation ();
	while (generation-- > 0) {

		for (unsigned int i=0; i < m_population.size (); i++) {

			int prob_t = rand () % 10 + 1; 

			if ( (double)prob_t/10 < prob)
				plasmidio2 (i);
		}
	}
}

//need parameter
void Transgenetic::run4 (int generation, double prob) {
	
	createPopulation ();
	while (generation-- > 0) {

		for (unsigned int i=0; i < m_population.size (); i++) {

			int prob_res = rand () % 10 + 1; 

			prob_res = (double)prob_res/10;
			if (prob_res < prob)
				plasmidio3 (i);
			else {

				std::vector<rca::Link> edges;
				getEdgesFromIndividual (m_population[i], edges);

				//usando melhores arestas	
				getPathsFromSet (m_best_paths, edges);

				std::sort (edges.begin (), edges.end());
	
				SteinerTree st ( 1 , m_network->getNumberNodes() );
				initSolution (&st); //from heuristic.h
	
				createSolution (st, edges);	//from heuristic.h

				update ( i , st );

			} //end else
		} //end for
	}

}

void Transgenetic::transposson (int pos) {

	
}

void Transgenetic::run5 (int generation) {

	createPopulation ();
	while (generation-- > 0) {

		for (unsigned int i=0; i < m_population.size (); i++) {
			plasmidioRecombinado (i);
		}
	}

}

void Transgenetic::run6 (int generation, double prob) {

	createPopulation ();
	while (generation-- > 0) {
	

		unsigned int size = m_population.size () / 3;

		for (unsigned int i=0; i < m_population.size (); i++)	{


			if (i < size) {
				int prob_res = rand () % 10 + 1; 
				prob_res = (double)prob_res/10;
				if (prob_res < prob)			
					plasmidio3 (i);
				else {

					std::vector<rca::Link> edges;
					getEdgesFromIndividual (m_population[i], edges);

					//usando melhores arestas	
					getPathsFromSet (m_best_paths, edges);

					std::sort (edges.begin (), edges.end());
	
					SteinerTree st ( 1 , m_network->getNumberNodes() );
					initSolution (&st); //from heuristic.h
	
					createSolution (st, edges);	//from heuristic.h

					update ( i , st );

				}		

			}

			if (i > size && i < 2*size ) {
				
				plasmidioRecombinado (i);
			}

			if (i > 2*size) {
				
				plasmidio2 (i);
			}

		}
	}

}

void Transgenetic::bestInfo () {

	cout << m_population[m_best].getCost () << endl;

	//cout << "Has Improved: " << hasImproved << endl;

}
