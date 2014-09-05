#include <iostream>
#include <memory>

#include "aco.h"
#include "aco.cpp"
#include "acompp.h"

using namespace rca;

int main (int argv, char **argc) {
    
	std::string str = argc[1];
	ACO<AcoMPP> aco;
	
	double alpha = 0.5;
	double betha = 0.5;
	double phe_rate = 0.9;
	double prob_heur = 0.5;
	int iterations = 100;
	
	aco.run (6, str, iterations, alpha, betha, phe_rate, prob_heur);
	
	return 0;
}