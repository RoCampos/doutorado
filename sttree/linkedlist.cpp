#include "linkedlist.h"

LinkedList::LinkedList ()
{
	head = NULL;
	tail = NULL;
}

Node* LinkedList::addFirst (int value)
{
	Node *node = new Node(value);
	
	if (head == NULL) {
		head = node;
		tail = node;
		
	} else {
		
		head->prev = node;
		node->next = head;
		head = node;
	}
	
	return node;
}

Node * LinkedList::addLast (int value) {
	
	Node *node = new Node(value);
	
	if (head == NULL) {
		head = node;
		tail = node;
	} else {
		node->prev = tail;
		tail->next = node;
		tail = node;
	}	
	
	return node;
}

Node * LinkedList::remove (Node * node)
{
	if (node == NULL)
		return NULL;
	
	if (tail == head) {
		
		delete node;
		head = NULL;
		tail = NULL;
		node = NULL;
		
		return NULL;
		
	} else {
		
		if (node == tail) {
			
			tail = node->prev;
			tail->next = NULL;
			delete node;
			node = NULL;
			
			return NULL;
		}
		
		if (node == head) {
			
			head = head->next;
			head->prev = NULL;
			delete node;
			node = NULL;
			
			return NULL;
		}
		
		node->next->prev = node->prev;
		node->prev->next = node->next;
		
		delete node;
		node = NULL;
		
		return NULL;
	}	
}


void LinkedList::print ()
{
	Node *aux = head;
	
	for (; aux != NULL; aux = aux->next) {
		std::cout << aux->value << " --> ";
	}
	std::cout << "\n";
}
