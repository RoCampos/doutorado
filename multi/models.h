#ifndef MODELS_H
#define MODELS_H


#include <iostream>
#include <sstream>

#include <boost/regex.hpp>

#include "gurobi_c++.h"
#include "network.h"
#include "group.h"

typedef typename std::vector<rca::Group> vgroup_t;
typedef typename std::vector<std::vector<rca::Link>> vsolution_t;

std::string const get_var_name (int x, int y, int k, int d);

std::string const get_y_var_name (int x, int y, int k);

/**
* função que busca variável e a adiciona a uma expressão (Sumatório)
*
*/
void add_term_expr (int,int,int,int, GRBLinExpr&);

vsolution_t solution_info (std::string file,
	GRBModel & grbmodel, int gsize);


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

class ResidualModel {

protected:
	virtual void capacity (GRBModel &, rca::Network&, vgroup_t&, int Z = 0);	
	virtual void add_objective_function (GRBModel&) final;

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


class LeeModel : public BaseModel, ResidualModel
{
public:

	LeeModel (GRBModel & grbmodel, rca::Network& net, 
		vgroup_t& groups)
	: BaseModel (grbmodel, net, groups)
	{
		ResidualModel::add_objective_function (grbmodel);
		ResidualModel::capacity (grbmodel, net, groups, 0);
	}

	LeeModel(GRBModel & grbmodel, rca::Network& net, 
		vgroup_t& groups, std::vector<double>& limits) 
	: BaseModel (grbmodel, net, groups){

		this->set_tree_limits (grbmodel, net, limits);	
		ResidualModel::add_objective_function (grbmodel);
		ResidualModel::capacity (grbmodel, net, groups, 0);
	}

	~LeeModel() {}

	void set_tree_limits (GRBModel & grbmodel, 
		double);

protected:

	virtual void set_tree_limits (GRBModel & grbmodel, 
		rca::Network &net,
		std::vector<double>& limits);

	void avoid_leafs (GRBModel & grbmodel, 
		rca::Network& net, vgroup_t& groups) {}
	
};


class LeeModifiedModel : public LeeModel {

public:
	LeeModifiedModel(GRBModel & grbmodel, rca::Network& net, 
		vgroup_t& groups, std::vector<double>& limits) 	
	: LeeModel (grbmodel, net, groups) {

		this->set_tree_limits (grbmodel, net, limits);		
	}

private:
	virtual void set_tree_limits (GRBModel & grbmodel, 
		rca::Network &net,
		std::vector<double>& limits) override;

};


class BudgetModel : public BaseModel, ResidualModel
{
public:
	BudgetModel(GRBModel & grbmodel, rca::Network& net, 
		vgroup_t& groups, int budget) 
	: BaseModel (grbmodel, net, groups)
	{

		ResidualModel::add_objective_function (grbmodel);				
		ResidualModel::capacity (grbmodel, net, groups, 0);

		this->budget (grbmodel, net, groups, budget);
	}

	virtual ~BudgetModel() {}

protected:	
	virtual void budget (GRBModel&, rca::Network&, vgroup_t&, int budget);
	
};

#endif // MODELS_H