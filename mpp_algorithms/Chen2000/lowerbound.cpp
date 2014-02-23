#include <stdio.h>
#include "lowerbound.h"
#include "network.h"

using namespace rca;

int * partitions;

void alloc_partition (int part_size) {
	
	partitions = (int*) calloc (part_size, sizeof(int));
	
}

void dealloc_partition () {
	
}

void configure_patition (const vector<Group*> & group) {
	
}

int main (void){

	alloc_partition (10);
	
	for (int i = 0; i < 10; i++) {
		printf ("%d \n", partitions[i]);
	}
	
	return 0;
}