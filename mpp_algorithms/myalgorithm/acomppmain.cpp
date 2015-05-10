#include <iostream>
#include <memory>

#include "aco.h"
#include "aco.cpp"
#include "acompp.h"

using namespace rca;

void helpFunction ();

int main (int argv, char **argc) {
    
	if (argv < 15) {
		helpFunction ();
		exit (1);
	}
	
	std::string str = argc[1];
	ACO<AcoMPP> aco;
	
	int iterations = atoi(argc[3]);
	double alpha = atof(argc[5]);
	double betha = atof(argc[7]);
	double phe_rate = atof(argc[9]);
	double prob_heur = atof(argc[11]);
	double local_upd = atof(argc[13]);
	double ref = atof (argc[15]);
	double budget = atof (argc[17]);
	
	double upd1 = atof (argc[18]);
	double upd2 = atof (argc[19]);
	double upd3 = atof (argc[20]);
	
	aco.run (12,str,iterations,alpha,betha,phe_rate,prob_heur,local_upd,ref,budget,upd1, upd2, upd3);
	
	return 0;
}

void helpFunction () {

	printf ("Parameters:\n");
	
	printf ("acompp <instance> --iter <value>");
	printf (" --alpha <value> --betha <value>");
	printf (" --phe <value> --pheur <value> --local_upd <value>");
	printf (" --ref <value> --budget <value>\n");
	
	std::string str = "--ter: define o número máximo de iterações\n";
	str += "--alpha: parâmetro alpha do colônia de formigas(0.01,1.00)\n";
	str += "--betha: parâmetro betha do colônia de formigas(0.01,1.00)\n";
	str += "--phe: parâmetro de evaporação do colônia de formigas(0.01,1.00)\n";
	str += "--pher: valor para escolha da heurística ou informação de feromônio\n";
	str += "--local_upd: valor para atualizar feromônio com base em árvores de Steiner\n";
	str += "--ref: valor de aplicação do operador de refinamento de custo\n";
	str += "--budget: valor de limite de custo\n";
	
	printf ("%s", str.c_str () );
}