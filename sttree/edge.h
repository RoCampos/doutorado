#ifndef _EDGE_H_
#define _EDGE_H_

#include <iostream>
#include "def.h"

/**
 * Definição de Aresta de árvore
 * 
 * 
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
 * 
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
	
};

#endif