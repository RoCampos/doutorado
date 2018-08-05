#include <iostream>

#include "models.h"
#include "gurobi_c++.h"
#include "network.h"
#include "reader.h"
#include "multisource_group.h"


/* Enum that defines the options to choose Gurobi Models*/
enum Option {
	NONE = -1,
	LEE_MODEL = 0,
	LEE_MODEL_MODIFIED = 1,
	BUDGET_MODEL = 2,
	COST_Z_MODEL = 3,
	RESIDUAL_MODEL = 4,
	HOP_MODEL = 5
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

	write_results (sol, net, modelo, LEE_MODEL_MODIFIED);

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

	write_results (sol, net, modelo, NONE);

}

void RunResidualModel (rca::Network & net, 
	std::vector<rca::Group> &  groups) {

	GRBEnv env = GRBEnv ();
	GRBModel modelo = GRBModel (env);

	ConcreteResidualModel (modelo, net, groups);

	write_log_model (modelo);

	modelo.optimize ();	

	auto sol = solution_info ("file", modelo, groups.size());

	write_results (sol, net, modelo, RESIDUAL_MODEL);

}

void RunHopModel (rca::Network & net, 
	std::vector<rca::Group> &groups, int hoplimit, int Z) {

	GRBEnv env = GRBEnv ();
	GRBModel modelo = GRBModel (env);

	//build the model
	HopCostModel (modelo, net, groups, hoplimit, Z);

	write_log_model (modelo);
	modelo.optimize ();
	auto sol = solution_info ("file", modelo, groups.size ());

	write_results (sol, net, modelo, HOP_MODEL);

}

void help () {
	cout << "\nUsage:\n" << endl;
	std::stringstream ss;
	ss << "\tmathmodel [--model <math_model>] [--brite <type>] [--opt|--budget|--hop|--Z|--alpha]\n";
	ss << "\nOptions:\n";

	ss << "\t--model	mathematical model to be used\n";
	ss << "\t\t LEE MODEL (LM)\n";
	ss << "\t\t LEE MODEL MODIFIED (LMM)\n";
	ss << "\t\t BUDGET MODEL (BM)\n";
	ss << "\t\t RESIDUAL MODEL (RM)\n";
	ss << "\t\t COST_Z_MODEL (BZ)\n";
	ss << "\t\t HOP MODEL (HM)\n";

	ss << "\t--brite	type of instance used.\n";

	ss << "\t--opt	a list of double values associated with each multicast tree of the instance.\n";
	ss << "\t\t Applyied only with LM and LMM models.\n";
	ss << "\t\t In LM version, opt means the number of links (optimal) of each tree.\n";
	ss << "\t\t In LMM version, opt means the cost (optimal) of each tree.\n";

	ss << "\t--budget	a budget value used to limit maximum solution cost\n";
	ss << "\t\t Applyied only with BM model.\n";

	ss << "\t--Z	minimum residual capacity requirement for the solution\n";
	ss << "\t\t Applyied only with BZ and HM models.\n";

	ss << "\t--alpha	a parameter to control the size of Optimal trees\n";
	ss << "\t\t Applyied only with LM and LMM models.\n";

	ss << "\nExamples:\n";

	ss << "\tmathmodel --model LM --brite b30_1.brite --opt optfile --alpha 1.5\n";
	ss << "\tmathmodel --model LMM --brite b30_1.brite --opt optfile --alpha 1.5\n";
	ss << "\tmathmodel --model BM --brite b30_1.brite --budget 1000000\n";
	ss << "\tmathmodel --model RM --brite b30_1.brite\n";
	ss << "\tmathmodel --model BZ --brite b30_1.brite --Z 32\n";
	ss << "\tmathmodel --model HM --brite b30_1.brite --Z 32 --hop 5\n";

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

	if (opt.compare ("RM") == 0) {
		return 4;
	}

	if (opt.compare ("HM") == 0) {
		return 5;
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


using stream_t = rca::network::stream_t;
using msource_list_t = std::vector<stream_t> ;
void read_instace (
	std::string type,
	std::string file, 
	rca::Network & network, 
	std::vector<rca::Group> & mgroups) 
{
	
	if (type.compare("--brite") == 0) {
		rca::reader::get_problem_informations (
			file, network, mgroups);

	} else if (type.compare ("--yuh") == 0 ){

		rca::reader::YuhChenReader ycr(file);
		ycr.configure_network (network, mgroups);

		for (int i = 0; i < network.getNumberNodes(); ++i)
		{
			for (int j = 0; j < network.getNumberNodes(); ++j)
			{
				if (network.getCost (i,j) > 0.0) {
					network.setBand (i,j, mgroups.size ());
				}
			}
		}
	}	
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

	std::string type = argv[3];
	std::string file;
	if (type.compare ("--brite") == 0 || type.compare ("--yuh") == 0) {
		file = std::string (argv[4]);
	} else {
		help ();		
	}

	read_instace (type, file, net, multicast_group);

	int option = get_option (model);	
	switch (option) {

		case Option::LEE_MODEL : {

			cout << argv[6] << endl;

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

			RunCostZModel (net, multicast_group, Z);

		}break;

		case Option::RESIDUAL_MODEL : {

			RunResidualModel (net, multicast_group);

		}break;

		case Option::HOP_MODEL : {
			std::string opt_budget = argv[5];
			int Z = -1;
			if (opt_budget.compare ("--Z") == 0) {
				Z = atoi (argv[6]);
			} else {
				help ();
			}
			cout << "Z: " << Z << endl;

			std::string hop_limit = argv[7];
			int hop = 0;
			if (hop_limit.compare ("--hop") ==0) {
				hop = atoi (argv[8]);
			} else {
				help ();
			}

			RunHopModel (net, multicast_group, hop, Z);

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


		if (type == LEE_MODEL || type == LEE_MODEL_MODIFIED || type == RESIDUAL_MODEL) {
			cerr << "Overall Cost: " << ocost << endl;
		} 

		model.write ("Sol.sol");

	}
	catch(const GRBException& e) {
		std::cerr << e.getMessage () << '\n';
	}


}