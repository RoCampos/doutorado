#include "genetic_algorithm.h"
#include "metaheuristic.h"

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
	
	/*orçamento*/
	m_budget = budget;
	
	/*configuring parameters - default*/
	init_parameters ();
	
	/*init population*/
	init_population ();
	
	m_population[0].operator1 (m_network,m_groups);
	getchar ();
	
	while ( --m_iter > 0) {
	
		int i = rand () % m_population.size ();
		int j;
		do {
			j = rand () % m_population.size ();
		} while (i == j);
			
		crossover(i,j);
		
		i = rand () % m_population.size ();
		
		mutation (i);
		
	}
	
	int max = m_population[0].m_residual_capacity;
	int best = 0;
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
	
#ifdef DEBUG1
	std::cout << m_population[best].m_cost << " ";
	std::cout << m_population[best].m_residual_capacity << std::endl;
	m_population[best].print_solution (m_network, m_groups);
#endif
	
	for (const PathRepresentation & p : m_population) {
		std::cout << p.m_cost << " ";
		std::cout << p.m_residual_capacity << std:: endl;
	}
	
	//deallocatin of resources;
	delete m_network;
}

void GeneticAlgorithm::init_population ()
{
#ifdef DEBUG1
	std::cout << "Creating Population: size = " << m_pop << std::endl;	
#endif
	
	int best = -1;
	int max = 1000000;
	int cost = 10000000;
	m_population = std::vector<PathRepresentation> (m_pop);
	for (int i=0; i < m_pop; i++) {
		m_population[i].init_rand_solution (m_network, m_groups);
		
		if (m_population[i].getCost () < m_budget) {
		
			if (m_population[i].getResidualCap () < max) {
			
				best = i;
				max =  m_population[i].getResidualCap ();
			} else if (max == m_population[i].getResidualCap ()) {
			
				if (m_population[i].getCost () < cost) {
					cost = m_population[i].getCost ();
					best = i;
				}
			}
			
		}
		
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
	
	if (sol.m_cost > m_budget) return;
	
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
	sol.init_rand_solution (m_network, m_groups);
	
	if ( sol.m_cost < m_budget ) {
		
		if (sol.m_residual_capacity < m_population[i].m_residual_capacity) {
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


void PathRepresentation::init_rand_solution (rca::Network * net, 
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
			if (rnd < 5) {

				/* removing this path */
				std::vector<int>::reverse_iterator it = path.rbegin ();
				for (; it!= path.rend () -1; it++) {
					rca::Link link( (*it), *(it + 1), 0);
					net->removeEdge (link);
					
					links.push_back (link);
				}
#ifdef DEBUG1
				std::cout << "\t\tremoving path ";
				std::cout << path << " size (";
				std::cout << path.size () << ")\n";
#endif
			}
			
			m_genotype.push_back (path);
			
			std::vector<int>::reverse_iterator it = path.rbegin ();
			for (; it!= path.rend () -1; it++) {
				//rca::Link link( (*it), *(it + 1), 0);
				int v = *it;
				int x = *(it+1);
				
				int cost = net->getCost (v,x);
				stObserver.addEdge (v, x, cost, i);
				
			}
		
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
	
#ifdef DEBUG
	std::cout << __FUNCTION__ << std::endl;	
#endif

	int GROUP_ID = -1;
	int max = group[0].getTrequest ();
	int begin = 0;
	//getting the max trquest
	for (int i =1; i < (int)group.size(); i++) {
		if (max < group[i].getTrequest ()) {
			max = group[i].getTrequest ();
			GROUP_ID = i;
		}
	}
	//getting the right position of the group in the m_genotype
	for (int i=0; i < GROUP_ID; i++) {
		begin += group[i].getSize ();
	}
	
	std::cout << GROUP_ID << std::endl;
	std::cout << begin << std::endl;
	
	/* remove the most usage links */
	int end = (begin+group[GROUP_ID].getSize()); 
	std::cout << end << std::endl;
	
	std::cout << this->m_cg.getUsedLinks ().size () << std::endl;
	
	for (const rca::Link & l : m_cg.getUsedLinks()) {
		std::cout << l << ":" << l.getValue () << std::endl;
	}
	
	/*
	for (int i=begin; i < end; i++ ) {
		
		rca::Path p = m_genotype[i];
		std::cout << p << std::endl;
		
		auto rit = p.rbegin ();
		for (; it != )
		
	}*/
	
	
}

int main (int argc, char**argv) 
{
	//srand (time(NULL));
	srand (0);
	
	std::string instance = argv[1];
	MetaHeuristic<GeneticAlgorithm> algorithm;
	
	
	int budget = atoi(argv[2]);
	algorithm.run (instance, budget);
	
}