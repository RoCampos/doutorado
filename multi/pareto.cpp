#include <iostream>
#include "gurobi_c++.h"

#include "models.h"
#include "network.h"
#include "reader.h"
#include "steiner.h"
#include "group.h"

#include <boost/regex.hpp>

using namespace std;

typedef struct pareto_ind_t {
	std::vector<int> objective;

	pareto_ind_t (int v1, int v2, int v3) {
		objective.push_back (v1);
		objective.push_back (v2);
		objective.push_back (v3);
	}

	bool operator==  (pareto_ind_t const& ind) {
		if (ind.objective.at (0) == objective.at (0) &&
			ind.objective.at (1) == objective.at (1) &&
			ind.objective.at (2) == objective.at (2))
			{return true;}
		return false;
	}

	int at (int obj) {
		return objective.at (obj);
	}
};

/* ESTA VARIÁVEL GUARDA O TAMANHO MÁXIMO DOS CAMINHOS*/
int LIMIT_PATH;
int NODES;
int GROUPS;

//shortest path(hop count) for the optmization of cost subject to Z
int MIN;

//optimal front
std::vector<pareto_ind_t> poptimal;

void to_dot (GRBModel const& model, 
	vgroup_t const& multicast_group, int G);

void bi_objective (GRBModel & model, 
	rca::Network &, vgroup_t &, HopCostModel&, int);

int three_objective (GRBEnv &model, 
	rca::Network&, vgroup_t&, int);


bool dominance (pareto_ind_t&);

void print_x_var (int x, int y, int k, int d, GRBModel const&);

//recebe modelo e o valor de Z (count)
void print_solution (GRBModel const& model, int count, vgroup_t & groups);

void teste_solution (steiner & st, rca::Group const& g, int Z) {

	auto members = g.getMembers ();
	int source = g.getSource ();

	int count = 0;
	for (int member : members) {

		auto path = st.get_path (source, member);
		if (path.size () - 1 > LIMIT_PATH) {
			count++;			
		}
	}

	printf("Invalid paths %d for Z (%d)\n", count, Z);

}

void help () {

	std::cout << "This programa is used to generate the pareto front to the multicast\n";
	cout << "Routing Problem with multiple multicast group.\n";
	cout << "It is possible to generate the pareto front for two or three objectives.\n\n";

	std::cout << "\nUsage:\n";
	cout << "\t paretogen [--brite <instance>] [--Z <int value>] [--objectives <inv value>]\n";

	std::stringstream ss;
	std::cout << "\nOptions:\n";
	ss << "\t --brite	type of instance used to be solver\n";
	ss << "\t --Z 	initial value for the residual capacity. It permits restart the process.\n";
	ss << "\t\t Z value can be 0 or MAX residual capacity.\n";
	ss << "\t --objectives	int value that indicates the number of objectives [two or three].\n";
	ss << "\t\t When set two objectives, hop limit is set to 5.\n";

	cout << ss.str () << endl;

	std::cout << "Examples:\n";

	cout << "\tparetogen --brite b30_1.brite --Z 32 --objectives 3\n";
	cout << "\tparetogen --brite b30_1.brite --Z 32 --objectives 2\n";

}



int main(int argc, char const *argv[])
{

	int STOP = 5;
	MIN = std::numeric_limits<int>::max();

	if (argc == 1) {
		help ();
		exit (0);
	}

	rca::Network net;
	std::vector<rca::Group> multicast_group;

	//instance to be optimized
	std::string file = argv[1];
	if (file.compare ("--brite") == 0) {
		file = argv[2];
	} else {
		help ();
		exit (0);
	}
	
	std::vector<shared_ptr<rca::Group>> temp;	
	rca::reader::MultipleMulticastReader r (file);		
	r.configure_real_values (&net,temp);
	for (auto g : temp) {
		multicast_group.push_back (*g);
	}

	std::string zvalue = argv[3];
	int Z = 0;
	if (zvalue.compare ("--Z") == 0) {
		Z = atoi (argv[4]);
	} else {
		help ();
		exit (0);
	}
	

	std::string objectives = argv[5];
	int obj = 2;
	if (objectives.compare ("--objectives") == 0) {
		obj = atoi (argv[6]);
	} else {
		help ();
		exit (0);
	}

	GRBEnv env = GRBEnv();	
	env.set(GRB_IntParam_LogToConsole, 0);
	GRBModel m = GRBModel(env);

	if (obj == 2) {
		LIMIT_PATH = 5;
		NODES = net.getNumberNodes ();
		GROUPS= multicast_group.size ();
		HopCostModel costmodel(m, net, multicast_group, LIMIT_PATH, Z);
		bi_objective (m, net, multicast_group, costmodel, Z);	
	} else {
		int count = 0;
		int result = 0;
		do {
			result = three_objective (env, net, multicast_group, Z++);
			count += result;
		} 
		while (result > 0);

		for (auto && x : poptimal) {
			if (dominance(x)) {
				cout << x.at (0) << " " << x.at(1) << " " << x.at (2) << endl;
			}
		}

	}

	return 0;
}
	

void bi_objective (GRBModel & m, 	
	rca::Network & net, 
	vgroup_t & v, HopCostModel& costmodel, int Z) {

	std::stringstream name;
	name << "pareto_" << ( Z < 10 ? "0" : "") << Z << ".log";

	std::stringstream modelname;
	modelname << "CostModel_" << ( Z < 10 ? "0" : "") << Z << ".lp";


	std::stringstream solname;
	solname << "Solution_" << (Z < 10 ? "0" : "") << Z << ".sol";


	m.getEnv().set (GRB_StringParam_LogFile, name.str ());
	m.write (modelname.str ());
	m.optimize ();

	std::fstream pareto ("pareto.txt", std::fstream::out );
	std::fstream ftime ("time.txt", std::fstream::out);

	int count = Z;
	double _time_ = 0.0;
	do {

		pareto << count << " ";
		pareto << m.get (GRB_DoubleAttr_ObjVal) << " ";
		//time is wall-clock, reported in seconds
		_time_ += m.get (GRB_DoubleAttr_Runtime);
		pareto << m.get (GRB_DoubleAttr_Runtime) << endl;
		// m.reset ();
		costmodel.set_residual_capacity (m, net, v, 1);

		name.clear ();
		name.str ("");
		modelname.clear ();
		modelname.str ("");

		count += 1;		
		
		name << "pareto_" << (count < 10 ? "0" : "") << count << ".log";
		modelname << "CostModel_" << (count < 10 ? "0" : "") << count << ".lp";

		m.getEnv().set (GRB_StringParam_LogFile, name.str ());		
		m.write (modelname.str ());

		m.optimize ();

		//testing the solutio to see if path has LIMIT_PATH size
		print_solution (m, count, v);

	} while (m.get(GRB_IntAttr_Status) == GRB_OPTIMAL);

	ftime << _time_ << endl;

	ftime.close ();
	pareto.close ();
}

int three_objective (GRBEnv & env,  
	rca::Network& net, 
	vgroup_t& mgroups, 
	int Z)
{

	int STOP = 5;

	std::fstream pareto ("pareto.txt", std::fstream::app);
	std::fstream ftime ("time.txt", std::fstream::app);

	//core events

	bool continue_loop = true;
	int repetions = 0; //controle if the algorithm doen`t improve.
	int previous = 0; //number of repetions without improvements
	int path_limit = 0; //the size of the path(initilizaded with 0)

	int number_solution = 0;

	bool first_sol = false;

	double _time1_ = 0.0, _time2_ = 0.0;

	do {
		GRBModel x = GRBModel(env);
		HopCostModel costmodel(x, net, mgroups, path_limit, Z);

		try	{
			x.optimize ();
			_time2_ += x.get (GRB_DoubleAttr_Runtime);
			int cost = (int)x.get(GRB_DoubleAttr_ObjVal);
			_time1_ += x.get (GRB_DoubleAttr_Runtime);

			pareto_ind_t sol (Z, cost, path_limit);
			poptimal.push_back (sol);
			
			//output
			pareto << Z << " " << cost << " " << path_limit;
			pareto << " " << _time1_ << endl;
			
			number_solution++;

			std::stringstream name;
			name << "pareto_" << ( Z < 10 ? "0" : "") << Z;
			name << "_" << (path_limit < 10 ? "0" : "") << path_limit << ".log";
			
			std::stringstream modelname;
			modelname << "CostModel_" << ( Z < 10 ? "0" : "") << Z;
			modelname << "_" << (path_limit < 10 ? "0" : "") << path_limit << ".lp";
			
			std::stringstream solname;
			solname << "Solution_" << (Z < 10 ? "0" : "") << Z;
			solname << "_" << (path_limit < 10 ? "0" : "") << path_limit << ".sol";
			
			x.getEnv().set (GRB_StringParam_LogFile, name.str ());
			x.write (modelname.str ());

			if (path_limit < MIN) {
				MIN = path_limit;
			}

			if (previous == x.get (GRB_DoubleAttr_ObjVal)) {
				repetions++;
			} else {
				previous = x.get (GRB_DoubleAttr_ObjVal);
			}
			//stop condicion the path limit has growing and the cost is the same
			//for STOP repetions
			if (repetions == STOP) {
				continue_loop = false;
			}

			path_limit++;

		} catch (const GRBException& e) {
			std::cerr << __LINE__ <<":"<< e.getMessage() << '\n';
			//if it is not possible to running for a path limit
			path_limit++;

			//the model cannot be solve because of Z is over the best value
			//x.optimize () will throw an exception
			if (path_limit > MIN) {
				continue_loop = false;
			}
		}

	} while (continue_loop);

	ftime.close ();
	pareto.close ();

	return (number_solution);

}

bool dominance (pareto_ind_t& sol)
{
	for (auto && x : poptimal) {
		//Z, cost, hop
		if (x.at (0) >= sol.at (0) && 
			x.at(1) <= sol.at(1) && 
			x.at (2) <= sol.at (2) && !(x==sol)) {
			return (false);
		}
	}
	return (true);
}

void to_dot ( GRBModel const& m, 
	vgroup_t const& multicast_group, int G) {

	GRBVar * array = m.getVars ();
	int VAR = m.get (GRB_IntAttr_NumVars);

	std::stringstream fname;
	fname << "tmp/" << G << ".dot";
	std::fstream out (fname.str ().c_str (), std::fstream::out);

	cout << "Saving on: " << fname.str () << endl;

	out << "digraph {\n";
	out << multicast_group[G].getSource ();
	out << " [style=filled fillcolor=blue] ;\n";
	for (auto d : multicast_group[G].getMembers ()) {

		out << d << " [style=filled fillcolor=red] ;\n";	
		
	}

	std::stringstream ss;
	ss << G+1 << ")";
	

	for (int i = 0; i < VAR; ++i)
	{
		std::string var = array[i].get(GRB_StringAttr_VarName);
		if (var.find ("y") != std::string::npos) {

			if (var.find(ss.str ()) == std::string::npos) continue;

			if (array[i].get (GRB_DoubleAttr_X) == 1) {
				
				boost::regex ptn ("\\d+");
				boost::sregex_iterator rit ( var.begin(), var.end(), ptn );
  				boost::sregex_iterator rend;
  				
  				out << atoi (rit->str ().c_str ()) - 1 << " -> ";
  				rit++;
  				out << atoi (rit->str ().c_str ()) - 1 << " ;\n";
  				rit++;
  				// cout << atoi (rit->str ().c_str ()) - 1 << endl;

			}			
		} 
	}
	out << "}\n";

	out.close ();

}

void print_solution (GRBModel const& m, int count, vgroup_t & groups) {

	GRBVar * vars = m.getVars ();
	int NUM_VAR = m.get (GRB_IntAttr_NumVars);

	std::stringstream solname;
	solname << "pareto_" << (count < 10 ? "0" : "") << count << ".sol";

	std::fstream file (solname.str(), std::fstream::out);

	std::vector<steiner> mtrees;

	for (int i = 0; i < GROUPS; ++i)
	{
		steiner st(NODES, 
				groups[i].getSource (), 
				groups[i].getMembers ());

		mtrees.push_back (st);
	}

	for (int i = 0; i < NUM_VAR; ++i)
	{
		std::string var = vars[i].get(GRB_StringAttr_VarName);
		if (var.find ("y") != std::string::npos) {

			if (vars[i].get (GRB_DoubleAttr_X) == 1) {
				
				boost::regex ptn ("\\d+");
				boost::sregex_iterator rit ( var.begin(), var.end(), ptn );
  				boost::sregex_iterator rend;
  				
  				int x = atoi (rit->str ().c_str ());
  				rit++;
  				int y = atoi (rit->str ().c_str ());
  				rit++;
  				int k = atoi (rit->str ().c_str ());

  				file << x << " - " << y << ":" << k << ";\n";

  				//criando st trees
  				mtrees[k-1].add_edge (x-1, y-1, 1);

			}

		}
	}


	for (int i = 0; i < GROUPS; ++i)
	{
		// mtrees[i].print ();
		teste_solution (mtrees[i], groups[i], count);
	}


	vars = NULL;
	file.close ();
}

void print_x_var (int x, int y, int k, int d, GRBModel const& m) {

	int comp = 0;
	if (x != -1) comp++;
	if (k != -1) comp++;
	if (d != -1) comp++;
	if (y != -1) comp++;

	GRBVar * vars = m.getVars ();
	int VAR = m.get (GRB_IntAttr_NumVars);

	boost::regex expr ("\\d+");

	for (int x_id = 0; x_id < VAR; ++x_id)
	{
		GRBVar var = vars[x_id];

		std::string const& varname = var.get(GRB_StringAttr_VarName);

		boost::sregex_iterator rit ( varname.begin(), varname.end(), expr);
  		boost::sregex_iterator rend;

  		if (std::distance (rit, rend) == 3) continue;

  		int xx = atoi (rit->str().c_str());
  		rit++;
  		int yy = atoi (rit->str().c_str());
  		rit++;
  		int kk = atoi (rit->str().c_str());
  		rit++;
  		int dd = atoi (rit->str().c_str());

  		int count = 0;
  		if (xx == x) count++;
  		if (yy == y) count++;
  		if (kk == k) count++;
  		if (dd == d) count++;

  		if (count == comp) {
  			cout << varname << ":" << var.get (GRB_DoubleAttr_X) << endl;
  		}

	}

}