#ifndef _STTREE_VISITOR_H_
#define _STTREE_VISITOR_H_

#include <vector>

#include "sttree.h"
#include "edgecontainer.h"
#include "steiner_tree_observer.h"

/**
 * Este método é um template, ele é usado
 * para fazer o prunning na árvore quando
 * for necessário passar um container de arestas.
 * 
 * Este container de arestas pode ser utilizado
 * para monitorar o uso das arestas.
 * 
 * @param STTree 
 * @param Container could be @EdgeContainer por exemplo
 * @param int representa o carga que será liberada para cada aresta podada
 * @param int bandwidth(max) da aresta para quando for necessário removê-las,
 * pode ser o tamanho do grupo. 
 * @author Romerito Campos
 * @date 04/12/2015
 */
template<class Container>
void prunning (STTree & st, Container & cont, int, int);

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

#endif