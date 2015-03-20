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

#include <cstdarg>

#include <boost/heap/fibonacci_heap.hpp>

#include "reader.h"
#include "network.h"
#include "reader.h"
#include "group.h"

#include "myrandom.h"
#include "rcatime.h"

#include "edgecontainer.h"

#include "steinerReader.h"

#include "ant.h"
#include "steinertree.h"

typedef typename std::vector<std::shared_ptr<rca::Group>> MGroups;

typedef typename std::vector<std::vector<double>> PheromenMatrix;

typedef typename std::shared_ptr<SteinerTree> ptr_SteinerTree;


typedef typename 
	rca::myrandom<std::mt19937, std::uniform_int_distribution<int>, int> Random;

typedef typename 
	rca::myrandom<std::mt19937, std::uniform_int_distribution<int>, int> AntRandom;
	
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
	
	/**
	 * This method is the initial point of the 
	 * execution.
	 * 
	 * It receives a va_list variable define in cstdarg.
	 * The arguments are extracted from the va_list and
	 * used to initialize the parameters of the algorithm.
	 * 
	 * @param va_list
	 * @author Romerito Campos.
	 */
	void run (va_list & arglist);
	
	/**
	 * This method is used to create a steiner tree multiple
	 * ants. 
	 * 
	 * Each ant start its search from a terminal node (and source).
	 * There is a loop where the stop condition is the number of
	 * ants in the current graph.
	 * 
	 * During the loop an ant is randomly choosed and perform its 
	 * moviment.
	 * 
	 * The central idea is join all ants forming a tree. After this
	 * the prunning method from SteinerTree class is perfomed to
	 * remove unnecessary edges.
	 * 
	 * @param int id of the group
	 * @param SteinerTree a reference to steinertree
	 * @param EdgeContainer the container of used edges
	 */
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
	
	/**
	 * This method is used only for test. It inialize
	 * the struct to support the algorithm solve a Steiner Tree
	 * problem.
	 * 
	 * The parameter file is a stp file
	 * 
	 * @param string file in stp format
	 * @author Romerito Campos.
	 */
	void configurate2 (std::string file);
	
	/**
	 * This method is used to print some informations
	 * About the results obtained by the algoritm
	 * 
	 * @author Romerito Campops.
	 */
	void print_results ();
	
private:
	
	/**
	 * This method is used to find the id of the ant that found
	 * the node next. 
	 * 
	 * The parameter pool is the pool of ants.
	 * 
	 * The parameter next_id is the id of the ant that found the
	 * next node.
	 * 
	 * With the returned int the algorithm can join two partitions
	 * formed by two ants.
	 * 
	 * @param std::vector<Ant> the ants
	 * @param int the id associated with the next next_component
	 * @return the ant that found the component next
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
	
	
	/**
	 * This method is used just for initialize the pool
	 * of ants.
	 * 
	 * @param int group id
	 * @param vector<Ant> the pool of ants
	 * @param vector<int> the vector to control cicles in the tree building
	 */
	void create_ants_by_group (int g_id, 
							   std::vector<rca::Ant> & pool,
							   std::vector<int>&);
		
	/**
	 * This method is used to update the congestion on the network.
	 * 
	 * The method receive four parameters.
	 *  
	 * The first parameter is the tree that will use the resources.
	 * 
	 * The second parameter is the container of used edges. This 
	 * container holds the information updated.
	 * 
	 * The third parameter is the cost of the current solution, it is
	 * create outside the method. After each tree is builded this value
	 * is update. So it is passed as a reference.
	 * 
	 * The four value is, in the same way of the cost, passed as reference.
	 * 
	 */
	void update_congestion (std::shared_ptr<SteinerTree>&,
							rca::EdgeContainer &ec, double&, double&);
	
	/**
	 * 
	 * This method is used to update the pheromene matrix.
	 * 
	 * It receives a reference to a EdgeContainer object.
	 * 
	 * This refence contains all the edges used to build a solution.
	 * 
	 */
	void update_pheromone_matrix (rca::EdgeContainer & ec);
	
	/**
	 * This method is used to find a next component based
	 * on the heuristic informations or the ant probabilities.
	 * 
	 * It receive a int that is the vertex where the ant is.
	 * 
	 * Beside, a vector of ineligible edges.
	 * 
	 * The return is a int that represent the next vertex on the 
	 * graph.
	 * @param int c_vertex
	 * @param std::vector<Link> ineligible edges
	 * @return the next step on the search.
	 */
	int next_component (int c_vertex, 
						 std::vector<rca::Link>& toRemove);
	
	/**
	 * This method is used to do local updates on the pheromene
	 * matrix.
	 * 
	 * After build a steiner tree it's possible verify if this tree
	 * is the best one tree found for the current group.
	 * 
	 * So if the tree is the best one found os far, than the local
	 * update of the PheromenMatrix is release.
	 * 
	 * @param SteinerTree
	 */
	void local_update (SteinerTree * st);
	
private:
	
	//random number generator
	Random my_random;
	
	long m_seed;
	
	//*----------------------*
	//PROBLEM objects
	rca::Network * m_network;
	
	//vector of multicast groups
	MGroups m_groups;
	
	//pheromene matrix
	PheromenMatrix m_pmatrix;
	
	std::vector<SteinerTree> m_best_solution;
	
	/*---------------------------------*/
	//AcoMPP parameters	
	double m_phe_rate; //evaporation rate
	
	double m_alpha; //parameter alpha
	
	double m_betha; //parameter betha
	
	double m_heuristic_prob;
	
	double m_local_upd;
	
	/*solution results*/
	std::vector<double> m_best_trees;
	
	double m_bcost; //the best cost value
	double m_bcongestion; //the best congestion value
	double m_best_iter; //the best iteration
	
	rca::time::elapsed_time time_elapsed;
	
};

}

#endif /*_MPP_ACO_HANDLEs_*/