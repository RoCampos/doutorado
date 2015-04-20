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

#endif