#ifndef _KSHORTESTPATH_H_
#define _KSHORTESTPATH_H_

#include <iostream>
#include <set>
#include <map>

#include "network.h"
#include "path.h"
#include "algorithm.h"
#include "link.h"


/**
* Esta struct é utilizada para armazenar caminhos
* candidados durante a busca por novos caminhos entre
* dois pares de nós.
*
* Ela armazena o index do caminho na lista de caminho 
* e o custo do caminho.
* @author Romerito C. Andrade
*/
struct PathNode {
	
	double cost;
	int index; //index of the path in the list all_paths
	
	bool operator< (const PathNode& path) const{
		return cost < path.cost;
	}
	
	bool operator==(const PathNode& path) const {
		return cost == path.cost && index == path.index;
	}

	
};

/**
* Esta classe encapsula o algoritmo para criar os k caminhos
* mais curtos entre dois pares de nós.
* 
* Ela é baseada no algoritmo Yen(1991) - http://goo.gl/MNGs96
*
* @author Romerito Campos
* @version 0.5.0
*/
class KShortestPath {
	
public:

	/**
	* Construtor da classe KShortestPath.
	* Recebe como parâmetro um ponteiro para um objeto 
	* do tipo @see rca::Network.
	*
	* @param rca::Network ponteiro para um objeto do tipo rca::Network
	*/
	KShortestPath (rca::Network *net);

	/**
	* Destrutor da classe.
	*	
	*/
	~KShortestPath () { }
	
	/**
	* Método que inicia a construção dos k-menores caminhos
	* 
	* O método recebe como parâmetro o nó fonte e o nó alvo da busca.
	*
	* @param int nó fonte v
	* @param int nó fonte w
	*/
	void init (int v, int w);//make the first shortest path

	/**
	* Este método é utilizado como um iterator. 
	* Ele retorna o próximo caminho mais curto caso ele exista.
	*
	* @return rca::Path próximo caminho mais curto
	*/
	rca::Path next (); //return the shortest path, create new candidates

	/**
	* Método que verifica se há um próximo caminho mais curto.
	*
	* @return bool 
	*/
	bool hasNext ();

	/**
	* Método utilizado para limpar todas as estrutura utilizadas
	* para armazenar os caminhos mais curtos.
	* 
	* Este método é útil para reutilizar um objeto do tipo KShortestPath
	* para criar os k-menores caminhos entre outros pares de vérices.
	*/
	inline void clear () {
		all_paths.clear ();
		m_candidate_paths.clear ();
		m_derivation_paths.clear ();
		m_kpaths.clear ();
	}
	
private:
	
	rca::Network * network;
	
	std::vector<rca::Path> all_paths;
	
	//PathNode storage index to path and cost
	std::set<PathNode> m_candidate_paths;
	
	//map <index to path, derivation vertex>
	std::map<int,int> m_derivation_paths;
	
	//ordered output paths
	std::vector<int> m_kpaths;
};

#endif
