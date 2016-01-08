#ifndef MODELS_H
#define MODELS_H


#include <iostream>
#include <sstream>

#include "gurobi_c++.h"
#include "network.h"
#include "group.h"

typedef typename std::vector<rca::Group> vgroup_t;

class BaseModel;
class CostModel;
class CapacityModel;


/**
* Modele matemática para otimização do custo
* da solução para o modelo biobjetivo
*
*/
class BaseModel
{

public:
	BaseModel(GRBModel & grbmodel, rca::Network& net, vgroup_t& groups){

		create_variables (grbmodel, net, groups);

		flow1 (grbmodel,net, groups);

		flow2 (grbmodel,net, groups);
		flow3 (grbmodel,net, groups);

		set_edge_as_used (grbmodel);
		capacity (grbmodel);
		avoid_leafs (grbmodel);		
	}
	
	~BaseModel() {}

	void create_variables (GRBModel &, rca::Network&, vgroup_t&);

private:

	void flow1 (GRBModel &, rca::Network&, vgroup_t&);
	void flow2 (GRBModel &, rca::Network&, vgroup_t&);
	void flow3 (GRBModel &, rca::Network&,vgroup_t&);

	//x_ij^kd <= y_ij^k
	void set_edge_as_used (GRBModel &) {};

	//b_ij <= sum(y_ij^k)
	void capacity (GRBModel &) {};

	//sum(x_ij^kd) - y_ij^k >= 0 
	void avoid_leafs (GRBModel &) {};


	void hop_limite (GRBModel &){};

	
};

class CostModel : public BaseModel
{


public:
	CostModel(GRBModel & grbmodel, rca::Network& net, vgroup_t& groups) :
		BaseModel(grbmodel, net, groups){

	}

	~CostModel();

private:
	void  t();
	
};

std::string const get_var_name (int x, int y, int k, int d);

#endif // MODELS_H