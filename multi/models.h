#ifndef MODELS_H
#define MODELS_H


#include <iostream>
#include <sstream>

#include "gurobi_c++.h"
#include "network.h"
#include "group.h"

typedef typename std::vector<rca::Group> vgroup_t;

class BaseModel;
class HopCostModel;
class LeeModel;


/**
* Modele matemática para otimização do custo
* da solução para o modelo biobjetivo
*
*/
class BaseModel
{

public:
	BaseModel(GRBModel & grbmodel, 
		rca::Network& net, 
		vgroup_t& groups, int limite, int Z = 0){

		create_variables (grbmodel, net, groups);

		flow1 (grbmodel,net, groups);
		flow2 (grbmodel,net, groups);
		flow3 (grbmodel,net, groups);

		set_edge_as_used (grbmodel, net, groups);
		avoid_leafs (grbmodel, net, groups);		
		capacity (grbmodel, net, groups, Z);
				
	}
	
	~BaseModel() {}

protected:
	virtual void hop_limite (GRBModel &grbmodel, 
		rca::Network& net, vgroup_t& groups, int limite) {}

private:

	void flow1 (GRBModel &, rca::Network&, vgroup_t&);
	void flow2 (GRBModel &, rca::Network&, vgroup_t&);
	void flow3 (GRBModel &, rca::Network&,vgroup_t&);

	void create_variables (GRBModel &, rca::Network&, vgroup_t&);

	//x_ij^kd <= y_ij^k
	void set_edge_as_used (GRBModel &, rca::Network&, vgroup_t&);

	//sum(x_ij^kd) - y_ij^k >= 0 
	virtual void avoid_leafs (GRBModel &, rca::Network&, vgroup_t&);

	//b_ij - sum(y_ij^k) >= 0
	void capacity (GRBModel &, rca::Network&, vgroup_t&, int Z = 0);

	
};

class HopCostModel : public BaseModel
{


public:
	HopCostModel(GRBModel & grbmodel, 
		rca::Network& net, 
		vgroup_t& groups, int hoplimit, int Z = 0) :	
		BaseModel(grbmodel, net, groups, hoplimit, Z){

		this->add_objective_function (grbmodel, net, groups);
		this->hop_limite (grbmodel, net, groups, hoplimit);

	}

	~HopCostModel() {}

	//utilizado para alterar o valor rhs da restrição de capacidade
	void set_residual_capacity (GRBModel&, rca::Network&, vgroup_t&, int);

private:

	void hop_limite (GRBModel &, rca::Network&, vgroup_t&, int);	
	void add_objective_function (GRBModel&, rca::Network&, vgroup_t&);

	
};

std::string const get_var_name (int x, int y, int k, int d);

std::string const get_y_var_name (int x, int y, int k);

/**
* função que busca variável e a adiciona a uma expressão (Sumatório)
*
*/
void add_term_expr (int,int,int,int, GRBLinExpr&);

#endif // MODELS_H