#include <iostream>
#include <cstring>

#include "SolverDataGenerator.h"
#include "group.h"
#include "network.h"
#include "reader.h"
#include "steinerReader.h"

void steiner_tree (std::string file);

void budget_multicast (std::string file);

void budget_by_sttree (std::string file, std::string dir_output);

/*
 * Gera dados considerando que cada grupo consome 1 unidade de tráfego.
 * A capacidade das arestas é igual ao tamanho do grupo.
 */
void budget_multicast_tk (std::string file);

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
	
	std::string msg = "Enter 1 for Steiner Tree\n";
	msg += "Enter 2 for Budget Multicast\n";
	msg += "Enter 3 for Budget Multicast by tree\n";
	msg += "Enter 4 for Budget with TK=1 and EdgeCap=sizeGroup\n";
	if (argc <= 1 || strcmp (argv[1],"--help") == 0) {
		std::cout << msg;
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
		case 4 : {
			budget_multicast_tk (file);
		}
		default : {
			std::cout << msg;
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
	
	DataGenerator<MultipleMulticastCommodityFormulation> dg;
	dg.run (net, g);
}


void budget_multicast_tk (std::string file)
{
	Network * net = new Network;	
	Reader r (file);
	r.configNetwork ( net );
	
	std::vector<shared_ptr<rca::Group>> g = r.readerGroup ();
	
	r.setBand (net, g.size () );
	
	DataGenerator<MultipleMulticastCommodityFormulation> dg;
	dg.run (net, g);
}
