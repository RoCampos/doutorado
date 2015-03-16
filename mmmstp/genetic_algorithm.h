#include <iostream>
#include <vector>
#include <cassert>
#include <utility>

#include "network.h"
#include "group.h"
#include "path.h"
#include "algorithm.h"
#include "reader.h"
#include "steinertree.h"
#include "SteinerTreeObserver.h"

class PathRepresentation;

class GeneticAlgorithm {
	friend class PathRepresentation;

protected:
	void run_metaheuristic (std::string instance, int budged);
	
private:
	void init_problem_information (std::string instance);
	inline void init_parameters (int pop = 25, double cross = 0.5, double mut = 0.2, 
								int iter = 50)
	{
		m_pop = pop;
		m_cross = cross;
		m_mut = mut;
		m_iter = iter;
	}
	
	void init_population ();
	
	//individual i and individual j
	void crossover (int i, int j);
	
	int selection_operator ();
	
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
	
	std::vector<PathRepresentation> m_population;
};

class PathRepresentation {
	friend class GeneticAlgorithm;
	
	
public:
	PathRepresentation (): m_cost(0), m_residual_capacity(0){}
	
	PathRepresentation (const PathRepresentation&);
	PathRepresentation & operator= (const PathRepresentation&);
	
	void init_rand_solution (rca::Network * net, 
							std::vector<rca::Group> & group);
	
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
	
private:
	int m_cost;
	int m_residual_capacity;
	
	std::vector<rca::Path> m_genotype;
	bool m_feasable;
	
	CongestionHandle m_cg;
	
	double USED_LIST = 0.10;
	
};


void compute_usage (int m_group, std::vector<rca::Link>&, 
					SteinerTree&, rca::Network*, rca::Group&);