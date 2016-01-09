#include <iostream>
#include "gurobi_c++.h"

#include "models.h"
#include "network.h"
#include "reader.h"

using namespace std;

int main(int argc, char const *argv[])
{

	// try {

	// 	GRBEnv env = GRBEnv ();
	// 	GRBModel model = GRBModel (env);

	//     // Create variables

	//     GRBVar x = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "x");
	//     GRBVar y = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "y");
	//     GRBVar z = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "z");

	//     // Integrate new variables

	//     model.update();

	//     // Set objective: maximize x + y + 2 z

	//     model.setObjective(x + y + 2 * z, GRB_MAXIMIZE);

	//     // Add constraint: x + 2 y + 3 z <= 4

	//     model.addConstr(x + 2 * y + 3 * z <= 4, "c0");

	//     // Add constraint: x + y >= 1

	//     model.addConstr(x + y >= 1, "c1");

	//     // Optimize model

	//     model.optimize();

	//     cout << x.get(GRB_StringAttr_VarName) << " "
	//          << x.get(GRB_DoubleAttr_X) << endl;
	//     cout << y.get(GRB_StringAttr_VarName) << " "
	//          << y.get(GRB_DoubleAttr_X) << endl;
	//     cout << z.get(GRB_StringAttr_VarName) << " "
	//          << z.get(GRB_DoubleAttr_X) << endl;

	//     cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << endl;


	// } catch (GRBException e) {

	// 	cout << "nothing" << endl;

	// }


	/* code */

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
	GRBModel m = GRBModel(env);
	
	BaseModel bm(m, net, v, 5);

	m.update ();
	m.write ("teste.lp");

	return 0;
}