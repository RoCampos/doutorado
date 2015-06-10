#ifndef _STTREE_VISITOR_H_
#define _STTREE_VISITOR_H_

#include <vector>

#include "sttree.h"
#include "edgecontainer.h"

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


template<class Container>
void remove_top_edges (Container & ob, 
					   rca::Network & m_network, 
					   rca::Group & group, int res);

#endif