#include "genetic_algorithm.h"
#include "metaheuristic.h"

//global variables
int path_size;
std::vector<ListPath> g_paths;  //all paths
std::vector<Tuple> g_members_info; //all members info
int used_list; //size of list in operator1

void GeneticAlgorithm::run_metaheuristic (std::string instance, int budget)
{
	/*configuring the problem*/
	m_instance = instance;
	m_network = new Network;
	Reader r (m_instance);
	r.configNetwork ( m_network );
	std::vector<shared_ptr<rca::Group>> g = r.readerGroup ();
	for (unsigned int i =0; i < g.size (); i++) {
		m_groups.push_back (*g[i].get ());
	}
	
	/*Lista de caminhos used in init_rand_solution3*/
	g_paths = k_paths (m_network, m_groups, path_size);	
	/*lista de genes used in init_rand_solution3*/
	g_members_info = create_members_info (m_groups);
	
	/*orçamento*/
	m_budget = budget;
	
	/*init population*/
	init_population ();
	
	while ( --m_iter > 0) {
		
		//crossover
		for (unsigned int pop = 0; pop < m_population.size (); pop+=4) {
			
			int i = selection_operator (pop, pop+1);
			int j = selection_operator (pop+2, pop+3);
				
			//one point crossover
			double cross_rate = (double)(rand () % 100 + 1)/100.0;
			if (cross_rate < m_cross) {
				crossover(i,j);
			}
		}
			
		//applying mutation as operator1
		for (int i=0; i < (int)m_population.size ();i++) {
			
			double cross_rate = (double)(rand () % 100 + 1)/100.0;
			if (cross_rate < m_mut) {				
				m_population[i].operator1 (m_network, m_groups);
			}
		}

	}
	int best = 0;
	int max = m_population[0].m_residual_capacity;
	for (int i=1; i < (int)m_population.size (); i++)
	{
		if (m_population[i].m_residual_capacity > max)
		{
			
			if (m_population[i].m_cost < m_budget) {
				max = m_population[i].m_residual_capacity;
				best = i;
			}
		}
	}
	
//#ifdef DEBUG1
	//std::cout << "Best: ";
	//std::cout << m_population[best].m_cost << " ";
	std::cout << m_population[best].m_residual_capacity << std::endl;
 	//m_population[best].print_solution (m_network, m_groups);

// 	for (const rca::Link &l : m_population[best].getCongestionHandle().getUsedLinks()){
// 		std::cout << l << " :" << l.getValue () << std::endl;
// 	}
	
	//deallocatin of resources;
#ifdef DEBUG1
	for (PathRepresentation & p : m_population) {
		std::cout << p.m_cost << " ";
		std::cout << p.m_residual_capacity << std::endl;
	}
#endif
	
	
	delete m_network;
}

int GeneticAlgorithm::selection_operator (int i, int j)
{

	//retorna melhor individual com base na capacidade residual
	return (m_population[i].m_residual_capacity > 
			m_population[j].m_residual_capacity ?
		   i : j);
}

void GeneticAlgorithm::init_population ()
{
#ifdef DEBUG
	std::cout << "Creating Population: size = " << m_pop << std::endl;	
#endif
	
#ifdef DEBUG1
	int best = -1;	
#endif
	
	int max = INT_MIN;
	int cost = 10000000;
	m_population = std::vector<PathRepresentation> (m_pop);
	for (int i=0; i < m_pop; i++) {

		if (m_init == 1)
			m_population[i].init_rand_solution1 (m_network, m_groups);
		else if (m_init == 2)
			m_population[i].init_rand_solution2 (m_network, m_groups);
		else if (m_init == 3)
			m_population[i].init_rand_solution3 (m_network, m_groups);
			
		
#ifdef DEBUG
	std::cout << m_population[i].m_cost << " ";
	std::cout << m_population[i].m_residual_capacity << std::endl;
	//m_population[i].print_solution (m_network,m_groups);
#endif
		
		//if (m_population[i].getCost () < m_budget) {
		
			if (m_population[i].getResidualCap () > max) {
			
#ifdef DEBUG1
				best = i;
#endif 
				max =  m_population[i].getResidualCap ();
				
				m_budget = m_population[i].m_cost;
			} else if (max == m_population[i].getResidualCap ()) {
			
				if (m_population[i].getCost () < cost) {
					cost = m_population[i].getCost ();
#ifdef DEBUG1
					best = i;
#endif
					m_budget = m_population[i].m_cost;
				}
			}
			
		//}
		
	}
	
#ifdef DEBUG1
	std::cout << "Best Initalization \n";
	std::cout << m_population[best].getCost () << " ";
	std::cout << m_population[best].getResidualCap () << " ";
	
	m_population[best].print_solution (m_network,m_groups);
#endif
	
}

void GeneticAlgorithm::crossover (int i, int j)
{
	
	int size = m_population[i].m_genotype.size ();
	int i_pos = rand () % size;
		
	PathRepresentation sol;
	std::vector<rca::Link> used_links;
	
	for (int gen=0; gen < size; gen++) {
		
		if ( gen <= i_pos) {
			sol.m_genotype.push_back ( m_population[i].m_genotype[gen]);
		} else {
			sol.m_genotype.push_back ( m_population[j].m_genotype[gen]);
		}
	}
	
	int GROUPS = m_groups.size ();
	int stop = 0;
	
	CongestionHandle cg(m_network, m_groups);
	sol.setCongestionHandle (cg);
	SteinerTreeObserver stObserver(NULL, &sol.getCongestionHandle ());
	for (int k = 0; k < GROUPS; k++) {
			
		int source = m_groups[k].getSource ();
		SteinerTree st (m_network->getNumberNodes (), source, m_groups[k].getMembers());
		
		stObserver.setTree(&st);
		
		int g_size = m_groups[k].getSize ();
		int gen = stop;
		stop += g_size;
		//std::cout << gen <<" " << stop << std::endl;
		for (; gen < stop; gen++) {
		
			rca::Path path = sol.m_genotype[gen];
			
			std::vector<int>::reverse_iterator it = path.rbegin ();
			for (; it!= path.rend () -1; it++) {
				
				int v = *it;
				int x = *(it+1);
				
				int cost = m_network->getCost (v,x);
				stObserver.addEdge (v, x, cost, k);
			}
		}
		
		stObserver.prunning (k);
		
	}
	
	//sol.m_used_links = used_links;
	sol.m_cost = stObserver.getCost ();
	sol.m_residual_capacity = sol.getCongestionHandle() .congestion ();
	
	//if (sol.m_cost > m_budget) return;
	
	int old = -1;
	if (m_population[i].m_residual_capacity < m_population[j].m_residual_capacity)
	{
		old = i;
	} 
	else if (m_population[i].m_residual_capacity > m_population[j].m_residual_capacity)
	{
		old = j;
	}
	
	if (old != -1) {
#ifdef DEBUG1
	std::cout << "Crossver Improvement\n"; 	
#endif
		m_population[old] = sol;
		
	}
	
}

void GeneticAlgorithm::mutation (int i) 
{
	
	auto it = m_population[i].getCongestionHandle 
						().getUsedLinks ().begin ();
	
	int j=0;
	int max =  m_population[i].getCongestionHandle 
						().getUsedLinks ().size () * 0.10; 
						
	for (; j++ < max; it++) {
		
		rca::Link l =  m_population[i].getCongestionHandle 
										().getUsedLinks ()[j];
		
		m_network->removeEdge (l);
		
	}
	
	PathRepresentation sol;
	sol.init_rand_solution1 (m_network, m_groups);
	
	if ( sol.m_cost < m_budget ) {
		
		if (sol.m_residual_capacity > m_population[i].m_residual_capacity) {
#ifdef DEBUG1
	std::cout << "Some Improvement=";
	std::cout << (sol.m_residual_capacity)<<":"<< m_population[i].m_residual_capacity;
	std::cout << " " << sol.m_cost;
	std::cout << std::endl;
#endif
			m_population[i] = sol;
		}
		
	}
	
}

/*Copy constructor*/
PathRepresentation::PathRepresentation (const PathRepresentation& ind)
{
	m_cost = ind.m_cost;
	m_residual_capacity = ind.m_residual_capacity;	
	m_feasable = ind.m_feasable;
	
	//using move from algorithm
	m_genotype = std::move (ind.m_genotype);
}

/*Assignement operator*/
PathRepresentation& PathRepresentation::operator= (const PathRepresentation& ind)
{
	m_cost = ind.m_cost;
	m_residual_capacity = ind.m_residual_capacity;	
	m_feasable = ind.m_feasable;
	
	//using move from algorithm
	m_genotype = std::move (ind.m_genotype);

	return *this;
}


void PathRepresentation::init_rand_solution1 (rca::Network * net, 
									  std::vector<rca::Group>& groups)
{
	
	/*the overall cost of solution*/
	m_cost = 0;
	
	std::vector<rca::Link> used_links;
	
	int GROUPS = groups.size ();
	
	//CongestionHandle
	CongestionHandle cg(net, groups);
	
	//the path holds the congestion Object
	this->setCongestionHandle(cg);
	
	//SteinerTreeObserver object, publish new changes
	SteinerTreeObserver stObserver(NULL, &getCongestionHandle ());
	
	for (int i=0; i < GROUPS; i++) {
		int source = groups[i].getSource ();
		
		/*Steiner tree to compute correctly cost*/
		SteinerTree st (net->getNumberNodes (), source, groups[i].getMembers());		
		stObserver.setTree (&st);
		
		rca::Path path;
		int w = groups[i].getMember (0);
		path = shortest_path (source, w, net);
#ifdef DEBUG1
		std::cout << "TREE :" << i << std::endl;
		printf ("\tpath %d to %d ", source, w);
		std::cout << path << std::endl;
#endif		
		for (int d=0; d < groups[i].getSize (); ) {
			
			d++;
			
			std::vector<rca::Link> links;
			int rnd = rand () % 10 + 1;
			//if (rnd < 8) { //garante a diversidade

				/* removing this path */
				std::vector<int>::reverse_iterator its = path.rbegin ();
				for (; its!= path.rend () -1; its++) {
					rca::Link link( (*its), *(its + 1), 0);
					
					
					if (rnd < 8) {
						net->removeEdge (link);
						links.push_back (link);
					}
					
					rca::Link link2 = link;
					int cost = net->getCost (link2.getX(),link2.getY());
					link2.setValue (cost);
					stObserver.addEdge (link2.getX(), link2.getY(), cost, i);					
					
				}
#ifdef DEBUG1
				std::cout << "\t\tremoving path ";
				std::cout << path << " size (";
				std::cout << path.size () << ")\n";
#endif
			//}
			
			m_genotype.push_back (path);
		
			if (d == groups[i].getSize ()) break;
			
			w = groups[i].getMember (d);
			path = shortest_path (source, w, net);
			
			if (path.size () == 0) {
				net->clearRemovedEdges ();
				path = shortest_path (source, w, net);
			}
			
#ifdef DEBUG1
		printf ("\tpath %d to %d ", source, w);
		std::cout << path;
		std::cout << " size= " << path.size () <<std::endl;
#endif
			
			std::vector<rca::Link>::iterator itl = links.begin ();
			for (; itl!= links.end (); itl++) {
				net->undoRemoveEdge (*itl);
			}		
		}
		
		//passing group i
		stObserver.prunning ( i );
		
#ifdef DEBUG1
		st.xdotFormat ();
		getchar ();
#endif
	
		net->clearRemovedEdges ();
	}
	
	m_cost = stObserver.getCost ();
	m_residual_capacity = m_cg.congestion ();

}

void PathRepresentation::init_rand_solution2 (rca::Network * net, 
									  std::vector<rca::Group>& groups)
{

#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif

	//observers
	std::vector<SteinerTreeObserver> treesObserver;
	CongestionHandle cg(net, groups);
	this->setCongestionHandle (cg);
	
	int gene_size = 0;
	
	//steiner trees
	std::vector<SteinerTree*> sts;
	
	//creating the StObservers && getting the size of individual
	for (Group & g : groups) {
		gene_size += g.getSize ();
		
		//creating observers
		SteinerTree *st = new SteinerTree(net->getNumberNodes (), 
					  g.getSource(), g.getMembers());
		SteinerTreeObserver ob(NULL,&this->getCongestionHandle());
		ob.setTree (st);
		treesObserver.push_back (ob);
		sts.push_back (st);
		st = NULL;
		
	}
	
	//auxiliar functins define in genetic_algorithm.h
	std::vector<int> genes = create_gene_vector (gene_size);
	
	//shuffling the order of each genes
	std::random_shuffle (genes.begin (), genes.end());
	
	m_genotype = std::vector<rca::Path> (gene_size);
	for (int i=0; i < (int)genes.size (); i++) {
		
		Tuple t = g_members_info[ genes[i] ];

#ifdef DEBUG1
		std::cout << "building path ... " << std::get<0>(t);
		printf (" from %d to %d in group %d =", std::get<2>(t),
											std::get<3>(t),
											std::get<1>(t));
#endif
		
		int gene = std::get<0>(t);
		int g = std::get<1>(t);
		int source = std::get<2>(t);
		int dest = std::get<3>(t);
		
		//parameters: v, w, network, groups
		rca::Path p = capacited_shortest_path (source, 
											   dest, 
											   net,
											   &this->getCongestionHandle(),
										       groups[ g ]);

#ifdef DEBUG1
	std::cout << p << std::endl;
#endif 
		if (p.size () == 0) {
			//net->clearRemovedEdges ();
			p = shortest_path ( source, dest, net);
		}
		
		m_genotype[gene] = p;
		
		//removing path from network
		auto it = p.rbegin ();
		
		for (; it != p.rend()-1; it++) {
			int x = *(it);
			int w = *(it+1);
			
			int cost = net->getCost (x,w);
			treesObserver[g].addEdge (x,w,cost,g);
		}
		
	}
	int i = 0;
	m_cost = 0;
	for (SteinerTreeObserver & st : treesObserver) {
		st.prunning (i++);
		m_cost += st.getCost ();
	}
	m_residual_capacity = this->getCongestionHandle().congestion ();
	
	
// 	for (rca::Path &p : m_genotype) {
// 		std::cout << p << std::endl;
// 	}
	
// 	for (const rca::Link & l: this->getCongestionHandle().getUsedLinks()){
// 		std::cout << l << " " << l.getValue()<<std::endl;
// 	}
	
}

void PathRepresentation::init_rand_solution3 (rca::Network * net,
											  std::vector<rca::Group>&groups)
{
	//observers
	std::vector<SteinerTreeObserver> treesObserver;
	CongestionHandle cg(net, groups);
	this->setCongestionHandle (cg);
	
	int gene_size = 0;
	
	//steiner trees
	std::vector<SteinerTree*> sts;
	
	//creating the StObservers && getting the size of individual
	for (Group & g : groups) {
		gene_size += g.getSize ();
		
		//creating observers
		SteinerTree *st = new SteinerTree(net->getNumberNodes (), 
					  g.getSource(), g.getMembers());
		SteinerTreeObserver ob(NULL,&this->getCongestionHandle());
		ob.setTree (st);
		treesObserver.push_back (ob);
		sts.push_back (st);
		st = NULL;
		
	}
	
	std::vector<int> genes = create_gene_vector (gene_size);
	
	//shuffling the order of each genes
	std::random_shuffle (genes.begin (), genes.end());
	
	m_genotype = std::vector<rca::Path> (gene_size);
	for (int i=0; i < (int)genes.size (); i++) {
		
		Tuple t = g_members_info[ genes[i] ];

#ifdef DEBUG1		
		std::cout << "building path ... " << std::get<0>(t);
		printf (" from %d to %d in group %d\n", std::get<2>(t),
											std::get<3>(t),
											std::get<1>(t));
#endif
		
		//getting a random path from k_paths
		int gene_id = std::get<0>(t);
		int r = rand () % g_paths[ gene_id ].size ();
		
		///getting the path based on random r
		rca::Path p = g_paths [ gene_id ][r];
		
		
		m_genotype[gene_id] = p;
		
		//removing path from network
		auto it = p.rbegin ();
		int g = std::get<1>(t); //getting the id of the group
		for (; it != p.rend()-1; it++) {
			int x = *(it);
			int w = *(it+1);
			
			int cost = net->getCost (x,w);
			
			net->removeEdge ( rca::Link (x,w,0));
			
			treesObserver[g].addEdge (x,w,cost,g);
		}
		
	}
	
	m_residual_capacity = this->getCongestionHandle().congestion ();
	m_cost = 0;
	for (SteinerTreeObserver & st : treesObserver) {
		m_cost += st.getCost ();
	}

}

void PathRepresentation::print_solution (rca::Network *net, 
										 std::vector<rca::Group> & g)
{

	int GROUPS = g.size ();
	int begin = 0;
	int end = 0;
	for (int i=0; i < GROUPS; i++) {
	
		end += g[i].getSize ();
		
		SteinerTree st (net->getNumberNodes (), 
						g[i].getSource(), 
						g[i].getMembers());
		
		/*disjoint set to avoid circle*/
		//TODO IMPLEMENTAR DENTRO DA ST_IMPLEMENTAÇÃO
		DisjointSet2 dset (net->getNumberNodes());
		
		for (int l = begin; l < end; l++) {
		
			rca::Path path = m_genotype[l];
			std::vector<int>::reverse_iterator it = path.rbegin ();
			for (; it!= path.rend () -1; it++) {
				//rca::Link link( (*it), *(it + 1), 0);
				int v = *it;
				int x = *(it+1);
				
				if ( dset.find2 (v) != dset.find2 (x) ) {
					
					dset.simpleUnion (v, x);
					
					(v > x ? st.addEdge (v, x, (int)net->getCost (v,x)):
							st.addEdge (x, v, (int)net->getCost (v,x)));
				
				}
			}
		}
		
		begin = end;
		
		st.prunning();
		
		Edge * e = st.listEdge.head;
		while ( e != NULL) {
		
			printf ("%d - %d:%d;\n", e->i+1, e->j+1, i+1); 
			
			e = e->next;
		}
		
	}
	
}

void PathRepresentation::operator1 (rca::Network *net, 
									std::vector<rca::Group> & group)
{
	
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;	
#endif
	
	int list_size = m_cg.getUsedLinks ().size () * PathRepresentation::USED_LIST;
	//std::cout << list_size << std::endl;
	//std::vector<rca::Path> tree;
	
	//removing from network
	//std::cout << "removed\n";
	for (int l = 0; l < list_size; l++) {		
		//std::cout << m_cg.getUsedLinks ()[l] << std::endl;
		net->removeEdge ( m_cg.getUsedLinks ()[l] );
	}
	
	CongestionHandle cg(net,group);
	SteinerTreeObserver stObserver(NULL, &cg);
	int end = group[0].getSize ();
	SteinerTree st (net->getNumberNodes (), 
					group[0].getSource(),
					group[0].getMembers ());
	stObserver.setTree (&st);
	int k = 0;
	
	//std::cout << "Group " << k << std::endl;
	//std::cout << end << std::endl;
	std::vector<rca::Path> paths;
	for (int i=0; i < (int)m_genotype.size (); i++ ) {
		
		//std::cout << m_genotype[i] << std::endl;
		bool contains = 0;
		for (int l = 0; l < list_size; l++) {
			
			//pegando links usados
			rca::Link link = m_cg.getUsedLinks ()[l];
			
			//para todo individual 
			auto path = m_genotype[i].rbegin ();
			for ( ; path != m_genotype[i].rend() -1; path++) {
				
				int v = *path;
				int w = *(path +1);
				rca::Link link_t (v,w,0);
				
				//se link existe, entao
				if (link_t == link) {
					
					contains = 1;
					break;
				}
			}
			
			if (contains) {
				break;
			}			
		}
		
		rca::Path path;
		if (contains) {
 			path = shortest_path (*(m_genotype[i].rbegin()),
 											*(m_genotype[i].rend()-1),net);
			
			//std::cout <<"*"<<path << std::endl;
		} else {
			//std::cout << m_genotype[i] << std::endl;
			path = m_genotype[i];
		}
		
		if (path.size () == 0) {
			path = m_genotype[i];
		}
		
		paths.push_back (path);
		
		if (i < end) {
			
			//addin path to SteinerTree
			auto it = path.rbegin ();
			for ( ; it != path.rend ()-1; it++) {
				int v = *it;
				int w = *(it+1);
				int cost = net->getCost (v,w);
				stObserver.addEdge (v,w,cost,k);
			}
			
			
		} else {
			
			k++;
			st = SteinerTree(net->getNumberNodes (), 
					group[k].getSource(),
					group[k].getMembers ());
			stObserver.setTree (&st);
			auto it = path.rbegin ();
			for ( ; it != path.rend ()-1; it++) {
				int v = *it;
				int w = *(it+1);
				int cost = net->getCost (v,w);
				stObserver.addEdge (v,w,cost,k);
			}
			
			end += group[k].getSize ();
		
		}
		//adding path or constructor
		
	}
	
	net->clearRemovedEdges ();
	
	if (cg.congestion () > this->m_residual_capacity) {
	
		this->m_genotype = paths;
		this->m_cg = cg;
		this->m_residual_capacity = cg.congestion ();
		this->m_cost = stObserver.getCost ();
		
	}
	
	
	
}

void test (int argc, char**argv) {
	
	srand (time(NULL));
	
	rca::Network * m_network = new Network;
	std::string instance = argv[1];
	Reader r (instance);
	r.configNetwork ( m_network );
	std::vector<shared_ptr<rca::Group>> g = r.readerGroup ();
	std::vector<rca::Group> m_groups;
	for (unsigned int i =0; i < g.size (); i++) {
		m_groups.push_back (*g[i].get ());
	}
	
	/*Lista de caminhos used in init_rand_solution3*/
	g_paths = k_paths (m_network, m_groups, path_size);	
	/*lista de genes used in init_rand_solution3*/
	g_members_info = create_members_info (m_groups);
	
	PathRepresentation p;
	p.init_rand_solution2(m_network, m_groups);
	
	int cong = 15;
	int max = p.getResidualCap ();
	std::cout << max << std::endl;
	do{
		
		int size = p.getCongestionHandle ().getUsedLinks ().size () * 0.30;
		auto it = p.getCongestionHandle ().getUsedLinks ().begin();
		p.getCongestionHandle ().congestion ();
		for (; it != p.getCongestionHandle ().getUsedLinks ().end() && size-- >0;it++  ) {
			m_network->removeEdge(*it);
		}
	
		PathRepresentation x;
		x.init_rand_solution2(m_network,m_groups);
		
		if (max < x.getResidualCap ()) {
			max = x.getResidualCap();
			p = x;
		}
		
		m_network->clearRemovedEdges ();		
		
	} while (cong-- > 0);
	std::cout << max << std::endl;

	
	/*
	auto it = p.getCongestionHandle ().getUsedLinks ();
	p.getCongestionHandle ().congestion ();
	for (const rca::Link &l : it) {
		std::cout << l << " :" << l.getValue () << std::endl;
	}*/
	
	//std::cout << p.getResidualCap() << std::endl;
	
	exit (0);
}

int main (int argc, char**argv) 
{
	
#ifdef DEBUG1
	test (argc, argv);
#endif
	
	if (argc < 10 || strcmp(argv[1],"--help") == 0) {
		help ();
		exit (0);
	}
	
	int T = time (NULL);
	//srand (1426441393); //for bug in mutation on instance b30_14
	//srand (T);
	//std::cout << T << std::endl;
	srand (0);
	
	std::string instance = argv[1];
	//MetaHeuristic<GeneticAlgorithm> algorithm;
	
	GeneticAlgorithm algorithm;
	
	//int budget = atoi(argv[2]);
	int pop = atoi (argv[3]);
	double cross = atof (argv[5]);
	double mut = atof (argv[7]);
	int iter = atoi (argv[9]);
	int init = atoi (argv[11]);
	path_size = atoi (argv[13]);
	
	if (init == 2) {
		if (strcmp(argv[12],"--path")!=0) {
			printf ("parameters are incorrect\n");
			exit (1);
		}
	}
	
	if (strcmp(argv[12],"--list") == 0)
		PathRepresentation::USED_LIST = atof (argv[13]);
	else 
		PathRepresentation::USED_LIST = atof (argv[15]);	
	
#ifdef DEBUG
	printf ("--pop %d --cross %f --mut %f --iter %d --init %d --path %d --list %f\n",
			pop, cross, mut, iter, init, path_size, PathRepresentation::USED_LIST);
#endif
	
	algorithm.init_parameters (pop, cross, mut, iter, init);
	algorithm.run_metaheuristic (instance, 0.0);
	
}