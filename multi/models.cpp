#include "models.h"

using std::cout;
using std::endl;

void BaseModel::create_variables(GRBModel & grbmodel, 
	rca::Network& net, vgroup_t & groups) {

	size_t NODES = net.getNumberNodes ();
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
				sum1 += v1;
				sum2 += v2;

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
						sum1 += v1;
						sum2 += v2;

					}

				}

				//adding the constraint to model
				grbmodel.addConstr (sum1 - sum2 == 1, ss.str());

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
				sum1 += v1;
				sum2 += v2;

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
				std::string const var_x1 = get_var_name (x,y, k, d);
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
			ss1 << y << "," << k+1 << ")";
			
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
	rca::Network& net, vgroup_t& groups) {

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

		grbmodel.addConstr ( capacity - sum >= 0, ss.str ());

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