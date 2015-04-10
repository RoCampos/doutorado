#ifndef _STTREE_VISITOR_H_
#define _STTREE_VISITOR_H_

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
 * @param int bandwidth da aresta para quando for necessário removê-las
 */
template<class Container>
void prunning (STTree & st, Container & cont, int, int);

#endif