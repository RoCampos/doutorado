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
//class BZModel; //otimiza budget sujeito a capacidade residual

class SteinerTreeModel;

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

class BZModel : public BaseModel {

public:
	BZModel (GRBModel & grbmodel, 
		rca::Network& net, vgroup_t& groups, int Z) : 
			BaseModel(grbmodel, net, groups, Z) 
		{

			this->cost_function (grbmodel, net, groups, Z);
			this->capacity (grbmodel, net, groups, Z);
			

		}

	virtual ~BZModel () {

	}

private:
	virtual void cost_function (GRBModel& final, 
		rca::Network& net, 
		vgroup_t& groups, int);

	void capacity (GRBModel &, rca::Network&, vgroup_t&, int Z = 0);

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

// ----------------------------- Steiner Tree Definitions

enum SteinerMode{

	OPTIMIZE_BY_SIZE = 0,
	OPTIMIZE_BY_COST = 1

};

class SteinerTreeModel {

public:
	SteinerTreeModel (GRBModel& grbmodel, 
		rca::Network& net, rca::Group& group, int type) {

		create_variables (grbmodel, net, group);
		flow1 (grbmodel, net, group);
		flow2 (grbmodel, net, group);
		flow3 (grbmodel, net, group);
		mark (grbmodel, net, group);

		if (type == SteinerMode::OPTIMIZE_BY_COST) {
			this->set_objective_by_cost (grbmodel, net, group);
		} else if (type == SteinerMode::OPTIMIZE_BY_SIZE) {
			this->set_objective_by_links (grbmodel, net, group);
		}

	}


protected:

	void create_variables (GRBModel&, rca::Network&, rca::Group&);
	
	void flow1 (GRBModel&, rca::Network&, rca::Group&);	

	void flow2 (GRBModel&, rca::Network&, rca::Group&);
	
	void flow3 (GRBModel&, rca::Network&, rca::Group&);	
	
	void mark (GRBModel&, rca::Network&, rca::Group&);

	void set_objective_by_links (GRBModel&, rca::Network&, rca::Group&);
	
	void set_objective_by_cost (GRBModel&, rca::Network&, rca::Group&);

private:

	std::string get_y_name (int x, int y) const {
		std::stringstream ss;
		ss << "y(" << x+1 << "," << y+1 << ")";
		return ss.str ();
	}

	std::string get_x_name (int x, int y, int k) const {
		std::stringstream ss;
		ss << "x(" << x+1 << "," << y+1 << "," << k+1 << ")";
		return ss.str ();
	}

	GRBLinExpr get_sum_x (int x, int y, int k) {
		GRBLinExpr expr = 0;

		std::stringstream ss;		
		ss << "\\(";
		ss << (x != -1 ? std::to_string (x+1) : std::string("(\\d+)")) << ",";		
		ss << (y != -1 ? std::to_string (y+1) : "(\\d+)") << ",";
		ss << (k != -1 ? std::to_string (k+1) : "(\\d+)") << "\\)";
		
		boost::regex ptn(ss.str ());
		boost::smatch what;

		for(auto&& var : var_x) {				
			std::string const& str = var.get(GRB_StringAttr_VarName);
			if ( boost::regex_search (str, what, ptn) ) {												
				expr += var;
			}

		}

		return expr;
	}

private:

	std::vector<GRBVar> var_y;
	std::vector<GRBVar> var_x;

};

#endif // MODELS_H