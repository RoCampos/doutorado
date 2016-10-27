#include <iostream>

#include "SolverDataGenerator.h"
#include "group.h"
#include "network.h"
#include "reader.h"
#include "steinerReader.h"
#include "config.h"

std::string help ();

/**
 * Binário que gera os lps para cada instância do MPP
 * 
 * Usa a implementação de MultipleMulticastCommodityLP
 * 
 */
int main (int argv, char const *argc[])
{
	
	if (message (argv, argc, help()) ) {
		exit (1);
	}

	int option = atoi(argc[1]);
	
	std::string file = argc[2];
	
	Network * net = new Network;
	std::vector<shared_ptr<rca::Group>> g;
	//Reader r (file);
	rca::reader::MultipleMulticastReader r (file);
	//r.configNetwork ( net );
	
#ifdef MODEL_REAL
	r.configure_real_values (net,g);
#endif
	
#ifdef MODEL_UNIT
	r.configure_unit_values (net,g);
#endif
	
	switch (option) {
	
		case 1: {
			
			MMSTPBudgetLP dg;		
			int budget = atoi (argc[3]); //limite de orçamento
			dg.generate2 (net, g, budget);
			
		}
			
		break;
		
		case 2: {
			MMSTPCostLP dg;
			int budget = atoi (argc[3]); //capacidade residual como limite
			dg.generate2 (net, g, budget);
			
		}
		break;
		
		case 3: {
			
			LeeAndChooModel dg;		
			LeeAndChooModel::set_alpha (atof (argc[3]));
			LeeAndChooModel::set_opt (argc[4]);
			
			dg.generate (net, g);
			
		}
	}


	return 0;
}

std::string help ()
{

	std::string msg;
	
	msg = "\n\nThis app is used to create lp files to be used on Gurobi Solver\n";
	msg += "There are some models implemented on it and they receive different\n";
	msg += "of parameters.So if you have doubts type LpGenerator-h\n";
	msg += "\n";
	
	msg += "Option:\n";
	
	msg += "\t1 -MMSTPBudgetLP: Gera lp considerando um limite minimo de capacidade residual\n";
	msg += "\t\t Paramêtros: option, instance, BUDGET\n"; 	
	
	msg += "\t2 -MMSTPCostLP: Gera lp considerando um limite minimo de capacidade residual\n";
	msg += "\t\t Paramêtros: option, instance, Z\n"; 	
	
	msg += "\t3 - Lee and Cho Moodel: this model maximize residual capacity subject\n";
	msg += "\t\tto alpha*OPT_k on each multicast tree where\n";
	msg += "\t\tParameters: <option><brite><alpha><OPTFILE>\n";
	
	return msg;
}