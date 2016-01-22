#ifndef MODELS_H
#define MODELS_H


#include <iostream>
#include <sstream>

#include "gurobi_c++.h"
#include "network.h"
#include "group.h"

typedef typename std::vector<rca::Group> vgroup_t;

std::string const get_var_name (int x, int y, int k, int d);

std::string const get_y_var_name (int x, int y, int k);

/**
* função que busca variável e a adiciona a uma expressão (Sumatório)
*
*/
void add_term_expr (int,int,int,int, GRBLinExpr&);


class BaseModel;
class HopCostModel;
class LeeModel;
class LeeModifiedModel;
class BudgetModel;


/**
* Modele matemática para otimização do custo
* da solução para o modelo biobjetivo
*
*/
class BaseModel
{

public:
	BaseModel() {}

	BaseModel(GRBModel & grbmodel, 
		rca::Network& net, 
		vgroup_t& groups, int Z = 0){

		create_variables (grbmodel, net, groups);

		flow1 (grbmodel,net, groups);
		flow2 (grbmodel,net, groups);
		flow3 (grbmodel,net, groups);

		set_edge_as_used (grbmodel, net, groups);			
				
	}
	
	virtual ~BaseModel() {}

private:

	virtual void flow1 (GRBModel &, rca::Network&, vgroup_t&) final;
	virtual void flow2 (GRBModel &, rca::Network&, vgroup_t&) final;
	virtual void flow3 (GRBModel &, rca::Network&,vgroup_t&) final;

	void create_variables (GRBModel &, rca::Network&, vgroup_t&);

	//x_ij^kd <= y_ij^k
	void set_edge_as_used (GRBModel &, rca::Network&, vgroup_t&);

	//sum(x_ij^kd) - y_ij^k >= 0 
	virtual void avoid_leafs (GRBModel &, rca::Network&, vgroup_t&) {}

	//b_ij - sum(y_ij^k) >= 0
	virtual void capacity (GRBModel &, rca::Network&, vgroup_t&, int Z = 0) {}

	
};

class HopCostModel : public BaseModel
{


public:
	HopCostModel(GRBModel & grbmodel, 
		rca::Network& net, 
		vgroup_t& groups, int hoplimit, int Z = 0) :	
		BaseModel(grbmodel, net, groups, Z){

		this->add_objective_function (grbmodel, net, groups);
		this->hop_limite (grbmodel, net, groups, hoplimit);

		this->capacity (grbmodel, net, groups, Z);

	}

	~HopCostModel() {}

	//utilizado para alterar o valor rhs da restrição de capacidade
	void set_residual_capacity (GRBModel&, rca::Network&, vgroup_t&, int);

private:

	void hop_limite (GRBModel &, rca::Network&, vgroup_t&, int);	
	void add_objective_function (GRBModel&, rca::Network&, vgroup_t&);
	void avoid_leafs (GRBModel &, rca::Network&, vgroup_t&);
	void capacity (GRBModel &, rca::Network&, vgroup_t&, int Z = 0);

	
};


class LeeModel : public BaseModel
{
public:

	LeeModel (){}

	LeeModel(GRBModel & grbmodel, rca::Network& net, 
		vgroup_t& groups, std::vector<double>& limits, int Z = 0) 
	: BaseModel (grbmodel, net, groups){

		this->set_tree_limits (grbmodel, net, limits);	
		this->add_objective_function (grbmodel);
		this->capacity (grbmodel, net, groups, Z);
	}

	~LeeModel() {}

	void set_tree_limits (GRBModel & grbmodel, 
		double);

	void capacity (GRBModel &, rca::Network&, vgroup_t&, int Z);

protected:

	virtual void set_tree_limits (GRBModel & grbmodel, 
		rca::Network &net,
		std::vector<double>& limits);

	void avoid_leafs (GRBModel & grbmodel, 
		rca::Network& net, vgroup_t& groups) {}
	

	void add_objective_function (GRBModel&);
};


class LeeModifiedModel : public LeeModel, BaseModel {

public:
	LeeModifiedModel(GRBModel & grbmodel, rca::Network& net, 
		vgroup_t& groups, std::vector<double>& limits, int Z = 0) 	
	: BaseModel (grbmodel, net, groups) {

		this->set_tree_limits (grbmodel, net, limits);
		this->add_objective_function (grbmodel);
		LeeModel::capacity (grbmodel, net, groups, Z);
	}

private:
	virtual void set_tree_limits (GRBModel & grbmodel, 
		rca::Network &net,
		std::vector<double>& limits) override;

};

#endif // MODELS_H