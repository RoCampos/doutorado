#ifndef STEFANVOB_H
#define STEFANVOB_H

#include <vector>

#include "network.h"
#include "algorithm.h"
#include "group.h"
#include "path.h"
#include "steiner.h"
#include "steiner_tree_observer.h"
#include "edgecontainer.h"

namespace rca {

/*
*	Esta classe modelo as heurísticas propostas
*	no paper de Stefan Vob(1999).
*	As heurísticas constroem árvores de Steiner
*	sujeitas a restrição de hops entre caminhos
*	de uma fonte para cada um dos terminais 
*	presentes na conjunto de terminais.
*	@author Romerito Campos.
*	@date 2016/08/26
*/
class StefanHeuristic
{

	typedef rca::EdgeContainer<rca::Comparator, rca::HCell> Container;
	typedef rca::sttalgo::SteinerTreeObserver<Container, steiner> Observer;
	typedef typename std::vector<int> VectorI;

public:
	
	StefanHeuristic(rca::Network&, std::vector<rca::Group>&);

	StefanHeuristic(rca::Network&, std::vector<rca::Group>&, bool verbose);

	~StefanHeuristic() {
		this->m_network = 0;
	}

	/**
	*	Loop principal. 
	*	Chama os métodos auxiliares e mostra o resultado
	*	na saída padrão.
	*/
	void run (size_t hoplimit);


	/**
	*	This version fixes the problema of updating in the
	*	first version of the heuristic of vob(1999)
	*
	*	
	*
	*	@param int limit of the path (in terms of links)
	*/
	void run2 (size_t hoplimit);


	/**
	*	Este método adiciona os nós a árvore em construção.
	*	Atualiza os valores de distância u_i de cada nó
	*	presente no caminho até o nó terminal.
	*
	*	@param rca::Path
	*/
	void addNodes (const rca::Path& path);

private:

	/**
	*	Este método retorna o menor caminho restrito
	* 	a número de hops entre v(nó presente na árvore)
	*	e w(nó terminal não presente na árvore)
	*
	*	@param int v, int w
	* 	@return rca::Path
	*/
	rca::Path getShortestPath (VectorI&, VectorI&, VectorI&, VectorI&);

	/**
	*	A function to check if a vertex is into a container ov vertexis.
	*	
	*	@param std::vector<int> container of vertexis
	*	@param int vertex to be checked.
	*	@return bool true if the vertex is inside the structure
	*/
	bool findin (std::vector<int> & container, int vertex) {
		auto it = std::find (container.begin(), container.end(), vertex);
		return it != container.end ();
	}

	/**
	*	This method is the implementation of the constraint of the CHIN-R's 
	*	version of the heuristic of Vob(1999).
	*
	*	The summation of entry nodes in a vertex must be 1. So, 
	*	any node must have only one entry link over it..
	*
	*	@param std::vector<int> container to control entry vertex
	*	@param rca::Path analysed path.
	*/
	bool avoidloop (std::vector<int>& container, rca::Path & path) {

		for (size_t i = 0; i < path.size ()-1; ++i)
		{
			if (container[ path[i] ] >= 1) {
				return false;
			}
		}
		return true;

	}

	/**
	*	This method is used to get the cost of a path.
	*
	*
	*	@param rca::Path 
	*	@return int cost of the path
	*/
	int get_cost (rca::Path& path ) 
	{
		int cost = 0;
		for (size_t i = 0; i < path.size()-1; ++i) {
			cost += this->m_network->getCost (path[i],path[i+1]);
		}
		return cost;
	}

	/**
	*	This function is used to update the container of vertex
	*	that controls the entry of vertexis.
	*
	*	So, for each vertex in the path, the container 'invertex' is
	*	updated.
	*
	*	@param path
	*	@param vector<int> 'invertex' is the container to store info about entry points of a vertex
	*	@param vector<int>	'termT' is used to store terminal vertex found in the path
	*	@param vector<int> 'members' are the terminals vertexis to be connected to tree
	*
	*/
	void update_invertex (rca::Path& path, 
		VectorI& invertex, VectorI& termT, VectorI& members) 
	{
		for (size_t i = 0; i < path.size ()-1; ++i)
		{
			int vertex = path[i];

			if (findin (members, vertex)) {
				if (!findin (termT, vertex)) {
					termT.push_back (vertex);
				}	
			}			

			invertex[ vertex ] += 1;
		}
	}

	/**
	*	This method is used to update the position of eache vertex
	*	int the path that connect it to the tree.
	*	
	*	This version is used in CHIN-R heuristic, therefore it is static
	*	the position of each vertex is update once during the search for a tree.
	*
	*	For example, for a path 1-->2-->3, the container named 'position' will store
	*	the distance (hops) from the node to source of the tree. Considering the vertex 1
	* 	as the source, the distance of 2 and 3 are, respectively 1 and 2 (hops as distance).
	*
	*	@param VectorI 'verticesT' is a container that stores vertexis of the tree
	*	@param VectorI 'members' are the terminals vertex of the group considered
	*	@param VectorI 'position' stores the position of each vertex related to root of the tree
	*	@param VectorI 'termT' stores the terminals of a vertex already in the tree.
	*	@param rca::Path the path to be connect to tree.
	*/
	void update_position (VectorI& verticesT, 
		VectorI& members, 
		VectorI& position, 
		VectorI& termT,
		rca::Path& path);

	/**
	*	This method is used to update the position of each vertex added to the tree.
	*	It is used in the CHIN-RR version of the heuristic of Vob(1999).
	*
	*	The update of distance is dynamic in this implementation.
	*
	*	The idea is the same as 'update_position'.
	*	
	*	The main difference is the following: if a new path found has a vertex  X that has a position
	* 	already found, and in this new path the distance of the X to root is less than the current
	*	distance, then the path who connect X to tree is update do new path.
	*
	*	@param vector<rca::Path> the paths already in the tree
	*	@param VectorI 'verticesT' is a container that stores vertexis of the tree
	*	@param VectorI 'members' are the terminals vertex of the group considered
	*	@param VectorI 'position' stores the position of each vertex related to root of the tree
	*	@param VectorI 'termT' stores the terminals of a vertex already in the tree.
	*	@param rca::Path the path to be connect to tree.	
	*/
	void update_position_after (
		std::vector<rca::Path> & paths,
		VectorI& verticesT,
		VectorI& members,
		VectorI& position,
		VectorI& termT,
		rca::Path& path);
	
	/**
	*	The vertex X are added to tree connecting it to a vertex already in the tree.
	*	So this is made path shortest path. 
	*
	*	So, it is necessary to find the complete path from root to a vertex who connect X to
	* 	tree.
	*
	*	Consider that the tree as the following vertex 1, 2 and 3. A new vertex, 5, is connect to
	* 	tree by 3-->6-->5. Then it is important get the origin of vertex 3.
	*
	*	In this way, the complete path to 5 is: 1-->2-->3-->6-->5. This path is returned
	*
	*	@param vector<rca::Path> all paths of the current tree
	*	@param rca::Path new path adding a new terminal
	*	@return rca::Path returned path.
	*/
	rca::Path get_source_path (
		std::vector<rca::Path> & paths,
		rca::Path& path);

	/**
	*	This method is used to finished the tree construction.
	*	All nodes are connected to the tree and there is a complete path to each
	*	of the terminals nodes.
	*
	*	The aim of the function is to choose the paths who connect each terminal to
	* 	tree using the short path from all paths found.
	*
	*/
	void step4 (std::vector<rca::Path>& tpaths, 
		VectorI& position, 
		VectorI& members);

	void calculate_sol (
		std::vector<rca::Path>&,
		int group, 
		Observer&);

private:

	rca::Network *m_network;
	std::vector<rca::Group> m_groups;
	size_t H;
	bool m_verbose; //used to print extra informations 

	std::vector<steiner> m_solution; //tree constructed by the algorithm

	//0 - is vesion with no update of u_i variables		CHINS-R
	//1 - is the second version with update of u_i vars CHINS-RR
	bool m_version; //this variable indicates which version the vob heuristic

};

#endif // STEFANVOB_H



} // rca