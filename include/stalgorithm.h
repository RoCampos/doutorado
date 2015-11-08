#include <iostream>

/**
* Esta classe template é utilizada para encapsular algoritmos
* para criação de árvores de Steiner.
*
* Ela é baseada no policy design por Andrei Alexandrescu.
*
* @author Romerito C. Andrade
*
*/
template <typename Algo, typename Net, typename ST, typename Group>
class STAlgorithm : private Algo{
	
	using Algo::heuristic;
	
public:
	
	/**
	* Método build que é utilizado para executar
	* uma heuristica de árvore de Steiner
	*
	* Esta heurística é definida em tempo de compilação
	* Bastando utilizar uma classe que implemente o método heuristic
	* e recebe os seguintes parâmetros: Rede, estrutura para armazenar
	* uma árvore de Steiner e um grupo multicast.
	*
	* 
	* @param Net grafo onde será construida a árvore
	* @param ST estrutura para armazenar árvore de Steiner
	* @param Group grupo multlicast
	*/
	void build (Net & net, ST & st, Group & g) {
		heuristic (net, st, g);
	}
	
};