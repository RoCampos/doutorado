#include <stdio.h>
#include <stdlib.h>
#include <set>

using namespace std;

typedef struct _partition_ {
	
	int id;
	set<int> groups;
	set<int> nodes;
	
} Partition;

void join_partition (Partition * a, Partition *b){ }

/**
 * Allocation of a partition
 */
Partition * partition_alloc() {
	return (Partition *) malloc (sizeof (Partition));
}

/**
 * Deallocation of a partition 
 * 
 */
void free_partition (Partition *p) {
	free (p);
}

void init_partition (Partition * p, int id) {	
	p->id = id;	
}