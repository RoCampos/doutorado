#include <iostream>
#include "gurobi_c++.h"

#include "models.h"
#include "network.h"
#include "reader.h"

#include <boost/regex.hpp>

using namespace std;

void to_dot (GRBModel const& model, 
	vgroup_t const& multicast_group, int G);

void multiple_runs (GRBModel & model, 
	rca::Network &, vgroup_t &, CostModel&, int);

void print_x_var (int x, int y, int k, int d, GRBModel const&);

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

	CostModel costmodel (m, net, multicast_group, 5, Z);

	multiple_runs (m, net, multicast_group, costmodel, Z);

	return 0;
}

void multiple_runs (GRBModel & m, 	
	rca::Network & net, 
	vgroup_t & v, CostModel& costmodel, int Z) {

	std::stringstream name;
	name << "pareto_" << ( Z < 10 ? "0" : "") << Z << ".log";

	std::stringstream modelname;
	modelname << "CostModel_" << ( Z < 10 ? "0" : "") << Z << ".lp";

	m.getEnv().set (GRB_StringParam_LogFile, name.str ());
	m.write (modelname.str ());
	m.optimize ();

	std::fstream pareto ("pareto.txt", std::fstream::out );
	std::fstream ftime ("time.txt", std::fstream::out);

	int count = Z;
	double _time_ = 0.0;
	do {

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