#include <iostream>

#include "SolverDataGenerator.h"
#include "group.h"
#include "network.h"
#include "reader.h"
#include "steinerReader.h"

void steiner_tree (std::string file);

void budget_multicast (std::string file);

void budget_by_sttree (std::string file, std::string dir_output);

/**
 * Gera o binário dataModel
 * 
 * Este binário permite usar as implementações de @SolverDataGenerator que 
 * geram arquivos no formato *.dat
 * 
 * Pode-se gerar o dat para uma árvore de steiner.
 * 
 * Pode-se gerar o dat para uma instância do MulticastPackingProblem usando
 * o modelo @see @MultipleMulticastCommodityFormulation.
 * 
 * Pode-se gerar um dat para cada grupo de uma instância do MulticastPackingProblem
 * de modo que elas possam ser otimizados individualmente como problema da árvore
 * de Steiner.
 * 
 */
int main (int argc, char**argv)
{
	
	if (argc <= 1) {
		std::cout << "Enter 1 for Steiner Tree\n";
		std::cout << "Enter 2 for Budget Multicast\n";
		exit(1);
	}
	
	std::string file = argv[1];
	int option = atoi(argv[2]);
	
	switch (option) {
		case 1 : {
			steiner_tree (file);
		}break;
		
		case 2 : {
			budget_multicast (file);
		}break;
		case 3 : {
			budget_by_sttree (file, argv[3]);
		}break;
		default : {
			std::cout << "Enter 1 for Steiner Tree\n";
			std::cout << "Enter 2 for Budget Multicast\n";
		}break;
	}


	return 0;
};

void budget_by_sttree (std::string file, std::string dir_output)
{
	Network * net = new Network;
	Reader r (file);
	r.configNetwork ( net );
	
	std::vector<shared_ptr<rca::Group>> g = r.readerGroup ();
	for (unsigned int i=0; i < g.size (); i++) {
		DataGenerator<MultiCommidityFormulation> dg;
		dg.run (net, g[i].get ());
		
		std::cout << "\n\n";
	}
	
}

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
	
	r.setBand (net, g.size ());
	
	DataGenerator<MultipleMulticastCommodityFormulation> dg;
	dg.run (net, g);
}
