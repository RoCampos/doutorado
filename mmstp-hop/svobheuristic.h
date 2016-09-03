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

	typedef typename std::vector<int> VectorI;

public:
	StefanHeuristic(rca::Network&, std::vector<rca::Group>&);

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

	bool findin (std::vector<int> & container, int vertex) {
		auto it = std::find (container.begin(), container.end(), vertex);
		return it != container.end ();
	}

	bool avoidloop (std::vector<int>& container, rca::Path & path) {

		for (size_t i = 0; i < path.size ()-1; ++i)
		{
			if (container[ path[i] ] >= 1) {
				return false;
			}
		}
		return true;

	}

	int get_cost (rca::Path& path ) 
	{
		int cost = 0;
		for (size_t i = 0; i < path.size()-1; ++i) {
			cost += this->m_network->getCost (path[i],path[i+1]);
		}
		return cost;
	}

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

	void update_position (VectorI& verticesT, 
		VectorI& members, 
		VectorI& position, 
		VectorI& termT,
		rca::Path& path);

	void update_position_after (
		std::vector<rca::Path> & paths,
		VectorI& verticesT,
		VectorI& members,
		VectorI& position,
		VectorI& termT,
		rca::Path& path);
	
	rca::Path get_source_path (
		std::vector<rca::Path> & paths,
		rca::Path& path);

private:

	rca::Network *m_network;
	std::vector<rca::Group> m_groups;
	size_t H;

	std::vector<steiner> m_solution; //tree constructed by the algorithm

	//0 - is vesion with no update of u_i variables		CHINS-R
	//1 - is the second version with update of u_i vars CHINS-RR
	bool m_version; //this variable indicates which version the vob heuristic

};

#endif // STEFANVOB_H



} // rca