#include <iostream>

#include "chenalgorithm.h"

int main (int argc, char**argv) {
	
	//Chen c(argv[1],argv[2],0);
	
	std::string instace = "../MPP_instances/n30/b30_2.brite";
	std::string preproc = "../MPP_instances/preprocessing/n30/b30_2.pre";
	Chen c(instace, preproc, 2);
	
	//c.print_edges_xdot ();
	//c.print_information ();
	//c.sort_edges ();
	
	c.run ();
	
	return 0;
}