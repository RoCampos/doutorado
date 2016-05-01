#include <iostream>
#include "gurobi_c++.h"

#include "models.h"
#include "network.h"
#include "reader.h"
#include "steiner.h"
#include "group.h"

#include <boost/regex.hpp>

using namespace std;

/* ESTA VARIÁVEL GUARDA O TAMANHO MÁXIMO DOS CAMINHOS*/
int LIMIT_PATH;
int NODES;
int GROUPS;

void to_dot (GRBModel const& model, 
	vgroup_t const& multicast_group, int G);

void multiple_runs (GRBModel & model, 
	rca::Network &, vgroup_t &, HopCostModel&, int);

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



int main(int argc, char const *argv[])
{

	rca::Network net;
	std::vector<rca::Group> multicast_group;

	std::string file = argv[1];
	
	std::vector<shared_ptr<rca::Group>> temp;	
	MultipleMulticastReader r (file);		
	r.configure_real_values (&net,temp);
	for (auto g : temp) {
		multicast_group.push_back (*g);
	}

	//getting the parameter Z that was the last
	//case of optimizationg
	int Z = 0;
	if (argc > 2) {
		Z = atoi (argv[2]);
	}

	GRBEnv env = GRBEnv();	
	env.set(GRB_IntParam_LogToConsole, 0);
	GRBModel m = GRBModel(env);

	LIMIT_PATH = 5;
	NODES = net.getNumberNodes ();
	GROUPS= multicast_group.size ();

	HopCostModel costmodel(m, net, multicast_group, LIMIT_PATH, Z);

	multiple_runs (m, net, multicast_group, costmodel, Z);

	return 0;
}

void multiple_runs (GRBModel & m, 	
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