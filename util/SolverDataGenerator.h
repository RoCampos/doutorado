#ifndef _SOLVER_DATA_GENERATOR_H_
#define _SOLVER_DATA_GENERATOR_H_

#include <iostream>
#include <ostream>
#include <sstream>
#include <memory>

#include "network.h"
#include "group.h"

template <typename Model>
class DataGenerator : private Model
{
	using Model::generate;
	
public:
	
	void run (rca::Network * network, rca::Group *g) {		
		generate (network, g);
		
	}
	
	void run (rca::Network * network, std::vector<std::shared_ptr<rca::Group>>& g) {
		generate (network, g);
	}
	
	
};


class MultiCommidityFormulation 
{
	
public:
	
	void generate (rca::Network * network, rca::Group *g) ;
	
};

class MultipleMulticastCommodityFormulation
{

public:
	void generate (rca::Network * network, 
					std::vector<std::shared_ptr<rca::Group>>&);
	
};


/**
 * Esta classe é utilizada para criar instâncias para serem utilizas
 * no gurobi.
 * 
 * O modelo matemático utilizado é o mmstp.
 * 
 * O arquivo gerado é o farmato LP.
 * 
 */
class MultipleMulticastCommodityLP 
{
	
public:
	virtual void generate (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
protected:
	
	void constraint1 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	void constraint2 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	void constraint3 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	void constraint4 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	void constraint5 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	void constraint6 ();
	
	void constraint7 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	void constraint8 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	void bounds (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
};

class MMSTPBudgetLP : protected MultipleMulticastCommodityLP
{

public:
	virtual void generate (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
private:
	
	void constraint6 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	int m_budget;
	
};

#endif