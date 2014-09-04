#ifndef _MPP_ACO_HANDLEs_
#define _MPP_ACO_HANDLEs_

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <random>
#include <cmath>

#include <boost/heap/fibonacci_heap.hpp>

#include "reader.h"
#include "network.h"
#include "reader.h"
#include "group.h"

#include "myrandom.h"

#include "edgecontainer.h"

#include "steinerReader.h"

#include "ant.h"
#include "steinertree.h"

typedef typename std::vector<std::shared_ptr<rca::Group>> MGroups;

typedef typename std::vector<std::vector<double>> PheromenMatrix;

typedef typename std::shared_ptr<SteinerTree> ptr_SteinerTree;


typedef typename 
	rca::myrandom<std::mt19937, std::uniform_real_distribution<double>, double> Random;

namespace rca {

/**
 * This classes implements the methods
 * need to ACO.
 * 
 * the methods of the metaheuristic ACO 
 * are implemented here: construction, pheromene
 * update and so on.
 * 
 * @author Romerito Campos
 * @date 29/08/2014
 */
class AcoMPP {
	
public:
	void update_pheromone ();
	void solution_construction ();
	void initialization ();
	
	void build_tree (int id, 
					 std::shared_ptr<SteinerTree> & st, 
					 EdgeContainer & ec);
	
	/**
	 * This method is used to configurate
	 * the informations of the problem.
	 * 
	 * It receives a string as parameter. This
	 * string is the instance tha must be read.
	 * 
	 * inside this method all attributes are instatiated.
	 * 
	 * @author Romerito Campos
	 * @date 29/08/2014
	 */
	void configurate (std::string file);
	
	void configurate2 (std::string file);
	
private:
	
	/**
	 * This method is used to find the id of the the ant that found
	 * the node next. 
	 * 
	 * The parameter pool is the pool of ants.
	 * 
	 * The parameter next_id is the id of the ant that found the
	 * next node.
	 * 
	 */
	int select_ant_id (const std::vector<Ant>& pool, const int & next_id);
	
	/**
	 * This method is used to join an ant into other.
	 * 
	 * The first parameter is the pool of ant the has size n.
	 * In the final of execution of this method the size of pool is (n-1).
	 * 
	 * The second parameter is the ant the is receive the informations of the
	 * another ant.
	 * 
	 * The third parameter is the ant the will be removed of the pool.
	 * 
	 * the fourth parameter is the vector of visited nodes.
	 * 
	 * @author Romerito Campos. 
	 */
	void join_ants (std::vector<Ant>& pool, const int&, const int&, std::vector<int>&);
	
	void create_ants_by_group (int g_id, 
							   std::vector<rca::Ant> & pool,
							   std::vector<int>&);
		
	void update_congestion (std::shared_ptr<SteinerTree>&,
							rca::EdgeContainer &ec, double&, double&);
	
	/**
	 * 
	 * 
	 * 
	 */
	void update_pheromone_matrix (rca::EdgeContainer & ec);
	
	/**
	 * 
	 * 
	 * 
	 */
	int next_component (int c_vertex, 
						 std::vector<rca::Link>& toRemove);
	
	
private:
	
	Random my_random;
	
	//*----------------------*
	//PROBLEM objects
	rca::Network * m_network;
	
	MGroups m_groups;
	
	//value of cost
	//double m_cost;
	
	//value of congestion
	//double m_congestion;
	
	//pheromene matrix
	PheromenMatrix m_pmatrix;
	
	std::vector<SteinerTree> m_best_solution;
	
	/*---------------------------------*/
	//AcoMPP parameters
	
	double m_phe_rate; //evaporation rate
	
	double m_alpha; //parameter alpha
	
	double m_betha; //parameter betha
	
};

}

#endif /*_MPP_ACO_HANDLEs_*/