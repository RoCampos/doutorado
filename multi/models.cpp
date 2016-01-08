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

			std::stringstream ss;
			ss << "y(";
			ss << link.getX()+1 << ",";
			ss << link.getY()+1 << ",";
			ss << k+1 << ")";
			grbmodel.addVar (0,1,1, GRB_BINARY,ss.str());

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

std::string const get_var_name (int x, int y, int k, int d) {

	std::stringstream ss;
	ss<<"x(";
	ss << x+1 << ",";
	ss << y+1 << ",";
	ss << k+1 << "," << d+1 << ")";	

	return ss.str();

}