#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "group.h"

using namespace std;
using namespace rca;

extern int * patitions;

void alloc_partition (int part_size);
void dealloc_partition ();

void configure_patition (const vector<Group*> & group);
