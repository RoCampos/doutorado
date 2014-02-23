#include <stdio.h>
#include "lowerbound.h"
#include "network.h"

using namespace rca;

int main (void) {

	Partition * p = partition_alloc ();
	init_partition (p, 0);
	printf ("%d",p->id);
	
	free_partition (p);
	
	return 0;
}