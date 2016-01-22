#include <iostream>

#include "models.h"
#include "gurobi_c++.h"
#include "network.h"
#include "reader.h"


void cost_model (GRBModel & grbmodel, 
	rca::Network& net, vgroup_t& group);

void lee_model ();
void lee_modified_model ();
void capacity_model ();

int main(int argc, char const *argv[])
{
	
	rca::Network net;
	std::vector<rca::Group> multicast_group;

	std::string file = argv[1];
	
	std::vector<shared_ptr<rca::Group>> temp;	
	MultipleMulticastReader r (file);		
	r.configure_real_values (&net,temp);
	for (auto g : temp) {
		multicast_group.push_back (*g);
	}

	return 0;
}