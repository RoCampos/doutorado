#ifndef _SOLVER_SOLUTION_TEST_
#define _SOLVER_SOLUTION_TEST_

#include <iostream>
#include <string>
#include <cassert>
#include <memory>
#include <sstream>

#include "network.h"
#include "reader.h"
#include "group.h"

template <typename Solver>
class SolverSolution : private Solver
{
	
	using Solver::do_test;
	
public:
	
	bool test (std::string instance, std::string result, int objective) {		
		return do_test (instance, result,  objective);
	}
	
};

typedef std::vector<std::shared_ptr<rca::Group> >  list_groups;
class MMMSTPGurobiResult {

public:
	bool do_test (std::string instance, std::string result, int);
	
private:
	int objective_test (rca::Network *,list_groups, std::string result);
	bool steiner_tree_test (rca::Network *,rca::Group *g, std::string result);
	

};

#endif