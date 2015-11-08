#ifndef _EDGE_H_
#define _EDGE_H_

#include <iostream>
#include "def.h"

/**
 * @deprecated 
 *
 * Definição de Aresta de árvore
 * Representa uma aresta da árvore com 
 * um ponteiro para a próxima aresta.
 * 
 * 
 * @author Romerito Campos
 * @date 04/20/2013
 */
struct Edge {
	
	int i;
	int j;
	
	double cost;
	
	Edge (int _i, int _j, double _cost) 
	: i (_i), j(_j), cost (_cost) {
		next = NULL;
		prev = NULL;
	}
	
	Edge *next;
	Edge *prev;
};

/**
 * Lista de Aresta da Árvore
 * 
 * @author Romerito Campos
 * @date 04/20/2013
 */
struct LisfOfEdge {
	
	LisfOfEdge () : size (0){
		head = NULL;
		tail = NULL;
	}
			
	Edge * add (int i, int j, double value);
	Edge * remove (Edge *);
	Edge * first (){return head;};
	Edge * last (){return tail;};
	
	Edge * head;
	Edge * tail;
	int size;
	
	bool exist (int i, int j);
	
};

#endif