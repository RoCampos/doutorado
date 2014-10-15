#include "edge.h"

Edge * LisfOfEdge::add (int i, int j, double value) {
	
	Edge * edge = new Edge (i,j,value);
	edge->next = NULL;
	edge->prev = NULL;
	
	if (head == NULL) {
		head = edge;
		tail = edge;
	} else {
		
		tail->next = edge;
		edge->prev = tail;
		tail = edge;
	}
	
	return edge;
}

Edge * LisfOfEdge::remove (Edge * edge) {
	
	if (edge == NULL)
		return NULL;
	
	if (tail == head) {
		
		delete edge;
		head = NULL;
		tail = NULL;
		edge = NULL;
		
		return NULL;
		
	} else {
		
		if (edge == tail) {
			
			tail = edge->prev;
			tail->next = NULL;
			delete edge;
			edge = NULL;
			
			return NULL;
		}
		
		if (edge == head) {
			
			head = head->next;
			head->prev = NULL;
			delete edge;
			edge = NULL;
			
			return NULL;
		}
		
		edge->next->prev = edge->prev;
		edge->prev->next = edge->next;
		
		delete edge;
		edge = NULL;
		
		return NULL;
	}	
}

bool LisfOfEdge::exist (int i, int j) {
 
  Edge * aux = head;
  
  while (aux != NULL) {
   
    if ((aux->i == i && aux->j == j) || (aux->i == j && aux->j == i)) {
		
#ifdef DEBUG
      std::cout << "equals" << std::endl;
#endif
      return 1;
    }
    
    aux = aux->next;
    
  }
  return 0;
  
}