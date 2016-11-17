#ifndef _STTREE_VISITOR_H_
#define _STTREE_VISITOR_H_

#include <vector>

#include "sttree.h"
#include "steiner.h"
#include "edgecontainer.h"
#include "steiner_tree_observer.h"

namespace rca {
	
namespace sttalgo {

/**
 * Este método é um template, ele é usado
 * para fazer o prunning na árvore quando
 * for necessário passar um container de arestas.
 * 
 * Este container de arestas pode ser utilizado
 * para monitorar o uso das arestas.
 * 
 * @param SteinerType STTree ou steiner 
 * @param Container could be @EdgeContainer por exemplo
 * @param int representa o carga que será liberada para cada aresta podada
 * @param int bandwidth(max) da aresta para quando for necessário removê-las,
 * pode ser o tamanho do grupo. 
 * @author Romerito Campos
 * @date 04/12/2015
 */
template<class Container, class SteinerType, class NetworkType = rca::Network>
void prunning (SteinerType & st, Container & cont, int, int, NetworkType &);

/**
* Especialização do método prunning.
* 
* @param STTree tipo de representação de árvore 
* @param rca::EdgeContainer container de arestas
* @param int representa o carga que será liberada para cada aresta podada
* @param int bandwidth(max) da aresta para quando for necessário removê-las
* @author Romerito Campos
* @date 11/17/2015
*/
void prunning (STTree & st, 
	rca::EdgeContainer<rca::Comparator, rca::HCell> & cont, int, int, rca::Network & net);

/**
* Especialização do método prunning.
* 
* @param steiner tipo de representação de árvore 
* @param rca::EdgeContainer container de arestas
* @param int representa o carga que será liberada para cada aresta podada
* @param int bandwidth(max) da aresta para quando for necessário removê-las
* @author Romerito Campos
* @date 11/17/2015
*/
void prunning (steiner & st, 
	rca::EdgeContainer<rca::Comparator, rca::HCell> & cont, int, int, rca::Network & net);



/**
* Função template para fazer prunning individual sem alterar um container 
* de arestas.
*
* @param NetworkType um tipo de Network
* @param SteinerType um tipo de representação de árvore de steiner.
*/
template<class NetworkType, class SteinerType>
void make_prunning (NetworkType & network, SteinerType & tree);

/**
* Especialização da função make_prunning para uma representação de 
* árvore de steiner usand steiner.
*
* @param NetworkType um tipo de Network
* @param steiner um tipo de representação de árvore de steiner.
*/
void make_prunning (rca::Network & network, steiner & tree); 

/**
* Especialização da função make_prunning para uma representação de
* árvore de steiner usando STTree
*
*
* @param NetworkType um tipo de Network
* @param STTree um tipo de representação de árvore de steiner.
*/
void make_prunning (rca::Network & network, STTree & tree);



/**
 * Este método tempo por objetivo retorna uma lista
 * de caminhos que corresponde a representação de uma
 * solução por caminhos.
 * 
 * 
 * @param STTRe
 * @return std::vector<rca::Path>
 */ 
std::vector<rca::Path> stree_to_path (STTree & st, int source, int nodes);

/**
 * This method is used to separeted the nodes from a cut in the
 * steiner tree.
 * 
 * @param STTree steiner tree
 * @param int source of the tree Ti after the division
 * @param rca::Link the cur
 * @param std::vector<int> vertex marked
 * @param int nodes. The number of nodes of the graph
 */
std::vector<int> make_cut_visitor (std::vector<rca::Link> & st, 
			   int source, 
			   rca::Link &,
			   std::vector<int> &mark, 
			   int nodes);


/**
 * Este método é utilizado para remover as arestas da rede cujo o valor
 * seja menor ou igual ao valor minimo ob.top() + res. 
 * 
 * Durante as remoções, verifica-se a conectividade entre os membros do 
 * grupo multicast group.
 * 
 * @param Container edge container por exemplo
 * @param rca::Network
 * @param rca::Group
 * @author Romerito Campos.
 */
template<class Container>
void remove_top_edges (Container & ob, 
					   rca::Network & m_network, 
					   rca::Group & group, int res);

template<class Container>
void remove_top_edges (
	rca::Network & network,
	rca::Group & group, 
	Container& container) 
{

	auto it = container.get_heap ().ordered_begin ();
	auto end = container.get_heap ().ordered_end ();
	for ( ; it != end; it++) {
		network.removeEdge (*it);
 		if (is_connected (network, group) == false ) {
 			network.undoRemoveEdge (*it);
 		}
	}	
}

/**
 * This method create a new steiner tree based on AGM algorithm.
 * First, the most congested edges on network object are removed.
 * 
 * Second, all edges are sorted by its cost and removed from the network
 * if it is possible. 
 * 
 * Then, an AGM is built. After, we remove all non-terminal nodes. 
 * 
 * @param std::vector<STTree>
 * @param rca::Network
 * @param std::vector<rca::Group>
 * @param rca::EdgeContainer<rca::Comparator,rca::HCell>
 * @date 7/1/2015 
 * @author Romerito Campos.
 */
void improve_cost (std::vector<STTree>& m_trees, 
	rca::Network & network, 
	std::vector<rca::Group>& m_groups, 
	rca::EdgeContainer<rca::Comparator, rca::HCell> & cg, int);

void cost_by_usage (std::vector<rca::Link>&, 
					std::vector<STTree>&, 
					rca::Network & network);

/**
 * Este método é utilizado para imprimir uma solução do MPP
 * utilizando o formato xdot.
 * 
 * As informações são enviadas para std::cerr
 * 
 * @param std::vector<SteinerType> lista de árvores de steiner de um tipo genético
 */
template<class SteinerType>
void print_solution (std::vector<SteinerType>& trees);

/**
 * Este método imprime uma solução na saída padrão.
 * O formato da impressão é utilizado para fazer o teste
 * do valor objetivo e do valor limite de orçamento.
 * 
 * @param std::vector<SteinerType> lista de árvores de steiner de um tipo genético
 */
template<class SteinerType>
void print_solution2 (std::vector<SteinerType>& trees);

template<class SteinerType, class Container, class NetworkType>
void print_solutions_stats (std::vector<SteinerType>& trees, 
							Container &cg, NetworkType& net);


/**
 * Este método é utilizado para retonar os links disponíveis para substituir
 * um Link _link_ na árvore SteinerType considerando o valor de capacidade residual
 * 
 * @param SteinerType tipo que representa uma árvore de steiner
 * @param Container tipo que represnta o container de arestas
 * @param NetworkType tipo que representa uma árvore de steiner
 */
template<class SteinerType, class Container, class NetworkType>
std::vector<rca::Link> 
get_available_links (SteinerType &, 
					Container&, 
					NetworkType&,
					rca::Group&,
					rca::Link& );

/**
* Função utilizada para converter um STTree para um 
* lista de arestas.
*
* @param STTree árvore
* @return std::vector<rca::Link> lista de arestas
*/
std::vector<rca::Link> sttreeToVector (STTree & st);

/**
* Método utilizado para substituir uma aresta por outra.
* 
*
* @param STTree árvore a ser modificada
* @param rca::Link aresta que será substituída
* @param rca::LInk aresta nova
* @param rca::Network grafo onde a solução é construída.
*/
void replace_edge (STTree &, 
				   rca::Link &_old, 
				   rca::Link & _new, 
				   rca::Network&);

/**
*
* Método que verifica se os caminhos do no fonte para
* cada nó de demanda em cada grupo estão de acordo com
* um limit de hops dado.
*
* @param vector<steiner> conjunto de árvores da solução
* @param vector<rca::Group> conjunto de grupos multicast
* @param int limit de tamanho dos caminhos
* 
* @return true se a solução é valida ou falso caso contrário.
*/
bool check_path_limit (std::vector<steiner> & solucao, 
	std::vector<rca::Group> & mgroups, int limit);

bool check_path_limit (steiner&, rca::Group &, int limit);

}// end of namespace visitor
}// end of namespace rca

#endif