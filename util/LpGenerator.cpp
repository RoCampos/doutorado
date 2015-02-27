#include <iostream>

#include "SolverDataGenerator.h"
#include "group.h"
#include "network.h"
#include "reader.h"
#include "steinerReader.h"
/**
 * Binário que gera os lps para cada instância do MPP
 * 
 * Usa a implementação de MultipleMulticastCommodityLP
 * 
 */
int main (int argv, char**argc)
{
	std::string file = argc[1];
	
	Network * net = new Network;	
	Reader r (file);
	r.configNetwork ( net );
	
	std::vector<shared_ptr<rca::Group>> g = r.readerGroup ();
	
	DataGenerator<MultipleMulticastCommodityLP> dg;
	dg.run (net, g);

	return 0;
}