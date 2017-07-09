#include <iostream>
#include "spea2.h"
// #include "nsga2.h"
// #include "moead.h"

// #include "mopso.h"
#include "network.h"

using namespace std;
// using namespace evolutionary;

int main(int argc, char const *argv[])
{

	srand (time(NULL));
	std::string file = argv[1];
	std::string conf = argv[2];	
	Info::Problem::init (file, conf);	
	Spea2 spea2(100, 10, 500);
	spea2.run ();
	spea2.printArc();

	// Nsga2 nsga2 (80, 1000);
	// nsga2.run ();
	// nsga2.printPopAsPisa ();

	// MOEAD moead(25, 10, 4, 100);
	// moead.run ();
	// moead.printArc ();

	// rca::Network & net = Info::Problem::get_instance ()->network;
	// std::vector<rca::Link> links;
	// for (auto & link : net.getLinks ()) 
	// {	
	// 	links.push_back (link);
	// }

	// std::sort (links.begin(), links.end(), order_by_node());
	// std::sort (links.begin(), links.end(), order_by_node1());

	// for (auto & l : links) {
	// 	std::cout << l << std::endl;
	// }


	return 0;
}