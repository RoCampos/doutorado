#include "models.h"

using std::cout;
using std::endl;

void BaseModel::create_variables(GRBModel & grbmodel, 
	rca::Network& net, vgroup_t & groups) {

	size_t GROUPS = groups.size ();

	for (auto link : net.getLinks()) {
		for (size_t k = 0; k < GROUPS; ++k) {			
			auto members = groups[k].getMembers ();
			for (auto m : members) {
			
				std::string var1 = 
					get_var_name (link.getX(),link.getY(), k, m);

				std::string var2 = 
					get_var_name (link.getY(),link.getX(), k, m);

				try {
					grbmodel.addVar (0,1,1, GRB_BINARY,var1);
					grbmodel.addVar (0,1,1, GRB_BINARY,var2);
				}
				catch(const GRBException& e) {
					std::cerr << __LINE__ << e.getMessage() << '\n';
				}
				

			}

			std::string const& var_y1 = 
				get_y_var_name (link.getX(), link.getY(), k);
			grbmodel.addVar (0,1,1, GRB_BINARY,var_y1);

			std::string const& var_y2 = 
				get_y_var_name (link.getY(), link.getX(), k);
			grbmodel.addVar (0,1,1, GRB_BINARY,var_y2);

		}
	}

	grbmodel.update ();

}

void BaseModel::flow1 (GRBModel & grbmodel, 
	rca::Network& net, vgroup_t & groups) {

	size_t GROUPS = groups.size ();
	
	for (size_t k = 0; k < GROUPS; ++k)
	{
		int source = groups[k].getSource ();
		
		for (auto d : groups[k].getMembers ()) {
			
			GRBLinExpr sum1 = 0, sum2 = 0;
			std::stringstream ss;
			ss << "flow1(" << k+1 << "," << d+1 << ")";

			for (auto link : net.getEdges (source) ) {

				//getting the name of variables
				const std::string var1 = 
					get_var_name (link.getX(),link.getY(), k, d);

				//getting the name of variables
				const std::string var2 = 
					get_var_name (link.getY(),link.getX(), k, d);
	
				//getting the variables
				GRBVar v1, v2;
				try {
					v1 = grbmodel.getVarByName (var1);
					v2 = grbmodel.getVarByName (var2);
				}
				catch(const GRBException& e) {
					std::cerr << __LINE__ << e.getMessage() << '\n';
				}

				//creating the summation
				//entrando em Source
				if (link.getX() == source) {
					sum1 += v2;
					sum2 += v1;
				} else { //saindo de X --- > Y
					sum1 += v1;
					sum2 += v2;
				}

			}
			//adding the constraint (expression, name)
			try {
				grbmodel.addConstr(sum1 - sum2 == -1, ss.str());
			}
			catch(const GRBException& e) {
				std::cerr << __LINE__ << e.getMessage() << '\n';
			}

		}
	}
	grbmodel.update ();

}	

void BaseModel::flow2 (GRBModel & grbmodel, 
	rca::Network& net, vgroup_t & groups) {

	size_t GROUPS = groups.size ();
	size_t NODES = net.getNumberNodes ();

	for (size_t k = 0; k < GROUPS; ++k)
	{
		
		size_t source = groups[k].getSource ();

		for (size_t d : groups[k].getMembers ()) {

			for (size_t j = 0; j < NODES; ++j)
			{
				GRBLinExpr sum1 = 0, sum2 = 0;
				std::stringstream ss;
				ss << "flow2(" << k+1 << "," << d+1 << "," << j+1 <<")";

				if (j != source and j != d) {

					for (rca::Link const& link : net.getEdges(j)) {

						//getting the name of variables
						const std::string var1 = 
							get_var_name (link.getX(),link.getY(), k, d);

						//getting the name of variables
						const std::string var2 = 
							get_var_name (link.getY(),link.getX(), k, d);

						//getting the variables
						GRBVar v1, v2;
						try {
							v1 = grbmodel.getVarByName (var1);
							v2 = grbmodel.getVarByName (var2);
						}
						catch(const GRBException& e) {
							std::cerr << __LINE__ << e.getMessage() << '\n';
						}

						//updateing the GRBLinExpr terms
						if ((size_t)link.getX() == j) {
							sum1 += v1;
							sum2 += v2;	
						} else {
							sum1 += v2;
							sum2 += v1;	
						}

					}

					//adding the constraint to model
					try {
						grbmodel.addConstr (sum1 - sum2 == 0, ss.str());
					}
					catch(const GRBException& e) {
						std::cerr << __LINE__ << e.getMessage() << '\n';
					}
					
					
				} else {
					try {
						grbmodel.addConstr (sum1 == 0, ss.str ());
					}
					catch(const GRBException& e) {
						std::cerr << __LINE__ << e.getMessage() << '\n';
					}
					
				}

			}

		}

	}

	grbmodel.update ();

}

void BaseModel::flow3 (GRBModel &grbmodel, 
	rca::Network& net,vgroup_t& groups)
{

	size_t GROUPS = groups.size ();

	for (size_t k = 0; k < GROUPS; ++k)
	{		
		for (int d : groups[k].getMembers())
		{
			
			GRBLinExpr sum1 = 0, sum2 = 0;
			std::stringstream ss;
			ss << "flow3(" << k+1 << "," << d+1 << ")";

			for (rca::Link const& link : net.getEdges(d)) {

				//getting the name of variables
				const std::string var1 = 
					get_var_name (link.getX(),link.getY(), k, d);

				//getting the name of variables
				const std::string var2 = 
					get_var_name (link.getY(),link.getX(), k, d);

				//getting the variables
				GRBVar v1, v2;
				try {
					v1 = grbmodel.getVarByName (var1);
					v2 = grbmodel.getVarByName (var2);
				}
				catch(const GRBException& e) {
					std::cerr << __LINE__ << e.getMessage() << '\n';
				}

				//updateing the GRBLinExpr terms
				if (link.getX() == d) {
					sum1 += v2;
					sum2 += v1;	
				} else {
					sum1 += v1;
					sum2 += v2;
				}
				

			}

			try {
				grbmodel.addConstr (sum1 - sum2 == 1, ss.str());
			}
			catch(const GRBException& e)  {
				std::cerr << __LINE__ << e.getMessage() << '\n';
			}
			

		}

	}

	grbmodel.update ();
}

void BaseModel::set_edge_as_used (GRBModel &grbmodel,
	rca::Network& net, vgroup_t& groups) {

	size_t GROUPS = groups.size ();
	for (size_t k = 0; k < GROUPS; ++k)
	{
		for (int d : groups[k].getMembers ())
		{
			
			for (rca::Link const& link : net.getLinks()) {

				int x = link.getX();
				int y = link.getY();
				std::string const var_x1 = get_var_name (x, y, k, d);
				std::string const var_y1 = get_y_var_name (x, y, k);
				std::stringstream ss1;	
				ss1 << "mark("<< x+1 << "," << y+1 <<",";
				ss1 << k+1 << "," << d+1 << ")";

				GRBVar x1, y1;
				try {
					x1 = grbmodel.getVarByName (var_x1);
					y1 = grbmodel.getVarByName (var_y1);

					grbmodel.addConstr ( x1 <= y1, ss1.str());
				}
				catch(const GRBException& e) {
					std::cerr << __LINE__ << e.getMessage() << '\n';
				}

				std::string const var_x2 = get_var_name (y,	x, k, d);
				std::string const var_y2 = get_y_var_name (y, x, k);
				std::stringstream ss2;	
				ss2 << "mark("<< y+1 << "," << x+1 <<",";
				ss2 << k+1 << "," << d+1 << ")";

				GRBVar x2, y2;
				try {
					x2 = grbmodel.getVarByName (var_x2);
					y2 = grbmodel.getVarByName (var_y2);

					grbmodel.addConstr ( x2 <= y2, ss2.str());	
				}
				catch(const GRBException& e) {
					std::cerr << __LINE__ << e.getMessage() << '\n';
				}
				

				

				
			}

		}
	}

	grbmodel.update ();

}

void BaseModel::avoid_links_repeated (GRBModel & grbmodel, 
	rca::Network& net, vgroup_t& groups) {

	size_t GROUPS = groups.size ();

	for (int k = 0; k < GROUPS; k++) {

		std::vector<int> members = groups[k].getMembers ();
		for (auto d : members) {

			for (rca::Link const& link : net.getLinks ()) {

				int x = link.getX();
				int y = link.getY();

				std::string const& vname1 = get_var_name (x,y,k,d);
				std::string const& vname2 = get_var_name (y,x,k,d);

				std::stringstream ss;
				ss << "avoid_repeated_links(" << x+1 <<","<< y+1;
				ss <<  "," << k+1 << ")";

				GRBLinExpr sum = 0;
				GRBVar y1, y2;
				try {
					
					y1 = grbmodel.getVarByName (vname1);
					y2 = grbmodel.getVarByName (vname2);

				}
				catch(const GRBException& e) {
					std::cerr << __LINE__ << e.getMessage() << '\n';
				}

				sum = (y1 + y2);
				grbmodel.addConstr ( sum <= 1, ss.str ());
			}

		}
	}

}

void HopCostModel::capacity (GRBModel &grbmodel, 
	rca::Network& net, vgroup_t& groups, int Z) {

	size_t GROUPS = groups.size ();

	for (rca::Link const& link : net.getLinks ()) {

		int x = link.getX();
		int y = link.getY();

		GRBLinExpr sum = 0;
		std::stringstream ss;
		ss << "capacity(" << x+1 <<","<< y+1 << ")";

		for (size_t k = 0; k < GROUPS; ++k)
		{
			std::string const& vname1 = get_y_var_name (x,y,k);
			std::string const& vname2 = get_y_var_name (y,x,k);

			int tk = groups[k].getTrequest ();

			GRBVar y1, y2;
			try {
				y1 = grbmodel.getVarByName (vname1);
				y2 = grbmodel.getVarByName (vname2);
			}
			catch(const GRBException& e) {
				std::cerr << __LINE__ << e.getMessage() << '\n';
			}
			

			sum += (y1 + y2)*tk;
		}

		int capacity = net.getBand (x,y);
		
		// std::stringstream ss1;
		// ss1 << "capacity(" << y+1 <<","<< x+1 << ")";

		grbmodel.addConstr ( capacity - sum >= Z, ss.str ());
		// grbmodel.addConstr ( capacity - sum >= 0, ss1.str ());

	}

	grbmodel.update ();
}

void HopCostModel::avoid_leafs (GRBModel &grbmodel, 
	rca::Network& net, vgroup_t& groups) {

	size_t GROUPS = groups.size ();
	for (size_t k = 0; k < GROUPS; ++k)
	{
		for (rca::Link const& link : net.getLinks()) {

			int x = link.getX();
			int y = link.getY();

			std::stringstream ss1;
			ss1 << "avoid(" << x+1 << ",";
			ss1 << y+1 << "," << k+1 << ")";
			
			std::stringstream ss2;
			ss2 << "avoid(" << y+1 << ",";
			ss2 << x+1 << "," << k+1 << ")";

		
			GRBLinExpr sum1 = 0, sum2 = 0;

			for (int d : groups[k].getMembers ()) {

				std::string const& var_name1 = get_var_name (x,y,k,d);
				GRBVar var1 = grbmodel.getVarByName (var_name1);
				sum1 += var1;

				std::string const& var_name2 = get_var_name (y,x,k,d);
				GRBVar var2 = grbmodel.getVarByName (var_name2);
				sum2 +=var2;

			}

			std::string const& var_y1 = get_y_var_name (x, y, k);
			std::string const& var_y2 = get_y_var_name (y, x, k);

			GRBVar y1 = grbmodel.getVarByName (var_y1);
			GRBVar y2 = grbmodel.getVarByName (var_y2);

			grbmodel.addConstr (sum1 - y1 >= 0, ss1.str());
			grbmodel.addConstr (sum2 - y2 >= 0, ss2.str());

		}
	}

	grbmodel.update ();
}

void HopCostModel::hop_limite (GRBModel& grbmodel, 
	rca::Network& net, vgroup_t& groups, int limite) {

	size_t GROUPS = groups.size ();

	for (size_t k = 0; k < GROUPS; ++k)
	{
		for (int d : groups[k].getMembers ()) {
			std::stringstream ss;
			ss << "hop(" << k+1 << "," << d+1 << ")";

			GRBLinExpr sum = 0;
			for (rca::Link const& link : net.getLinks ()) {
				std::string const& varname1 = 
					get_var_name (link.getX(), link.getY(),k,d);
				std::string const& varname2 = 
					get_var_name (link.getY(), link.getX(),k,d);

				GRBVar var1, var2;
				try {
					var1 = grbmodel.getVarByName (varname1);
					var2 = grbmodel.getVarByName (varname2);
				}
				catch(const GRBException& e) {
					std::cerr << __LINE__ << e.getMessage() << '\n';
				}
				
				sum += var1;
				sum += var2;
			}

			grbmodel.addConstr (sum <= limite, ss.str ());

		}
	}

	grbmodel.update ();
}

void HopCostModel::add_objective_function (GRBModel& grbmodel, 
	rca::Network& net, vgroup_t& groups) {

	GRBLinExpr sum = 0;
	size_t GROUPS = groups.size ();
	for (size_t k = 0; k < GROUPS; ++k)
	{

		GRBLinExpr part = 0;
		for (rca::Link const& l : net.getLinks ()) {
			
			int cost = net.getCost (l.getX(), l.getY()); 

			std::string const& var = 
				get_y_var_name (l.getX(), l.getY(), k);

			GRBVar y;
			try {
				y = grbmodel.getVarByName (var);
			}
			catch(const GRBException& e) {
				std::cerr << __LINE__ << e.getMessage() << '\n';
			}
			
			part += (y * cost);

			std::string const& var2 = 
				get_y_var_name (l.getY(), l.getX(), k);

			GRBVar y2;
			try {
				y2 = grbmodel.getVarByName (var2);
			}
			catch(const GRBException& e) {
				std::cerr << __LINE__ << e.getMessage() << '\n';
			}
			
			part += (y2 * cost);
		}

		sum += part;
	}

	try {
		grbmodel.setObjective (sum, GRB_MINIMIZE);
	}
	catch(const GRBException& e) {
		std::cerr << __LINE__ << e.getMessage() << '\n';
	}
	

	grbmodel.update ();

}

void HopCostModel::set_residual_capacity (GRBModel& grbmodel, 
	rca::Network& net, vgroup_t& groups, int Z) {

	GRBConstr * array = grbmodel.getConstrs ();
	int length = grbmodel.get (GRB_IntAttr_NumConstrs);

	for (int i = 0; i < length; ++i)
	{
		std::string varname = 
			array[i].get (GRB_StringAttr_ConstrName);

		if (varname.find ("capacity") != std::string::npos) {
			try {
				double value = array[i].get (GRB_DoubleAttr_RHS);				
				array[i].set (GRB_DoubleAttr_RHS, value+Z);	
			}
			catch(const GRBException& e) {
				cout << __LINE__ << " : " <<  e.getMessage () << endl;
			}
			
		}

	}

	grbmodel.update ();

}

void LeeModel::set_tree_limits (GRBModel & grbmodel, 
	rca::Network& net,
	std::vector<double>& limits) {

	size_t GROUPS = limits.size ();

	for (size_t k = 0; k < GROUPS; ++k)
	{
		
		GRBLinExpr sum = 0;
		std::stringstream ss;
		ss << "opt(k=" << k + 1 << ")";
		for (rca::Link const& l : net.getLinks () ) {

			std::string const& varname1 =
				get_y_var_name (l.getX(), l.getY(), k);			
			GRBVar var1 = grbmodel.getVarByName (varname1);

			std::string const& varname2 =
				get_y_var_name (l.getY(), l.getX(), k);			
			GRBVar var2 = grbmodel.getVarByName (varname2);

			// int cost = net.getCost ( l.getX(), l.getY());
			
			// sum += ((var1 + var2)*cost);			
			sum += ((var1 + var2)*1);

		}

		grbmodel.addConstr (sum <= limits[k], ss.str ());
	}

	grbmodel.update ();

}

void ResidualModel::capacity (GRBModel & grbmodel, 
	rca::Network& net, vgroup_t& groups, int Z) {

	// AQUI O LIMITE É O VALOR DE Z
	size_t GROUPS = groups.size ();

	GRBVar var_z;
	try {
		 var_z = grbmodel.getVarByName ("Z");		 
	}
	catch(const GRBException& e) {
		cout << __LINE__ << " : " <<  e.getMessage () << endl;
	}

	for (rca::Link const& link : net.getLinks ()) {

		int x = link.getX();
		int y = link.getY();

		GRBLinExpr sum = 0;
		std::stringstream ss;
		ss << "capacity(" << x+1 <<","<< y+1 << ")";

		for (size_t k = 0; k < GROUPS; ++k)
		{
			std::string const& vname1 = get_y_var_name (x,y,k);
			std::string const& vname2 = get_y_var_name (y,x,k);

			int tk = groups[k].getTrequest ();

			GRBVar y1, y2;

			try {
				y1 = grbmodel.getVarByName (vname1);
				y2 = grbmodel.getVarByName (vname2);
			}
			catch(const GRBException& e) {
				cout << __LINE__ << " : " <<  e.getMessage () << endl;
			}
			

			sum += (y1 + y2)*tk;
		}

		int capacity = net.getBand (x,y);

		grbmodel.addConstr ( capacity - sum >= var_z, ss.str ());


	}

	grbmodel.update ();

}

void LeeModel::set_tree_limits (GRBModel & grbmodel, 
		double factor) {

	GRBConstr * array = grbmodel.getConstrs ();
	int length = grbmodel.get (GRB_IntAttr_NumConstrs);

	for (int i = 0; i < length; ++i)
	{
		std::string constr = 
			array[i].get (GRB_StringAttr_ConstrName);

		if (constr.find ("opt") != std::string::npos) {

			try {
				double value = array[i].get (GRB_DoubleAttr_RHS);							
				array[i].set (GRB_DoubleAttr_RHS, value*factor);		
			}
			catch(const GRBException& e) {
				std::cerr << __LINE__ << e.getMessage() << '\n';
			}

		}

	}
	grbmodel.update ();
}

void ResidualModel::add_objective_function (GRBModel &grbmodel) {

	double up = std::numeric_limits<double>::max();
	GRBVar Z = grbmodel.addVar (0,up,1, GRB_INTEGER, "Z");
	grbmodel.update ();

	GRBLinExpr sum;
	sum += Z;

	try {
		grbmodel.setObjective (sum, GRB_MAXIMIZE);
	}
	catch(const GRBException& e) {
		std::cerr << __LINE__ << e.getMessage() << '\n';
	}
	grbmodel.update ();	

}

void ConcreteResidualModel::avoid_leafs (GRBModel & grbmodel, 
	rca::Network& net, vgroup_t& groups) {

	size_t GROUPS = groups.size ();
	for (size_t k = 0; k < GROUPS; ++k)
	{
		for (rca::Link const& link : net.getLinks()) {

			int x = link.getX();
			int y = link.getY();

			std::stringstream ss1;
			ss1 << "avoid(" << x+1 << ",";
			ss1 << y+1 << "," << k+1 << ")";
			
			std::stringstream ss2;
			ss2 << "avoid(" << y+1 << ",";
			ss2 << x+1 << "," << k+1 << ")";

		
			GRBLinExpr sum1 = 0, sum2 = 0;

			for (int d : groups[k].getMembers ()) {

				std::string const& var_name1 = get_var_name (x,y,k,d);
				GRBVar var1 = grbmodel.getVarByName (var_name1);
				sum1 += var1;

				std::string const& var_name2 = get_var_name (y,x,k,d);
				GRBVar var2 = grbmodel.getVarByName (var_name2);
				sum2 +=var2;

			}

			std::string const& var_y1 = get_y_var_name (x, y, k);
			std::string const& var_y2 = get_y_var_name (y, x, k);

			GRBVar y1 = grbmodel.getVarByName (var_y1);
			GRBVar y2 = grbmodel.getVarByName (var_y2);

			grbmodel.addConstr (sum1 - y1 >= 0, ss1.str());
			grbmodel.addConstr (sum2 - y2 >= 0, ss2.str());

		}
	}

	grbmodel.update ();
}

void BZModel::cost_function (GRBModel &grbmodel,
	rca::Network& net, vgroup_t& groups, int z) {

	GRBLinExpr sum = 0;
	size_t GROUPS = groups.size ();
	for (size_t k = 0; k < GROUPS; ++k)
	{

		GRBLinExpr part = 0;
		for (rca::Link const& l : net.getLinks ()) {
			
			int cost = net.getCost (l.getX(), l.getY()); 

			std::string const& var = 
				get_y_var_name (l.getX(), l.getY(), k);

			GRBVar y;
			try {
				y = grbmodel.getVarByName (var);
			}
			catch(const GRBException& e) {
				std::cerr << __LINE__ << e.getMessage() << '\n';
			}
			
			part += (y * cost);

			std::string const& var2 = 
				get_y_var_name (l.getY(), l.getX(), k);

			GRBVar y2;
			try {
				y2 = grbmodel.getVarByName (var2);
			}
			catch(const GRBException& e) {
				std::cerr << __LINE__ << e.getMessage() << '\n';
			}
			
			part += (y2 * cost);
		}

		sum += part;
	}

	try {
		grbmodel.setObjective (sum, GRB_MINIMIZE);		

	}
	catch(const GRBException& e) {
		std::cerr << __LINE__ << e.getMessage() << '\n';
	}
	
	grbmodel.update ();

}

void BZModel::capacity (GRBModel &grbmodel, 
	rca::Network& net, vgroup_t& groups, int Z) {

	size_t GROUPS = groups.size ();

	for (rca::Link const& link : net.getLinks ()) {

		int x = link.getX();
		int y = link.getY();

		GRBLinExpr sum = 0;
		std::stringstream ss;
		ss << "capacity(" << x+1 <<","<< y+1 << ")";

		for (size_t k = 0; k < GROUPS; ++k)
		{
			std::string const& vname1 = get_y_var_name (x,y,k);
			std::string const& vname2 = get_y_var_name (y,x,k);

			int tk = groups[k].getTrequest ();

			GRBVar y1, y2;
			try {
				y1 = grbmodel.getVarByName (vname1);
				y2 = grbmodel.getVarByName (vname2);
			}
			catch(const GRBException& e) {
				std::cerr << __LINE__ << e.getMessage() << '\n';
			}
			

			sum += (y1 + y2)*tk;
		}

		int capacity = net.getBand (x,y);
		
		// std::stringstream ss1;
		// ss1 << "capacity(" << y+1 <<","<< x+1 << ")";

		grbmodel.addConstr ( capacity - sum >= Z, ss.str ());
		// grbmodel.addConstr ( capacity - sum >= 0, ss1.str ());

	}

	grbmodel.update ();
}

void LeeModifiedModel::set_tree_limits (GRBModel & grbmodel, 
	rca::Network& net,
	std::vector<double>& limits) {

	size_t GROUPS = limits.size ();

	for (size_t k = 0; k < GROUPS; ++k)
	{
		
		GRBLinExpr sum = 0;
		std::stringstream ss;
		ss << "opt(k=" << k + 1 << ")";
		for (rca::Link const& l : net.getLinks () ) {

			std::string const& varname1 =
				get_y_var_name (l.getX(), l.getY(), k);			
			GRBVar var1 = grbmodel.getVarByName (varname1);

			std::string const& varname2 =
				get_y_var_name (l.getY(), l.getX(), k);			
			GRBVar var2 = grbmodel.getVarByName (varname2);
			
			int cost = net.getCost ( l.getX(), l.getY());
			
			sum += ((var1 + var2)*cost);	

			// sum += ((var1 + var2)*1);			

		}
		grbmodel.addConstr (sum <= limits[k], ss.str ());
	}

	grbmodel.update ();

}

void BudgetModel::budget (GRBModel& grbmodel, 
	rca::Network& net, vgroup_t& groups, int budget) {

	GRBLinExpr sum = 0;
	size_t GROUPS = groups.size ();
	for (size_t k = 0; k < GROUPS; ++k)
	{

		GRBLinExpr part = 0;
		for (rca::Link const& l : net.getLinks ()) {
			
			int cost = net.getCost (l.getX(), l.getY()); 

			std::string const& var = 
				get_y_var_name (l.getX(), l.getY(), k);

			GRBVar y = grbmodel.getVarByName (var);
			part += (y * cost);

			std::string const& var2 = 
				get_y_var_name (l.getY(), l.getX(), k);

			GRBVar y2 = grbmodel.getVarByName (var2);
			part += (y2 * cost);
		}

		sum += part;
	}

	grbmodel.addConstr (sum <= budget, "Budget");
	grbmodel.update ();
}

void SteinerTreeModel::create_variables (GRBModel& grbmodel, 
	rca::Network& net, rca::Group& group) {

	//creating x varibles
	for (rca::Link const& l : net.getLinks ()) {

		std::string varname = 
			this->get_y_name (l.getX(), l.getY());

		GRBVar v1 = grbmodel.addVar(0, 1, 1, GRB_BINARY, varname);
		varname = this->get_y_name (l.getY(), l.getX());
		GRBVar v2 =grbmodel.addVar(0, 1, 1, GRB_BINARY, varname);

		var_y.push_back (v1);
		var_y.push_back (v2);

	}

	for (int member : group.getMembers ())
	{
		for (rca::Link const& l : net.getLinks ()) {
			
			int x = l.getX();
			int y = l.getY();
			
			std::string varname = 
				this->get_x_name (x,y,member);		

			GRBVar v1 = grbmodel.addVar(0,1,1, GRB_BINARY, varname);
			varname = this->get_x_name (y,x, member);
			GRBVar v2 = grbmodel.addVar(0,1,1, GRB_BINARY, varname);

			var_x.push_back ( v1 );
			var_x.push_back ( v2 );
		}
		
	}

	grbmodel.update ();
}

void SteinerTreeModel::flow1 (GRBModel& grbmodel, 
	rca::Network& net, rca::Group& group) {

	int source = group.getSource ();
	for(auto&& member : group.getMembers ()) {		
		
		GRBLinExpr out =  this->get_sum_x (-1, source, member);
		GRBLinExpr in =  this->get_sum_x (source, -1, member);

		std::stringstream ss;
		ss << "flow1(" << source + 1 << "," << member + 1<< ")";
		grbmodel.addConstr (out - in == -1, ss.str ());

	}

	grbmodel.update ();

}

void SteinerTreeModel::flow2 (GRBModel& grbmodel, 
	rca::Network& net, rca::Group& group) {
	
	for(auto&& member : group.getMembers ()) {
		
		std::stringstream ss;
		ss << "flow2(" << member + 1 << ")";

		GRBLinExpr out =  this->get_sum_x (-1, member, member);
		GRBLinExpr in =  this->get_sum_x (member, -1, member);

		grbmodel.addConstr (out - in == 1, ss.str ());

	}

	grbmodel.update ();

}

void SteinerTreeModel::flow3 (GRBModel& grbmodel, 
	rca::Network& net, rca::Group& group) {
	
	int VERTEX = net.getNumberNodes ();
	int source = group.getSource ();

	for(auto&& member : group.getMembers()) {		
		for (int j = 0; j < VERTEX; ++j)
		{

			if (j == member || j == source) continue;

			std::stringstream ss;
			ss << "flow3(" << member + 1<< "," << j + 1;

			GRBLinExpr out =  this->get_sum_x (-1, j, member);
			GRBLinExpr in =  this->get_sum_x (j, -1, member);

			grbmodel.addConstr (out - in == 0, ss.str ());

		}

	}

	grbmodel.update ();

}

void SteinerTreeModel::mark (GRBModel& grbmodel, 
	rca::Network& net, rca::Group& group) {

	for(auto&& member : group.getMembers()) {		 
		for(auto&& l : net.getLinks ()) {
			
			int x = l.getX();
			int y = l.getY();

			std::stringstream ss1;
			ss1 << "mark(" << x+1 << "," << y+1 << "," << member+1 << ")";

			GRBVar varx = grbmodel.getVarByName (get_x_name (x,y,member));
			GRBVar vary = grbmodel.getVarByName (get_y_name (x,y));

			grbmodel.addConstr (varx <= vary, ss1.str());

			varx = grbmodel.getVarByName (get_x_name (y,x,member));
			vary = grbmodel.getVarByName (get_y_name (y,x));

			std::stringstream ss2;
			ss2 << "mark(" << y+1 << "," << x+1 << "," << member+1 << ")";
			grbmodel.addConstr (varx <= vary, ss2.str ());

		}

	}

	grbmodel.update ();

}

void SteinerTreeModel::set_objective_by_links (GRBModel& grbmodel, 
	rca::Network& net, rca::Group& group) {

	GRBLinExpr sum = 0;

	for (auto&& var : var_y) {
		sum += var;
	}

	grbmodel.setObjective (sum, GRB_MINIMIZE);
	grbmodel.update ();

}

void SteinerTreeModel::set_objective_by_cost (GRBModel& grbmodel, 
	rca::Network& net, rca::Group& group) {


	GRBLinExpr sum = 0;

	for(auto&& l : net.getLinks ()) {
		
		int x = l.getX(), y = l.getY();
		int cost = net.getCost (x, y);
		sum += (grbmodel.getVarByName (get_y_name (x, y) ) * cost);
		sum += (grbmodel.getVarByName (get_y_name (y, x) ) * cost);

	}

	grbmodel.setObjective (sum , GRB_MINIMIZE);
	grbmodel.update ();

}


std::string const get_var_name (int x, int y, int k, int d) {

	std::stringstream ss;
	ss<<"x(";
	ss << x+1 << ",";
	ss << y+1 << ",";
	ss << k+1 << "," << d+1 << ")";	

	return ss.str();

}

std::string const get_y_var_name (int x, int y, int k) {

	std::stringstream ss;
	ss<<"y(";
	ss << x+1 << ",";
	ss << y+1 << ",";
	ss << k+1 << ")";

	return ss.str();
}

vsolution_t solution_info (std::string file,
	GRBModel & grbmodel, int gsize) 
{

	vsolution_t solution = 	vsolution_t(gsize);

	GRBVar * array = grbmodel.getVars ();
	int numvar = grbmodel.get (GRB_IntAttr_NumVars);

	for (int k = 0; k < gsize; ++k)
	{
		
		for (int i = 0; i < numvar; ++i)
		{
			GRBVar var = array[i];

			if (var.get(GRB_StringAttr_VarName).find ("y") 
				!= std::string::npos) {

				if (var.get(GRB_DoubleAttr_X) == 1) {

					std::string varname = var.get(GRB_StringAttr_VarName);

					std::regex ptn ("\\d+");
					std::sregex_iterator rit ( varname.begin(), 
						varname.end(), ptn );

	  				std::sregex_iterator rend;

	  				int vertex1 = atoi ( rit->str ().c_str () ) - 1;
	  				rit++;
	  				int vertex2 = atoi (rit->str ().c_str ()) - 1;
	  				rit++;
	  				int tree = atoi (rit->str ().c_str ()) - 1;

	  				if (tree == k) {
	  					rca::Link l (vertex1, vertex2, 0);
	  					solution[k].push_back (l);
	  				}

				}

			}
		}

	}

	return solution;

}