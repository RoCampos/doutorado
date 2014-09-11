#include <iostream>
#include <memory>

#include "aco.h"
#include "aco.cpp"
#include "acompp.h"

using namespace rca;

int main (int argv, char **argc) {
    
	std::string str = argc[1];
	ACO<AcoMPP> aco;
	
	int iterations = atoi(argc[2]);
	double alpha = atof(argc[3]);
	double betha = atof(argc[4]);
	double phe_rate = atof(argc[5]);
	double prob_heur = atof(argc[6]);
	
	
	aco.run (6, str, iterations, alpha, betha, phe_rate, prob_heur);
	
	return 0;
}