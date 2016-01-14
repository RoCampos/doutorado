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


				grbmodel.addVar (0,1,1, GRB_BINARY,var1);
				grbmodel.addVar (0,1,1, GRB_BINARY,var2);

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
				GRBVar v1 = grbmodel.getVarByName (var1);
				GRBVar v2 = grbmodel.getVarByName (var2);

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
			grbmodel.addConstr(sum1 - sum2 == -1, ss.str());
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
						GRBVar v1 = grbmodel.getVarByName (var1);
						GRBVar v2 = grbmodel.getVarByName (var2);

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
					grbmodel.addConstr (sum1 - sum2 == 0, ss.str());
					
				} else {
					grbmodel.addConstr (sum1 == 0, ss.str ());
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
				GRBVar v1 = grbmodel.getVarByName (var1);
				GRBVar v2 = grbmodel.getVarByName (var2);

				//updateing the GRBLinExpr terms
				if (link.getX() == d) {
					sum1 += v2;
					sum2 += v1;	
				} else {
					sum1 += v1;
					sum2 += v2;
				}
				

			}

			grbmodel.addConstr (sum1 - sum2 == 1, ss.str());

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
				
				GRBVar x1 = grbmodel.getVarByName (var_x1);
				GRBVar y1 = grbmodel.getVarByName (var_y1);
				grbmodel.addConstr ( x1 <= y1, ss1.str());

				std::string const var_x2 = get_var_name (y,	x, k, d);
				std::string const var_y2 = get_y_var_name (y, x, k);
				std::stringstream ss2;	
				ss2 << "mark("<< y+1 << "," << x+1 <<",";
				ss2 << k+1 << "," << d+1 << ")";

				GRBVar x2 = grbmodel.getVarByName (var_x2);
				GRBVar y2 = grbmodel.getVarByName (var_y2);
				grbmodel.addConstr ( x2 <= y2, ss2.str());

				
			}

		}
	}

	grbmodel.update ();

}

void BaseModel::avoid_leafs (GRBModel &grbmodel, 
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

void BaseModel::capacity (GRBModel &grbmodel, 
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

			GRBVar y1 = grbmodel.getVarByName (vname1);
			GRBVar y2 = grbmodel.getVarByName (vname2);

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

void BaseModel::hop_limite (GRBModel& grbmodel, 
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

				GRBVar var1 = grbmodel.getVarByName (varname1);
				GRBVar var2 = grbmodel.getVarByName (varname2);

				sum += var1;
				sum += var2;
			}

			grbmodel.addConstr (sum <= limite, ss.str ());

		}
	}

	grbmodel.update ();
}

void BaseModel::r7 (GRBModel &grbmodel, 
	rca::Network& net, vgroup_t& groups) {

	size_t GROUPS = groups.size ();
	int NODES = net.getNumberNodes ();

	for (size_t k = 0; k < GROUPS; ++k)
	{

		for (int j = 0; j < NODES; ++j)
		{

			if (j == groups[k].getSource ()) continue;

			std::stringstream ss;
			ss << "r7(" << k+1 << ",j=" << j+1 << ")";

			GRBLinExpr sum = 0;
			for (rca::Link const& l : net.getEdges(j))
			{
				int x = l.getX();
				int y = l.getY();
				if (x == j) {
					int aux = y;
					y = x;
					x = aux;					
				}

				std::string const& varname = get_y_var_name (x,y,k);				
				GRBVar var = grbmodel.getVarByName (varname);
				sum += var;

			}

			grbmodel.addConstr (sum <= 1, ss.str ());
 
		}

	}
	grbmodel.update ();

}

void CostModel::add_objective_function (GRBModel& grbmodel, 
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

			GRBVar y = grbmodel.getVarByName (var);
			part += (y * cost);

			std::string const& var2 = 
				get_y_var_name (l.getY(), l.getX(), k);

			GRBVar y2 = grbmodel.getVarByName (var2);
			part += (y2 * cost);
		}

		sum += part;
	}

	grbmodel.setObjective (sum, GRB_MINIMIZE);

	grbmodel.update ();

}

void CostModel::set_residual_capacity (GRBModel& grbmodel, 
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
				cout << e.getMessage () << endl;
			}
			
		}

	}

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