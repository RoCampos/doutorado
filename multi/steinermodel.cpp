#include <iostream>

#include <boost/regex.hpp>
#include "gurobi_c++.h"

#include "models.h"
#include "network.h"
#include "group.h"
#include "reader.h"

int main(int argc, char const *argv[])
{
	
	std::string file = argv[1];

	rca::Network net;
	std::vector<rca::Group> multicast_group;
	std::vector<shared_ptr<rca::Group>> temp;	
	
	MultipleMulticastReader r (file);		
	r.configure_real_values (&net,temp);
	for (auto g : temp) {
		multicast_group.push_back (*g);
	}

	GRBEnv env = GRBEnv ();
	GRBModel model = GRBModel (env);

	SteinerTreeModel st (model, net, multicast_group[0], 
		SteinerMode::OPTIMIZE_BY_SIZE);


	// model.write ("teste.lp");

	model.optimize ();


	return 0;
}