#include <iostream>
#include <vector>
#include <cassert>
#include <utility>
#include <cstring>

#include "network.h"
#include "group.h"
#include "path.h"
#include "algorithm.h"
#include "reader.h"
#include "steinertree.h"
#include "SteinerTreeObserver.h"
#include "kspath.h"

class PathRepresentation;

class GeneticAlgorithm {
	friend class PathRepresentation;

public:
	inline void init_parameters (int pop = 24, double cross = 0.5, double mut = 0.2, 
								int iter = 25, int init = 0)
	{
		m_pop = pop;
		m_cross = cross;
		m_mut = mut;
		m_iter = iter;
		m_init = init;
	}
	
	void run_metaheuristic (std::string instance, int budged);
	
private:
	void init_problem_information (std::string instance);
	
	
	void init_population ();
	
	//individual i and individual j
	void crossover (int i, int j);
	
	int selection_operator (int i, int j);
	
	//individual i
	void mutation (int i);
	
	/* method to run the algorithm against the instance*/
	void run ();
	
private:
	
	/*problem parameters*/
	std::string m_instance;
	rca::Network * m_network;
	std::vector<rca::Group> m_groups;
	int m_budget;
	
	/* Algorithm parameters*/
	int m_pop;
	double m_cross;
	double m_mut;
	int m_iter;
	int m_init;
	
	std::vector<PathRepresentation> m_population;
};


class PathRepresentation {
	friend class GeneticAlgorithm;
	
	
public:
	PathRepresentation (): m_cost(0), m_residual_capacity(0){}
	
	PathRepresentation (const PathRepresentation&);
	PathRepresentation & operator= (const PathRepresentation&);
	
	//build from gene 0 to gene n-1( sum D_k for all k)
	void init_rand_solution1 (rca::Network * net, 
							std::vector<rca::Group> & group);

	//Aletaoriamente escolhe o gene(source,member of group i)
	//compute shortest_path
	void init_rand_solution2 (rca::Network * net, 
							std::vector<rca::Group> & group);
	
	//using list of paths
	void init_rand_solution3 (rca::Network *,
							  std::vector<rca::Group>&);
	
	void operator1 (rca::Network *net, std::vector<rca::Group> & group);
	
	
	inline int getCost () {return m_cost;}
	inline int getResidualCap () {return m_residual_capacity;}
	
	void print_solution (rca::Network *net,std::vector<rca::Group> &);
	
	void setCongestionHandle (CongestionHandle &cg) {
		m_cg = cg;
	}
	CongestionHandle & getCongestionHandle () {
		return this->m_cg;
	}
	
public:
	static double USED_LIST;
	
private:
	int m_cost;
	int m_residual_capacity;
	
	std::vector<rca::Path> m_genotype;
	bool m_feasable;
	
	CongestionHandle m_cg;
	
	
	
};

//tupla contendo informações sobre id(no gene do individuo), group_id, 
//source, desination
typedef std::tuple<int,int,int,int> Tuple;

/**
 * Função que retorna um vector de tuplas que contém informapções sobre
 * cada par source/member para cada grupo.
 */
std::vector<Tuple> create_members_info (std::vector<rca::Group>& groups) {

	//contém informações sobre cada grupo
	int cont = 0;
	std::vector<Tuple> members_info;
	for (Group & g : groups) {	
		for (const int & i : g.getMembers ()) {
			
			//stores gene_id, group_id, source, member
			Tuple info (cont++, g.getId(),g.getSource (), i);

			members_info.push_back (info);
		}
	}
	
	return members_info;
}

/**
 * Inicializa e retorna um vector de genes.
 * 
 * Este vector passará pelo shuffle para aleatorizar a construção
 * de soluções usando o algoritmo init2.
 */
std::vector<int> create_gene_vector (int gene_size) {

	//generating the genes
	std::vector<int> genes = std::vector<int> (gene_size,0);
	genes[0]=0;
	for (int i=1; i < (int)genes.size (); i++) {
		genes[i]= genes[i-1]+1;
	}
	
	return genes;
	
}


typedef std::vector<rca::Path> ListPath;

/**
 * Esta função retorna todos os SIZE caminhos de todos os nós fontes
 * para todos os destinos para cada grupo multicast
 * 
 */
std::vector<std::vector<rca::Path>> k_paths (rca::Network* net, 
											 std::vector<rca::Group>& groups,
											 int size)
{
	
	std::vector<std::vector<rca::Path>> paths; //all paths
	
	for (Group & g : groups) {
		
		int source = g.getSource ();
		
		KShortestPath path_builder (net);
		for (const int & m : g.getMembers ()) {
			
			path_builder.init (source, m);
			std::vector<rca::Path> path_i; //paths group i
			
			int i = 0;
			while (path_builder.hasNext () && i++ < size) {
				
				rca::Path path = path_builder.next ();
				path_i.push_back (path);
				
			}			
			paths.push_back (path_i);
			path_builder.clear ();
		}
		
	}	

	return paths;
}

extern int path_size;
extern std::vector<ListPath> g_paths;
extern std::vector<Tuple> g_members_info;
extern int used_list;

double PathRepresentation::USED_LIST;


/*HELP FUNCTION*/

void help (std::string p = "--h") {
	
	if (strcmp (p.c_str(),"--h") != 0) {
		printf ("Use: genetic_mmmstp --help\n");
	}
	
	printf ("Correct input is..\n");
	printf ("<instace> --pop <value> --cross <value> --mut <value>");
	printf (" --iter <value> --init <value>");
	printf (" --path <value> --list <value>\n");
	
	
	printf ("\n--pop : defines the size of population\n");
	printf ("--cross : crossover rate\n");
	printf ("--mut : mutation rate\n");
	printf ("--iter : maximum iterations\n");
	printf ("--init : initialization algorithm:");
	printf ("\n\t init_rand_solution<> 1, 2 or 3\n");
	printf ("--path : if init is 3, --path must be defined. Representent");
	printf (" the size of list of paths for each source/destination\n");
	printf ("--list : used in operator1 to define the size of removed list\n");
	
}