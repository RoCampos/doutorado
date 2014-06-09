#include <iostream>

#include "chenalgorithm.h"

int main (int argc, char**argv) {
	
	Chen c(argv[1],argv[2],0);
	
	//c.print_edges_xdot ();
	//c.print_information ();
	//c.sort_edges ();
	
	c.run ();
	
	return 0;
}