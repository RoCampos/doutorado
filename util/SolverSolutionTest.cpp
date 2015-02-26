#include "SolverSolutionTest.h"

bool MMMSTPGurobiResult::do_test (std::string instance, std::string result, int objective)
{
	//reading instance
	Network * net = new Network;
	Reader r(instance);
	r.configNetwork (net);	
	std::vector<std::shared_ptr<rca::Group> > groups = r.readerGroup ();

	
	int obj = objective_test (net, groups, result);
	bool teste = true;
	for (int i=0; i < (int)groups.size (); i++) {
		teste = teste && steiner_tree_test (net, groups[i].get(), result);
	}
	
	std::cout << "objective: "<< obj << " Teste:" << (teste ? "ok":"nok")<< " ";
	std::cout << "cost: " << cost (net,result) << std::endl;
	
	delete net;
	
	return 0;
}

int MMMSTPGurobiResult::objective_test (rca::Network * net, 
										 list_groups groups, 
										 std::string result)
{

	std::vector<std::vector<int>> matrix ( net->getNumberNodes () );
	for (unsigned int i =0; i < matrix.size (); i++) {
		matrix[i] = std::vector<int> ( net->getNumberNodes (), 100000);
	}
	
	for (int i = 0; i < net->getNumberNodes(); i++) {
		for (int j = 0; j < net->getNumberNodes(); j++) {
			
			matrix[i][j] = net->getBand (i,j);
			
		}
	}
	
	std::ifstream file (result);
	std::string line;
	while ( getline (file, line)){
	
		int v = -1;
		int w = -1;
		int g = -1;
		sscanf (line.c_str (), "%d - %d:%d;", &v, &w , &g);
		
		//updating the consuption
		matrix[ v-1 ][ w-1 ] -= groups[g-1]->getTrequest ();
		
	}
	
	int min = 100000;
	
	for (int i = 0; i < net->getNumberNodes(); i++) {
		for (int j = 0; j < net->getNumberNodes(); j++) {
			
			if (matrix[i][j] < min && matrix[i][j] != 0) {
				min = matrix[i][j];
			}
		}
	}
	
	//std::cout << min <<std::endl;	
	
	return min;
}

bool MMMSTPGurobiResult::steiner_tree_test (rca::Network * net, 
										 rca::Group * group, 
										 std::string result)
{

	int NODES = net->getNumberNodes ();
	std::vector<int> nodes = std::vector<int> (NODES,0);
	
	int GROUP_ID = group->getId();
	
	std::cout << "Checking Steiner Tree (" << GROUP_ID << "):\n";
	
	DisjointSet2 dset (NODES);
	
	std::ifstream file (result);
	std::string line;
	while ( getline (file, line)) 
	{
		
		int v = -1;
		int w = -1;
		int g = -1;
		sscanf (line.c_str (), "%d - %d:%d;", &v, &w , &g);
		if ( (g-1) == GROUP_ID) {
			//printf ("%d - %d:%d;\n", v-1, w-1 , g-1);
			nodes[v-1]++;
			nodes[w-1]++;
			
			if ( dset.find2 ( (v-1) ) != dset.find2( (w-1) ) ) {
				dset.simpleUnion ( (v-1), (w-1) );
			}
		}
	}
	int non_used_vertex = 0;
	
	//testing terminals number
	int count = 0;
	for (unsigned int j = 0; j < nodes.size (); j++) {
		
		//std::cout << j << "degree(" << nodes[j] <<")"<< std::endl;
		if ( nodes [j] > 0) {
			
			if (group->isMember ( (int)j ) || 
				group->getSource () == (int)j)
			{
				count++;
			}			
		} 
		
		if ( nodes [j] == 0) {
			non_used_vertex++;
		}
	}
	
	assert (count == (group->getSize () + 1) );
	std::cout << "\t - Terminals Test: " <<(count == (group->getSize() + 1))<< "\n";
	//std::cout << "\t"<<count << " --- " << group->getSize() + 1 << std::endl;
	
	bool flag = false;
	for (unsigned int j = 0; j < nodes.size (); j++) {
	
		if ( nodes [j] == 1) {
			
			if ( !(group->isMember ( (int)j ) ||
				 group->getSource () == (int)j) )
			{
				std::cout << j << "non-leaf with degree 1\n"; 
				flag = true;
			}
			
		}		
	}
	assert ( flag == false);
	std::cout << "\t - Non-leaf with degree test: " << (flag == false) << std::endl;
	
	//std::cout << dset.getSize () << std::endl;
	assert ( (dset.getSize () - non_used_vertex) == 1);
	
	return true;
}

double MMMSTPGurobiResult::cost (rca::Network * net,std::string result)
{
	double sol_cost = 0.0;
	
	std::ifstream file (result);
	std::string line;
	while ( getline (file, line))
	{
	
		int w = -1;
		int v = -1;
		int g = -1;
		sscanf (line.c_str (), "%d - %d:%d;", &v, &w, &g);
		
		sol_cost += net->getCost (v-1,w-1);
		
	}
	return sol_cost;
}

int main (int argv, char**argc)
{

	SolverSolution<MMMSTPGurobiResult> result;
	
	std::string instance = argc[1];
	std::string optimal = argc[2];
	
	result.test (instance, optimal, 0);

	return 0;
}
