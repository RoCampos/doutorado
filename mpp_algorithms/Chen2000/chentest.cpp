#include <iostream>

#include "chenalgorithm.h"

int main (int argc, char**argv) {
	
	Chen c(argv[1]);
	
	c.pre_processing ();
	
	return 0;
}