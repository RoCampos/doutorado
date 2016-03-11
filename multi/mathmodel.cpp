#include <iostream>

#include "models.h"
#include "gurobi_c++.h"
#include "network.h"
#include "reader.h"


/* Enum that defines the options to choose Gurobi Models*/
enum Option {
	NONE = -1,
	LEE_MODEL = 0,
	LEE_MODEL_MODIFIED = 1,
	BUDGET_MODEL = 2,
	COST_Z_MODEL = 3
};

void write_results (vsolution_t &, 
	rca::Network& net, GRBModel & model, int type_model);

void write_log_model (GRBModel & m) {

	std::stringstream name;
	name << "file.log";

	std::stringstream modelname;
	modelname << "modelo.lp";

	try {
		m.getEnv ().set (GRB_StringParam_LogFile, name.str ());
		m.getEnv ().set (GRB_IntParam_Threads, 1);
		m.write (modelname.str ());
	} catch (GRBException & e) {
		cout << e.getMessage () << endl;
		cout << e.getErrorCode () << endl;
	}

}

void RunLeeModel (rca::Network & net, 
	std::vector<rca::Group> &  groups,
	std::vector<double> & opt, double alpha) {

	GRBEnv env = GRBEnv ();
	GRBModel modelo = GRBModel (env);

	std::vector<double> opt_list;
	for (auto value : opt) {
		opt_list.push_back (value * alpha);
	}

	LeeModel (modelo, net, groups, opt_list);

	write_log_model (modelo);

	modelo.optimize ();

	auto sol = solution_info ("file", modelo, groups.size());

	write_results (sol, net, modelo, LEE_MODEL);
	
}

void RunBudgetModel (rca::Network &net, 
	std::vector<rca::Group> & groups,
	int budget) {


	GRBEnv env = GRBEnv ();
	GRBModel modelo = GRBModel (env);
	
	BudgetModel (modelo, net, groups, budget);

	write_log_model (modelo);

	modelo.optimize ();

	auto sol = solution_info ("file", modelo, groups.size());

	write_results (sol, net, modelo, NONE);

}

void RunLeeModifiedModel (rca::Network & net, 
	std::vector<rca::Group> &  groups,
	std::vector<double> & opt, double alpha) {

	GRBEnv env = GRBEnv ();
	GRBModel modelo = GRBModel (env);

	std::vector<double> opt_list;
	for (auto value : opt) {
		opt_list.push_back (value * alpha);
	}


	LeeModifiedModel (modelo, net, groups, opt_list);

	write_log_model (modelo);

	modelo.optimize ();

	auto sol = solution_info ("file", modelo, groups.size());

	write_results (sol, net, modelo, NONE);

}

//modelo que otimiza custo sujeito a Z
void RunCostZModel (rca::Network & net, 
	std::vector<rca::Group> &  groups,  int Z) {

	GRBEnv env = GRBEnv ();
	GRBModel modelo = GRBModel (env);

	BZModel (modelo, net, groups, Z);

	write_log_model (modelo);

	modelo.optimize ();	

	auto sol = solution_info ("file", modelo, groups.size());

	write_results (sol, net, modelo, LEE_MODEL_MODIFIED);

}

void help () {
	cout << "\n\tMathematical Models\n" << endl;

	std::stringstream ss;
	ss << "This program is used to run the following models on";
	ss << " gurobi solver:\n";
	ss << "\t LeeModel\n\t LeeModifiedModel\n\t BudgetModel";
	ss << "\nThe commands to run each of the models are the following:";
	ss << "\n\t -  ./build/mathmodel --model LM --instance <INST> --opt <OPT_FILE>";
	ss << " --alpha <double value>";
	ss << "\n\t -  ./build/mathmodel --model LMM --instance <INST> --opt <OPT_FILE>";
	ss << " --alpha <double value>";
	ss << "\n\t -  ./build/mathmodel --model BM --instance <INST> --budget <INT_VALUE>";
	ss << "\nThe parameters passed to models are: ";
	ss << "\n\t --opt: this parameter represent a list of double\n values for LM model, ";
	ss << "these values represent a limit in the cost of each tree";
	ss << "\n\t --opt: this parameter also represent a list of int\n values for LMM model, ";
	ss << "\n\t --alpha: this parameter represents a percentage of growning of a tree";
	ss << "these values represent a limit in the size of each tree";
	ss << "\n\t --budget: this parameter represent a represent a \nvalue of the global solution,";
	ss << "that contains all the trees and its respective costs";
	ss << "\n";


	cout << ss.str () << endl;

	exit (1);
}

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

	if (opt.compare ("BZ") == 0) {
		return 3;
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
	r.configure_real_values (&net,temp);
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

			std::string alphastr = argv[7];
			double alpha = 1.1;
			if (alphastr.compare ("--alpha") == 0 ) {
				alpha = atof(argv[8]);
			} else {
				help();
			}
			
			if (opt.size () != multicast_group.size ()) {
				cerr << "opt_list must have the same number of groups\n";
				exit (1);
			}

			RunLeeModel (net, multicast_group, opt, alpha);


		}break;

		case Option::LEE_MODEL_MODIFIED : {

			std::vector<double> opt;
			std::string opt_file = argv[5];
			if (opt_file.compare ("--opt") == 0) {
				opt_file = argv[6];
				opt = read_opt_file (opt_file);
			} else {
				help();
			}

			std::string alphastr = argv[7];
			double alpha = 1.1;
			if (alphastr.compare ("--alpha") == 0 ) {
				alpha = atof(argv[8]);
			} else {
				help();
			}
			
			if (opt.size () != multicast_group.size ()) {
				cerr << "opt_list must have the same number of groups\n";
				exit (1);
			}

			RunLeeModifiedModel (net, multicast_group, opt, alpha);

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
		case Option::COST_Z_MODEL : {
			std::string opt_budget = argv[5];
			int Z = -1;
			if (opt_budget.compare ("--Z") == 0) {
				Z = atoi (argv[6]);
			} else {
				help ();
			}

			cout << "Z: " << Z << endl;
			RunCostZModel (net, multicast_group, Z);

		}break;

		default: {
			help ();			
		}
	}


	return 0;
}

void write_results (vsolution_t & sol, 
	rca::Network& net, GRBModel & model, int type) {

	try {
		cerr << "Objec\tBound\tMIPGap\n";

		cerr << model.get (GRB_DoubleAttr_ObjVal) << "\t";
		cerr << model.get (GRB_DoubleAttr_ObjBound) << "\t";
		cerr << model.get (GRB_DoubleAttr_MIPGap) << endl;

		cerr << "Runtime: " << model.get (GRB_DoubleAttr_Runtime) << endl;

		cerr << endl;

		cerr << "Size\tCost" << endl;
		double ocost = 0.0;
		for (auto list : sol) {
			int cost_tree = 0;
			for (auto l : list) {
				cost_tree += net.getCost (l.getX(), l.getY());
				
			}
			ocost += cost_tree;
			cerr << list.size () << "\t" << cost_tree << endl;
		}


		if (type == LEE_MODEL || type == LEE_MODEL_MODIFIED) {
			cerr << "Overall Cost: " << ocost << endl;
		} 

		model.write ("Sol.sol");

	}
	catch(const GRBException& e) {
		std::cerr << e.getMessage () << '\n';
	}


}