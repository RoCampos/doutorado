#ifndef _NODE_H_
#define _NODE_H_

/**
 * @deprecated
 *
 * Struct Node define um nó com seu rótulo
 * e ponteiros para nós prev e next.
 *
 * @author Romerito Campos
 * @date 04/20/2013
 * 
 */
struct node {
	
	node(int _value) {
		value = _value;
		next = NULL;
		prev = NULL;
	}
	~node() {
		next = NULL;
		prev = NULL;
	}
	
	int value;
	struct node* next;
	struct node* prev;
	
};

typedef node Node;

#endif