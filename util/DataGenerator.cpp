#include <iostream>

#include "SolverDataGenerator.h"
#include "group.h"
#include "network.h"
#include "reader.h"
#include "steinerReader.h"

void steiner_tree (std::string file);

void budget_multicast (std::string file);

int main (int argc, char**argv)
{
	
	std::string file = argv[1];
	int option = atoi(argv[2]);
	
	switch (option) {
		case 1 : {
			steiner_tree (file);
		}break;
		
		case 2 : {
			budget_multicast (file);
		}break;
	}


	return 0;
};

void steiner_tree (std::string file)
{
	Network * net = new Network;	
	rca::SteinerReader r (file);
	r.configNetwork ( net );
	
	rca::Group g = r.terminalsAsGroup ();
	
	DataGenerator<MultiCommidityFormulation> dg;
	dg.run (net, &g);
}

void budget_multicast (std::string file)
{
	
	Network * net = new Network;	
	Reader r (file);
	r.configNetwork ( net );
	
	std::vector<shared_ptr<rca::Group>> g = r.readerGroup ();
	
	DataGenerator<MultipleMulticastCommodityFormulation> dg;
	dg.run (net, g);
}
