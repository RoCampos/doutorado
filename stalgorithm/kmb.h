#include <iostream>

#include <algorithm>
#include <functional>
#include <numeric>

#include "network.h"
#include "steinertree.h"
#include "algorithm.h"
#include "path.h"

/**
* Classe que encapsula o algoritmo de construção de árvore baseado
* em Kou , Markov e Berman (1981)
*
* @author Romerito C. Andrade
* @version 0.5.0
*/
class KMB {

protected:
	
	/**
	* Método que implementa a heurística que constrói uma árvore de Steiner
	* para o @see rca::Group multicast g.
	*
	* O algoritmo recebe como parâmetro um rede, uma estrutura que armazena
	* uma árovre de steiner e um grupo multicast.
	*
	* @param rca::Network
	* @param SteinerTree
	* @param rca::Group
	*/
	void heuristic (rca::Network & net, SteinerTree & st, rca::Group & g);
	
};