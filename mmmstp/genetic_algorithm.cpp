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
	
 	std::vector<shared_ptr<rca::Group>> g;
 	
	
	rca::reader::MultipleMulticastReader r(m_instance);
#ifdef MODEL_REAL
	r.configure_real_values (m_network, g);
#endif
	
#ifdef MODEL_UNIT
	r.configure_unit_values (m_network, g);
#endif
	
	for (unsigned int i =0; i < g.size (); i++) {
 		m_groups.push_back (*g[i].get ());
 	}
	
	/*orçamento*/
	m_budget = budget;
	
	rca::elapsed_time time_elapsed;	
	time_elapsed.started ();
	/*init population*/
	init_population ();
	
	int best = 0;
	
	int best_iter = m_iter, iter = m_iter;
	
	while ( --m_iter > 0) {
		
#ifdef DEBUG1
	printf ("Interation (%d)\n", m_iter);
#endif
	
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
		
		for (int i=0; i < (int)m_population.size ();i++) {
			double cross_rate = (double)(rand () % 100 + 1)/100.0;
			if (cross_rate < m_local_search) {
				local_search (i);
			}
		}
		
		int max = m_population[0].m_residual_capacity;
		for (int i=1; i < (int)m_population.size (); i++)
		{
			
			if (m_population[i].m_residual_capacity > max)
			{	
				if (m_population[i].m_cost < m_budget) {
					max = m_population[i].m_residual_capacity;
					best = i;
					
					iter = best_iter - m_iter;
					
				}
			} else if (m_population[i].m_residual_capacity == max){
				
				if (m_population[i].m_cost < m_population[best].m_cost){
					max = m_population[i].m_residual_capacity;
					best = i;
					
					iter = best_iter - m_iter;
				}
			}
		}

		
	}
	
	time_elapsed.finished ();
	
 	std::cout << m_population[best].m_residual_capacity << " ";
 	std::cout << m_population[best].m_cost << " ";
 	std::cout << time_elapsed.get_elapsed () << " ";
	std::cout << m_seed << " ";
	std::cout << iter << std::endl;
//   	m_population[best].print_solution (m_network, m_groups);
		
	//deallocatin of resources;
// #ifdef DEBUG1
	std::cout << "\n";
	for (PathRepresentation & p : m_population) {
		std::cout << p.m_cost << " ";
		std::cout << p.m_residual_capacity << std::endl;
	}
// #endif
	
	
	delete m_network;
}



int GeneticAlgorithm::selection_operator (int i, int j)
{
	
	//retorna melhor individual com base na capacidade residual
	
	if (m_population[i].m_residual_capacity == 
		m_population[j].m_residual_capacity)
	{
		return m_population[i].m_cost < 
			m_population[j].m_cost ? i : j;
	}
	
	return (m_population[i].m_residual_capacity > 
			m_population[j].m_residual_capacity ?
		   i : j);
}

void GeneticAlgorithm::init_population ()
{
#ifdef DEBUG1
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
// 		else if (m_init == 3)
// 			m_population[i].init_rand_solution3 (m_network, m_groups);
			
		
#ifdef DEBUG1
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
				
				//m_budget = m_population[i].m_cost;
			} else if (max == m_population[i].getResidualCap ()) {
			
				if (m_population[i].getCost () < cost) {
					cost = m_population[i].getCost ();
#ifdef DEBUG1
					best = i;
#endif
				//	m_budget = m_population[i].m_cost;
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
#ifdef DEBUG1
	printf (" ------------ %s -------------\n", __FUNCTION__);
#endif
	
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
	int NODES = m_network->getNumberNodes ();
	int stop = 0; //delimitador de árvore no genótipo
	
	//type defind in genetic header
	CongestionHandle cg;
	cg.init_congestion_matrix (m_network->getNumberNodes ());
	cg.init_handle_matrix (m_network->getNumberNodes ());
	rca::sttalgo::SteinerTreeObserver<CongestionHandle, STTree> stObserver;
	stObserver.set_container(cg);
	
	TreeAsLinks tree_as_links;
	for (int k = 0; k < GROUPS; k++) {
			
		int source = m_groups[k].getSource ();
		//SteinerTree st (m_network->getNumberNodes (), source, m_groups[k].getMembers());
		STTree st(NODES, source, m_groups[k].getMembers());
		
		//stObserver.setTree(&st);
		stObserver.set_steiner_tree (st, NODES);
		
		int g_size = m_groups[k].getSize ();
		int gen = stop; //delimitador de árvores no genótipo
		int init_set_path = gen; //usado no método setPath
		stop += g_size;
		//std::cout << gen <<" " << stop << std::endl;
		for (; gen < stop; gen++) {
		
			rca::Path path = sol.m_genotype[gen];
			
			std::vector<int>::reverse_iterator it = path.rbegin ();
			for (; it!= path.rend () -1; it++) {
				
				int v = *it;
				int x = *(it+1);
				
				int cost = m_network->getCost (v,x);
				int band_usage = m_network->getBand (v,x);
				stObserver.add_edge (v, x, cost, band_usage);
			}
		}
		
		//passing the rest value(1), and the max link band
		stObserver.prune (1, m_groups[k].getSize() );
		
		sol.setPath (init_set_path, 
					 stObserver.get_steiner_tree (), 
					 m_groups[k], NODES);
		
		tree_as_links.push_back (stObserver.getTreeAsLinks());
		
		//updating tree cost
		sol.m_cost += stObserver.get_steiner_tree ().get_cost ();
	}
	
	//updating congestion
	sol.m_residual_capacity = cg.top ();
	sol.setTreeAsLinks (tree_as_links);
	
	sol.m_cg = cg;
	
	//sol.print_solution (m_network, m_groups);
	
	int old = -1;
#ifdef DEBUG1
	int impro = -1;
#endif
	if (m_population[i].m_residual_capacity < m_population[j].m_residual_capacity)
	{
		old = i;
#ifdef DEBUG1
		impro = 1;
#endif
		
	} 
	else if (m_population[i].m_residual_capacity > m_population[j].m_residual_capacity)
	{
		old = j;
#ifdef DEBUG1
		impro = 1;
#endif
	} else {
		//verifica se melhorou o custo		
		int aux = -1;
		if (m_population[i].m_cost < m_population[j].m_cost) {
			aux = j;
		} else if (m_population[i].m_cost > m_population[j].m_cost) {
			aux = i;
		}
		
		if (aux != -1) {
			old = aux;
#ifdef DEBUG1
			impro = 2;
#endif
		}
	}
	
	if (old != -1) {
#ifdef DEBUG1
	//std::cout << "Crossver Improvement :\n";
	printf ("Crossover Improvement: (%s)\n", 
			(impro == 2 ? "Cost" : "Residual Capacity"));
	printf ("\tOld (%d) - New (%d)", m_population[old].m_residual_capacity, 
			sol.m_residual_capacity);
	printf ("\tOld (%d) - New (%d)\n", m_population[old].m_cost, 
			sol.m_cost);
#endif
		m_population[old] = sol;
		
	}
	
}

/**
 * Local Search method using visitor replacement
 * 
 * 
 */
void GeneticAlgorithm::local_search (int i)
{
	std::vector<STTree> trees, t;
	int NODES = m_network->getNumberNodes ();
	int BAND = m_groups.size ();
	
// 	printf ("Digite um Valor entre %d e %d: ", 0, m_pop-1);
// 	scanf("%d", &i);
	
	CongestionHandle ec;
	ec.init_congestion_matrix (NODES);
	ec.init_handle_matrix (NODES);
	rca::sttalgo::SteinerTreeObserver<CongestionHandle, STTree> stOb;
	stOb.set_container (ec);
	
	//getting the trees of individual is
	TreeAsLinks & tree_as_links = m_population[i].m_tree_links;
	int g = 0;
	//loop over all trees
	for (auto & tree : tree_as_links) {
		
		//creating tree structure: STTREE
		trees.push_back( STTree(NODES, 
						 m_groups[g].getSource (), 
					     m_groups[g].getMembers()));
		
		stOb.set_steiner_tree (trees[g], NODES);
		
		//loop over all links of individual tree
		for (rca::Link & link : tree) {
			//std::cout << link << std::endl;
// 			trees[g].add_edge (link.getX(), 
// 							   link.getY(), 
// 							   m_network->getCost (link.getX(), link.getY()) );
			stOb.add_edge (link.getX(),
						   link.getY(),
						   (int)m_network->getCost (link.getX(), link.getY()),
						   (int)m_network->getBand (link.getX(), link.getY()));
			
		}	
		
		//prunning<CongestionHandle>(trees[g], cg, 1, BAND);
		//prunning<CongestionHandle>(stOb.get_steiner_tree(), ec, 1, BAND);
		stOb.prune (1, BAND);
		
		g++;
	}
	
// 	CongestionHandle & ec = m_population[i].m_cg;
	
	//starting local search
	
	rca::sttalgo::ChenReplaceVisitor<> c(&trees);
	c.setNetwork (m_network);
	c.setMulticastGroups (m_groups);
	c.setEdgeContainer (ec);
	
	int cost = 0;

// local search to improve residual capacity
// 	double congestion = 0;	
//  bool improve = true;
// 	int tmp_cong = ec.top();
// 	while (improve) {
// 		this->accept (&c);
// 				
// 		int temp_cost = 0;
// 		for (auto st : trees) {
// 			temp_cost += (int)st.getCost ();
// 		}
// 		
// 		if (ec.top () > tmp_cong) {
// 			congestion = ec.top ();
// 			
// 			tmp_cong = congestion;
// 			cost = temp_cost;
// #ifdef DEBUG1
// 	printf ("Improvoment Local Search\n");
// #endif
// 			
// 		} else {
// 			cost = temp_cost;
// 			improve = false;
// 		}
// 	}
	
	int tt = 0.0;
	int imp = 0;
 	for (auto st : trees) {
 		tt += (int)st.get_cost ();
 	}	
	do {
		imp = tt;
		tt = 0.0;
		c.visitByCost ();
		tt = c.get_solution_cost ();
		
	} while (tt < imp);

 	cost = tt;
	
	rca::sttalgo::cycle_local_search<CongestionHandle> cls;
	cls.local_search (trees, *m_network, m_groups, ec, cost);

#ifdef DEBUG1
 	std::cout << congestion << std::endl;
 	std::cout << cost << std::endl;
#endif
	
	rca::sttalgo::SteinerTreeObserver<CongestionHandle, STTree> _stOb;
	tree_as_links.clear ();
	
	//varible passed to setPath delimitador de árvore no genótipo
	int pos_path = 0;
	
	g = 0; //control the access to a group
	for (auto & st: trees) {
		//configuring stObserver
		_stOb.set_steiner_tree (st, NODES);
		
		//getting the links as tree
		tree_as_links.push_back ( _stOb.getTreeAsLinks() );
		
		//setting the paths
		m_population[i].setPath (pos_path,st, m_groups[g], NODES);
		pos_path += m_groups[g].getSize ();
		g++;
	}
	
	m_population[i].m_residual_capacity = ec.top ();
	m_population[i].m_cost = cost;
	m_population[i].setTreeAsLinks(tree_as_links);
	m_population[i].m_cg = ec;
	
	//m_population[i].print_solution (m_network, m_groups);
	//getchar ();
}


/*Copy constructor*/
PathRepresentation::PathRepresentation (const PathRepresentation& ind)
{
	m_cost = ind.m_cost;
	m_residual_capacity = ind.m_residual_capacity;	
	m_feasable = ind.m_feasable;
	
	//using move from algorithm
	m_genotype = std::move (ind.m_genotype);
	
	//coping the trees
	m_tree_links = ind.m_tree_links;
	
	//coping the congestionHandle
	m_cg = ind.m_cg;
}

/*Assignement operator*/
PathRepresentation& PathRepresentation::operator= (const PathRepresentation& ind)
{
	m_cost = ind.m_cost;
	m_residual_capacity = ind.m_residual_capacity;	
	m_feasable = ind.m_feasable;
	
	//using move from algorithm
	m_genotype = std::move (ind.m_genotype);

	//coping the trees
	m_tree_links = ind.m_tree_links;
	
	m_cg = ind.m_cg;
	
	return *this;
}

void PathRepresentation::setPath (int init_pos, 
								  STTree & st, 
								  rca::Group & g, int nodes)
{
#ifdef DEBUG1
	printf (" ------------ %s ------------ \n", __FUNCTION__);
#endif

	
	std::vector<rca::Path> paths;
	paths = rca::sttalgo::stree_to_path (st, g.getSource (), nodes);
	
	int ind_pos = 0;
	for (auto member : g.getMembers() ) {
		
		int path_pos = -1;
		
		for (size_t j =0; j < paths.size (); j++) {
			if (paths[j][0] == member) {
				path_pos = j;
				break;
			}
		}	

		this->m_genotype[init_pos + ind_pos] = paths[path_pos];
		
		ind_pos++;
	}

}

void PathRepresentation::init_rand_solution1 (rca::Network * net, 
									  std::vector<rca::Group>& groups)
{
	
	/*the overall cost of solution*/
	m_cost = 0;
	
	std::vector<rca::Link> used_links;
	
	int GROUPS = groups.size ();
	int NODES = net->getNumberNodes ();
	
	//CongestionHandle
	CongestionHandle cg;//(net, groups);
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);
	
	//the path holds the congestion Object
	this->setCongestionHandle(cg);
	
	//SteinerTreeObserver object, publish new changes
	rca::sttalgo::SteinerTreeObserver<CongestionHandle, STTree> stObserver;					
	stObserver.set_container ( this->getCongestionHandle () );
	
	//variable control the position used in setPath
// 	int pos_path = 0;
	
	//store the links of the tree
	//TODO maybe can be removed, since genotype stores correct path
	TreeAsLinks tree_as_links;
	
	std::vector<int> index = create_gene_vector (GROUPS);
	std::random_shuffle (index.begin(), index.end());
	
	std::vector<int> init_pos;
	init_pos.resize (GROUPS);
	init_pos [0] = 0;
	int size_gen = groups[0].getMembers ().size ();
	for (int i=1; i < GROUPS; i++) {
		size_gen += groups[i].getMembers ().size ();
		init_pos[i] = init_pos[i-1] + groups[i-1].getMembers ().size();
	}
	
	m_genotype.resize (size_gen);
	tree_as_links.resize (GROUPS);
	
	for (int j=0; j < GROUPS; j++) {
	
		int i = index[j];
	
		//getting the source
		int source = groups[i].getSource ();
		
		/*Steiner tree to compute correctly cost*/
		STTree st (NODES, source, 
					groups[i].getMembers());
		//set the cost of steiner tree
		st.set_cost (0.0);
		
		//configuring observer
		stObserver.set_steiner_tree (st, NODES);
		
		//getting the first path		
		int w = groups[i].getMember (0);
		
		rca::sttalgo::remove_top_edges<CongestionHandle> 
						( stObserver.get_container (), *net, groups[i], 1 );
		
		std::vector<int> previous_t;
		previous_t = all_shortest_path (source, w, *net);
		
#ifdef DEBUG1
		std::cout << "TREE :" << i << std::endl;
		printf ("\tpath %d to %d ", source, w);
		std::cout << path << std::endl;
#endif		
		//loop over members
// 		for (int d=0; d < groups[i].getSize (); d++) {
		int g=0;
		for (int d : groups[i].getMembers()) {
			
			
			//getting the path
			rca::Path path = get_shortest_path (source , d ,
									*net, previous_t);
			
			assert (path.size () > 0);
			
			std::vector<int>::reverse_iterator its = path.rbegin ();
			for (; its!= path.rend () -1; its++) {
					
					//creatin edge
				rca::Link link( (*its), *(its + 1), 0);
					
				rca::Link link2 = link;
				int cost = net->getCost (link2.getX(),link2.getY());
				int band_usage = net->getBand (link2.getX(),link2.getY());
				link2.setValue (cost);
					
				//add edge to observer(st tree)
				stObserver.add_edge (link2.getX(), 
									 link2.getY(), 
									 cost, 
									 band_usage);
					
			}
			
			
#ifdef DEBUG1
			std::cout << "\t\tremoving path ";
			std::cout << path << " size (";
			std::cout << path.size () << ")\n";
#endif
			
 			m_genotype[ init_pos[i] + g ] = path;
 			g++;
		
#ifdef DEBUG1
		printf ("\tpath %d to %d ", source, d);
		std::cout << path;
		std::cout << " size= " << path.size () <<std::endl;
#endif
			
		}
		
		//making prunning in the tree
		stObserver.prune (1,  groups[i].getSize () );
		
		this->setPath (init_pos[i], 
					   stObserver.get_steiner_tree (), 
					   groups[i], NODES);
		
		//storing the links of the tree i
		tree_as_links [i] = stObserver.getTreeAsLinks ();
		
		//updating the cost
		this->m_cost += stObserver.get_steiner_tree ().get_cost ();
		
#ifdef DEBUG1
		st.xdotFormat ();
		getchar ();
#endif
	
		net->clearRemovedEdges ();
	}
	
	index.clear ();
	
	//m_cost = stObserver.getCost ();
	this->m_residual_capacity = this->m_cg.top ();
	this->setTreeAsLinks (tree_as_links);
	
}

void PathRepresentation::init_rand_solution2 (rca::Network * net, 
									  std::vector<rca::Group>& groups)
{

#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	//getting the number of nodes
	int NODES = net->getNumberNodes ();

	//creating observer: one for each tree
	std::vector<rca::sttalgo::SteinerTreeObserver<CongestionHandle, STTree>> treesObserver;
	
	//congestino handle
	CongestionHandle cg;
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);
	
	//configuring the congestions handle
	this->setCongestionHandle (cg);
	
	//variable store the size of individual(number of paths)
	int gene_size = 0;
	
	//steiner trees
	std::vector<STTree*> sts;
	
	//creating the StObservers && getting the size of individual
	for (Group & g : groups) {
		gene_size += g.getSize ();
		
		//creating observers
		STTree *st = new STTree(net->getNumberNodes (), 
					  g.getSource(), g.getMembers());
		
		//setting observer for each tree
		rca::sttalgo::SteinerTreeObserver<CongestionHandle, STTree> ob;
		ob.set_container (this->getCongestionHandle () );
		
		//setting the trees to observer
		ob.set_steiner_tree (*st, NODES);
		
		//storing the observer in the list of observers
		treesObserver.push_back (ob);
		sts.push_back (st);
		st = NULL;
		
	}
	
	//auxiliar functins define in genetic_algorithm.h
	std::vector<int> genes = create_gene_vector (gene_size);
	
	//shuffling the order of each genes
	std::random_shuffle (genes.begin (), genes.end());
	
	//initialization of genotype
	m_genotype = std::vector<rca::Path> (gene_size);
	
	//loop over the genes.
	for (int i=0; i < (int)genes.size (); i++) {
		
		//this tuple contains the gene(correct position in m_genotype)
		//the group g, source of g, dest in g
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
		
		//try to create a capacited path 
		//parameters: v, w, network, groups
		rca::Path p = capacited_shortest_path (source, 
											   dest, 
											   net,
											   &this->getCongestionHandle(),
										       groups[ g ]);

#ifdef DEBUG1
	std::cout << p << std::endl;
#endif 
		//if p is 0 size, creat a normal path
		if (p.size () == 0) {
			//net->clearRemovedEdges ();
			p = shortest_path ( source, dest, *net);
		}
		
		//add p to m_genotype
		m_genotype[gene] = p;
		
		//removing path from network
		auto it = p.rbegin ();
		
		//adding path to sttree in observers
		for (; it != p.rend()-1; it++) {
			int x = *(it);
			int w = *(it+1);
			
			int cost = net->getCost (x,w);
			int band_usage = net->getBand (x,w);
			treesObserver[g].add_edge (x,w,cost,band_usage);
		}
		
	}
	
	int i = 0; //control the group i to be prunned
	m_cost = 0; //store the cost of solution
	int pos_path = 0;
	//making prunning and updating the solution
	for (rca::sttalgo::SteinerTreeObserver<CongestionHandle, STTree> & st : treesObserver) {
		
		//prune
		st.prune (1, groups[i].getSize() );
		
		//setting correct path in genotype
		this->setPath (pos_path, 
					   st.get_steiner_tree (), 
					   groups[i], 
						NODES);
		
		pos_path += groups[i].getSize ();
		//updating cost
		m_cost += st.get_steiner_tree().get_cost ();
		
		//getting the tree as linsk
		this->m_tree_links.push_back (st.getTreeAsLinks ());
		
		i++; //next group
	}
	
	//updating residual capacity
	m_residual_capacity = this->m_cg.top ();
	
}


void PathRepresentation::print_solution (rca::Network *net, 
										 std::vector<rca::Group> & g)
{

	int group = 1;
	for (std::vector<rca::Link> & links: m_tree_links) {
		
		for (rca::Link &l: links) {
			std::cerr << l.getX()+1 << " - " << l.getY()+1	 << ":";
			std::cerr << group << ";"<< std::endl;
		}
		group++;
	}
	
}

void PathRepresentation::operator1 (rca::Network *net, 
									std::vector<rca::Group> & group)
{
	
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;	
#endif
	
 	//int list_size = m_cg.getUsedLinks ().size () * PathRepresentation::USED_LIST;
	auto & heap = m_cg.get_heap ();
	int list_size = heap.size () * PathRepresentation::USED_LIST;

#ifdef DEBUG1
	printf ("List Size (%d): \n", list_size);
#endif
	
	auto begin = heap.ordered_begin ();
	auto end_it = heap.ordered_end ();
	int size = 0;
	for (; begin != end_it && size < list_size; begin++) {
		rca::Link l = *begin;
		net->removeEdge ( l );
		size++;
	}
	
	double cost = 0.0;

	int NODES = net->getNumberNodes ();
	CongestionHandle cg;//(net,group);
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);
	
	rca::sttalgo::SteinerTreeObserver<CongestionHandle, STTree> stObserver;//(NULL, &cg);
	stObserver.set_container (cg);
	
	int end = group[0].getSize ();
	STTree *st = new STTree (net->getNumberNodes (), 
							group[0].getSource(),
							group[0].getMembers ());
	
	std::vector<STTree*> trees;
	trees.push_back (st);
	
	stObserver.set_steiner_tree (*trees[0], NODES);
	int k = 0; //group counting
	
	TreeAsLinks tree_as_links;
	
	//std::cout << end << std::endl;
	std::vector<rca::Path> paths;
	for (int i=0; i < (int)m_genotype.size (); i++ ) {
		
		//std::cout << m_genotype[i] << std::endl;
		bool contains = 0;
		//for (int l = 0; l < list_size; l++) {
		begin = heap.ordered_begin ();
		end_it = heap.ordered_end ();
		size = 0;
		for (; begin != end_it && size < list_size; begin++) {
			
			//pegando links usados
			//rca::Link link = m_cg.getUsedLinks ()[l];
			rca::Link link = *begin;
			
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
			
			size++; //control the number of removed edges
		}
		
		rca::Path path;
		if (contains) {
 			path = shortest_path (*(m_genotype[i].rbegin()),
 											*(m_genotype[i].rend()-1),*net);
			
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
				
				int band_usage = net->getBand (v,w);
				stObserver.add_edge (v,w,cost,band_usage);

			}
			
			
		} else {
			
			//updating cost
			stObserver.prune (1, group[k].getSize() );
			cost += stObserver.get_steiner_tree ().get_cost ();
			tree_as_links.push_back( stObserver.getTreeAsLinks () );
			
			
			//creating the next tree
			k++;
			STTree *_st = new STTree(net->getNumberNodes (), 
							group[k].getSource(),
							group[k].getMembers ());
			
			trees.push_back (_st);
			
			_st->set_cost (0.0);
			stObserver.set_steiner_tree (*trees[k], NODES);
			
			//adding the path to group k
			auto it = path.rbegin ();
			for ( ; it != path.rend ()-1; it++) {
				int v = *it;
				int w = *(it+1);
				int cost = net->getCost (v,w);
				int band_usage = net->getBand (v,w);
				stObserver.add_edge (v,w,cost,band_usage);
				
			}
			
			end += group[k].getSize ();
		
		}
		//adding path or constructor
		
	}
	
	stObserver.prune (1, group[k].getSize() );
	cost += stObserver.get_steiner_tree ().get_cost ();
	tree_as_links.push_back( stObserver.getTreeAsLinks () );
	
	for (size_t i=0; i < trees.size (); i++) {
		delete trees[i];
	}
	
	net->clearRemovedEdges ();
	
	if (cg.top () > this->m_residual_capacity) {
		this->m_genotype = paths;
		this->m_cg = cg;
		this->m_residual_capacity = cg.top ();
		this->m_cost = cost;
		this->setTreeAsLinks (tree_as_links);
#ifdef DEBUG1
	printf ("Opeartor1 Improvement: Residual Capacity\n");
#endif
		
	} else if (cg.top () == this->m_residual_capacity && cost < this->m_cost) {
		this->m_genotype = paths;
		this->m_cg = cg;
		this->m_residual_capacity = cg.top ();
		this->m_cost = cost;
		this->setTreeAsLinks (tree_as_links);
#ifdef DEBUG1
	printf ("Operator1 Improvement: Cost\n");
#endif
		
	}
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
	srand (T);
	//std::cout << T << std::endl;
// 	srand (0);
	
	std::string instance = argv[1];
	//MetaHeuristic<GeneticAlgorithm> algorithm;
	
	GeneticAlgorithm algorithm;
	
	//int budget = atoi(argv[2]);
	int pop = atoi (argv[3]);
	double cross = atof (argv[5]);
	double mut = atof (argv[7]);
	int iter = atoi (argv[9]);
	int init = atoi (argv[11]);
	double local_search;
	
	if (init == 3) {
		if (strcmp(argv[12],"--path")==0) {
			path_size = atoi (argv[13]);
			printf ("parameters are incorrect\n");
			help();
			exit (1);
		}
	}
	
	if (init == 2) {
		if (strcmp(argv[12],"--list")!=0) {
			printf ("parameters are incorrect\n");
			help ();
			exit (1);
		}
	}
	
	
	int budget = 0;
	if (strcmp(argv[12],"--list") == 0) {
		PathRepresentation::USED_LIST = atof (argv[13]);
		local_search = atof (argv[15]);
		budget = atoi(argv[17]);
	} else {
		PathRepresentation::USED_LIST = atof (argv[15]);	
		local_search = atof (argv[17]);		
		budget = atoi (argv[19]);
	}
	
	if (budget == 0) {
		budget = INT_MAX;
	}
	
#ifdef DEBUG1
	printf ("--pop %d --cross %f --mut %f --iter %d --init %d --path %d --list %f --lsearc %f\n",
			pop, cross, mut, iter, init, path_size, PathRepresentation::USED_LIST, local_search);
#endif
	
	algorithm.init_parameters (pop, cross, mut, iter, init, local_search);
	algorithm.run_metaheuristic (instance, budget);
	
}