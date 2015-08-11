#include <iostream>

#include "SolverDataGenerator.h"
#include "group.h"
#include "network.h"
#include "reader.h"
#include "steinerReader.h"

void help ();

/**
 * Binário que gera os lps para cada instância do MPP
 * 
 * Usa a implementação de MultipleMulticastCommodityLP
 * 
 */
int main (int argv, char**argc)
{
	int option = atoi(argc[1]);
	
	if ( (strcmp(argc[1], "-h") == 0) ) {
		help ();
		exit (1);
	}
	
	std::string file = argc[2];
	
	Network * net = new Network;
	std::vector<shared_ptr<rca::Group>> g;
	//Reader r (file);
	MultipleMulticastReader r (file);
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
			LeeAndChooModel::set_alpha (atoi (argc[3]));
			LeeAndChooModel::set_opt (argc[4]);
			
			dg.generate (net, g);
			
		}
	}


	return 0;
}

void help ()
{

	std::string msg;
	
	msg = "\n\nThis app is used to create lp files to be used on Gurobi Solver\n";
	msg += "There are some models implemented on it and they receive different\n";
	msg += "of parameters.So if you have doubts type LpGenerator-h\n";
	msg += "\n";
	
	msg += "Option:\n";
	
	msg += "\t1\n";
	
	msg += "\t2\n";
	
	msg += "\t3 - Lee and Cho Moodel: this model maximize residual capacity subject\n";
	msg += "\t\tto alpha*OPT_k on each multicast tree where\n";
	msg += "\t\tParameters: <option><brite><alpha><OPTFILE>\n";
	
	std::cout << msg << std::endl;
}