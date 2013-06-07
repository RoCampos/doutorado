#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "node.h"

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


