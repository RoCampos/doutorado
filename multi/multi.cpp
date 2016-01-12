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
	rca::Network const&, vgroup_t const&, CostModel&);

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

	GRBEnv env = GRBEnv();	
	// env.set(GRB_IntParam_OutputFlag, 0);
	GRBModel m = GRBModel(env);
	
	// BaseModel bm(m, net, v, 5);
	// m.update ();
	// m.write ("teste.lp");

	CostModel costmodel (m, net, multicast_group, 5);

	m.write ("teste.lp");
	m.optimize ();


	// for (rca::Group const& G : multicast_group) {
	// 	to_dot (m, multicast_group, G.getId());
	// }


	return 0;
}

void multiple_runs (GRBModel & m, 
	rca::Network & net, 
	vgroup_t & v, CostModel& costmodel) {

	m.optimize ();

	int Z = 1;
	do {

		//cout << "Objetivo (" << count++ << "): ";
		cout << m.get (GRB_DoubleAttr_ObjVal) << " ";
		cout << m.get (GRB_DoubleAttr_Runtime) << endl;
		m.reset ();
		// Z++;
		costmodel.set_residual_capacity (m, net, v, Z);

		m.optimize ();		

	} while (m.get(GRB_IntAttr_Status) == GRB_OPTIMAL);

}

void to_dot ( GRBModel const& m, 
	vgroup_t const& multicast_group, int G) {

	GRBVar * array = m.getVars ();
	int VAR = m.get (GRB_IntAttr_NumVars);
	int count = 0;

	std::stringstream fname;
	fname << "tmp/" << G << ".dot";
	std::fstream out (fname.str ().c_str (), std::fstream::out);

	cout << "Saving on: " << fname.str () << endl;

	out << "Graph {\n";
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
  				
  				out << atoi (rit->str ().c_str ()) - 1 << " -- ";
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