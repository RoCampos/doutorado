#include <iostream>

#include "models.h"
#include "gurobi_c++.h"
#include "network.h"
#include "reader.h"

#include "command.h"

void RunLeeModel (rca::Network & net, 
	std::vector<rca::Group> &  groups,
	std::vector<double> & opt, int alpha) {

	GRBEnv env = GRBEnv ();
	GRBModel modelo = GRBModel (env);
	LeeModel (modelo, net, groups, opt);

	modelo.optimize ();

}

void RunBudgetModel (rca::Network &net, 
	std::vector<rca::Group> & groups,
	int budget) {


	GRBEnv env = GRBEnv ();
	GRBModel modelo = GRBModel (env);
	
	BudgetModel (modelo, net, groups, budget);

	modelo.optimize ();

}

void help () {
	cout << "Help under construction!" << endl;
	exit (1);
}

enum Option {

	LEE_MODEL = 0,
	LEE_MODEL_MODIFIED = 1,
	BUDGET_MODEL = 2
};

int get_option (std::string & opt) {
	
	if (opt.compare ("LM") == 0) {
		return 0;
	}

	if (opt.compare ("LMM") == 0) {
		return 1;
	}

	if (opt.compare ("BM") == 0) {
		return 2;
	}

	return -1;
}

std::vector<double> read_opt_file (std::string & file) {
	
	std::fstream _file (file.c_str (),std::fstream::in);
	std::vector<double> v;
	double value;
	_file >> value;
	do {

		v.push_back (value);
		_file >> value;

	}while (_file.good());
	_file.close ();

	return v;
}

int main(int argc, char const *argv[])
{

	rca::Network net;
	std::vector<rca::Group> multicast_group;

	if (argc == 1) {
		help ();
	}

	std::string model = argv[1];	
	if (model.find ("--model") == std::string::npos) {
		help ();
	}
	model = argv[2];

	std::string file = argv[3];
	if (file.compare ("--instance") == 0) {
		file = argv[4];
	} else {
		help ();		
	}

	std::vector<shared_ptr<rca::Group>> temp;	
	MultipleMulticastReader r (file);		
	r.configure_unit_values (&net,temp);
	for (auto g : temp) {
		multicast_group.push_back (*g);
	}

	int option = get_option (model);	
	switch (option) {

		case Option::LEE_MODEL : {

			std::vector<double> opt;
			std::string opt_file = argv[5];
			if (opt_file.compare ("--opt") == 0) {
				opt_file = argv[6];
				opt = read_opt_file (opt_file);
			} else {
				help();
			}
			
			if (opt.size () != multicast_group.size ()) {
				cerr << "opt_list must have the same number of groups\n";
				exit (1);
			}

			RunLeeModel (net, multicast_group, opt, 0);

		}break;

		case Option::LEE_MODEL_MODIFIED : {

		}break;

		case Option::BUDGET_MODEL : {

			std::string opt_budget = argv[5];
			int budget = -1;
			if (opt_budget.compare ("--budget") == 0) {
				budget = atoi (argv[6]);
			} else {
				help ();
			}

			RunBudgetModel (net, multicast_group, budget);


		}break;
		default: {
			help ();			
		}
	}


	


	return 0;
}