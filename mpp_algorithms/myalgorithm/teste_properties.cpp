#include <iostream>
#include <memory>

#include "aco.h"
#include "aco.cpp"
#include "acompp.h"

using namespace rca;

int main (int argv, char **argc) {
    
	std::string str = argc[1];
	ACO<AcoMPP> aco;
	aco.set_instance (str);
	aco.set_iterations ( atoi (argc[2]) );
	
	aco.run ();
	
	return 0;
}