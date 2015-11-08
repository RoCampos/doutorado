#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "node.h"

/**
 * @deprecated
 *
 * Lista ligada para armazenar nós.
 * Está lista de por objetivo controlar de forma
 * dinâmica os nós que entram e saem da árvore.
 * 
 * @deprecated
 * @author Romerito Campos.
 * @date 04/20/2013
 * 
 */
class LinkedList {
	
public:
	LinkedList ();
	~LinkedList () {
		Node *n = first();
		while (n != NULL) {
			n = n->next;
			delete n;
		}
	}
	
	Node* addFirst (int);
	Node* addLast (int);
	Node* remove (Node*);
	
	Node* first (){return head;}
	Node* last (){return tail;}
	
	void print();

private:
	Node * head, *tail;
};

#endif


