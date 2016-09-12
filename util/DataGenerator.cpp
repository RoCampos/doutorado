#include <iostream>
#include <cstring>

#include "SolverDataGenerator.h"
#include "group.h"
#include "network.h"
#include "reader.h"
#include "steinerReader.h"

/**
* Função que gera um arquivo para uma árvore de Steiner.
* 
* A geração do aquivo .dat considera o problema da árvore de Steiner
* em grupo.
* 
*/
void steiner_tree (std::string file);

/*
 * Gera dados considerando que cada grupo consome 1 unidade de tráfego.
 * A capacidade das arestas é igual ao tamanho do grupo.
 * 
 * 
 */
void budget_multicast (std::string file);

/**
* Método que utiliza o gerador MultiCommidityFormulation
* 
* Este método gera árvores individualmente. 
* Um arquivo para cada árvore dos grupos multicast é gerado.
*/
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
	
	std::string msg = "Enter 1 for Steiner Tree\n";
	msg += "Enter 2 for Budget Multicast\n";
	msg += "Enter 3 for Budget Multicast by tree\n";
	//msg += "Enter 4 for Budget with TK=1 and EdgeCap=sizeGroup\n";
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
		default : {
			std::cout << msg;
		}break;
	}


	return 0;
};

void budget_by_sttree (std::string file, std::string dir_output)
{
	Network * net = new Network;
	rca::reader::Reader r (file);
	r.configNetwork ( net );
	
	std::vector<shared_ptr<rca::Group>> g = r.readerGroup ();
	for (unsigned int i=0; i < g.size (); i++) {
		DataGenerator<MultiCommidityFormulation> dg;
		dg.run (net, g[i].get ());
		
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
	rca::reader::MultipleMulticastReader r (file);
	std::vector<shared_ptr<rca::Group>> g;
	
	/*Lê os valores confome definidos nas instâncias.*/
#ifdef MODEL_REAL	
	r.configure_real_values (net, g);	
#endif
	
	/*Atribui valor 1 para traffic request (tk) e 
	 a capacidade das arestas é igual ao tamanho do grupo.*/
#ifdef MODEL_UNIT
	r.configure_unit_values (net,g);
#endif
	
	DataGenerator<MultipleMulticastCommodityFormulation> dg;
	dg.run (net, g);
}
