#include <iostream>
#include <memory>

#include "aco.h"
#include "aco.cpp"
#include "acompp.h"

using namespace rca;

int main (int argv, char **argc) {
    
	std::string str = argc[1];
	AcoMPP acompp;
	acompp.configurate (str);
	acompp.initialization ();
	
	return 0;
}