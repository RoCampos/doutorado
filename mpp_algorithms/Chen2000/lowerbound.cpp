#include <stdio.h>
#include "lowerbound.h"

int main (void){

	Partition p;
	Partition q;
	
	q.add_node (1);
	
	p.join(q);
	
	
	return 0;
}