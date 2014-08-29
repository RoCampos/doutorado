#include <iostream>
#include <memory>

#include "aco.h"
#include "aco.cpp"
#include "acompp.h"

using namespace rca;

int main (int argv, char **argc) {
    
	std::string str = argc[1];
	
	ACO<AcoMPP> algorithm;
	algorithm.set_instance (str);
	algorithm.run ();
	
	return 0;
}