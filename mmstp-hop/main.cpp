#include <iostream>

#include "svobheuristic.h"
#include "reader.h"

int main(int argc, char const *argv[])
{
	
	srand (time(NULL));
	
	std::string file = argv[1];
	int H = atoi (argv[2]);

	rca::Network network;
	std::vector<rca::Group> mgroups;
	get_problem_informations (file, network, mgroups);

	rca::StefanHeuristic obj (network, mgroups);

	obj.run (H);

	return 0;
}