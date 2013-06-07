#ifndef _NODE_H_
#define _NODE_H_

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