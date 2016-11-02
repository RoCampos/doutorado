#include <iostream>

#include "svobheuristic.h"
#include "reader.h"
#include "localsearch.h"
#include "steinerReader.h"

typedef rca::EdgeContainer<rca::Comparator, rca::HCell> Container;

void run_steiner_tree (std::string & file, int);

void run_multiple_tree (std::string & file, int);

int main(int argc, char const *argv[])
{

	srand (time(NULL));
	
	std::string file = argv[1]; //file to be read
	int H = atoi (argv[2]);		//limit of the path
	int opt = atoi (argv[3]);	//option to run


	if (opt == 1)
		run_steiner_tree (file, H);
	else if (opt == 2) 
		run_multiple_tree (file, H);

	return 0;
}

void run_steiner_tree (std::string & file, int H) 
{

	rca::Network network;
	Group group;
	get_steiner_info (network, group, file);	
	std::vector<rca::Group> mgroups;	
	mgroups.push_back (group);

	int NODES = network.getNumberNodes ();
	Container cg(NODES);

	rca::stphop::StefanHeuristic obj (network, mgroups);
	obj.set_container  (cg);
	obj.run2 (H);

	auto sol = obj.get_solution ();

	int acost = 0;
	for (auto&& s : sol) {
		acost += s.get_cost ();
	}

}

void run_multiple_tree (std::string & file, int H) 
{

	rca::Network network;
	std::vector<rca::Group> mgroups;
	rca::reader::get_problem_informations (file, network, mgroups);

	int NODES = network.getNumberNodes ();
	Container cg(NODES);

	rca::stphop::StefanHeuristic obj (network, mgroups, false, true);
	obj.set_container  (cg);
	obj.run2 (H);

	rca::sttalgo::PathExchange<Container, steiner> pex(H);

	std::vector<steiner> sol = obj.get_solution ();

	assert (sol.size () > 0);

	while(pex.run (sol, cg, network, mgroups) ) {}

	int acost = 0;
	for (auto&& s : sol) {
		acost += s.get_cost ();
	}

	cout << cg.top () << " " << acost << endl;

}