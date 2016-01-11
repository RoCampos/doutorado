#include <iostream>
#include "gurobi_c++.h"

#include "models.h"
#include "network.h"
#include "reader.h"

using namespace std;

int main(int argc, char const *argv[])
{

	rca::Network net;
	std::vector<rca::Group> v;

	std::string file = argv[1];
	
	std::vector<shared_ptr<rca::Group>> list;	
	MultipleMulticastReader r (file);		
	r.configure_real_values (&net,list);
	for (auto g : list) {
		v.push_back (*g);
	}

	GRBEnv env = GRBEnv();	
	// env.set(GRB_IntParam_OutputFlag, 0);
	GRBModel m = GRBModel(env);
	
	// BaseModel bm(m, net, v, 5);
	// m.update ();
	// m.write ("teste.lp");

	CostModel costmodel (m, net, v, 5);

	int Z = 1;
	m.write ("teste.lp");
	m.optimize ();


	// int count = Z-1;
	// do {

	// 	cout << "Objetivo (" << count++ << "): ";
	// 	cout << m.get (GRB_DoubleAttr_ObjVal) << " ";
	// 	cout << m.get (GRB_DoubleAttr_Runtime) << endl;
	// 	m.reset ();
	// 	// Z++;
	// 	costmodel.set_residual_capacity (m, net, v, Z);

	// 	m.optimize ();		

	// } while (m.get(GRB_IntAttr_Status) == GRB_OPTIMAL);


	// GRBVar * array = m.getVars ();
	// int VAR = m.get (GRB_IntAttr_NumVars);

	// int count = 0;
	// for (int i = 0; i < VAR; ++i)
	// {
	// 	std::string const& var = array[i].get(GRB_StringAttr_VarName);
	// 	if (var.find ("y") != std::string::npos) {
	// 		if (array[i].get (GRB_DoubleAttr_X) == 1) {
	// 			cout << var << " : ";
	// 			cout << array[i].get (GRB_DoubleAttr_X) << endl;	
	// 			count++;
	// 		}			
	// 	} 
	// }

	// cout << "NumVars = " << count << endl;


	return 0;
}