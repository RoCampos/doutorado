#include "SolverSolutionTest.h"

bool MMMSTPGurobiResult::do_test (std::string instance, std::string result, int objective)
{
	//reading instance
	Network * net = new Network;
	Reader r(instance);
	r.configNetwork (net);	
	std::vector<std::shared_ptr<rca::Group> > groups = r.readerGroup ();

	
	int obj = objective_test (net, groups, result);
	
	for (int i=0; i < (int)groups.size (); i++) {
		steiner_tree_test (net, groups[i].get(), result);
	}
	
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
	
	std::ifstream file (result);
	std::string line;
	while ( getline (file, line)) 
	{
		
		int v = -1;
		int w = -1;
		int g = -1;
		sscanf (line.c_str (), "%d - %d:%d;", &v, &w , &g);
		if (g == GROUP_ID) {
			nodes[v-1]++;
			nodes[w-1]++;
		}
	}
	
	//testing terminals number
	int count = 0;
	for (unsigned int j = 0; j < nodes.size (); j++) {
		
		if ( nodes [j] > 0) {
			
			if (group->isMember ( (int)j ) && 
				group->getSource () == (int)j)
			{
				count++;
			}			
		}
	}
	
	assert (count == (group->getSize () + 1) );
	std::cout << "\t Terminals Test: " <<(count == (group->getSize() + 1))<< "\n";
	
	return true;
}

int main (int argv, char**argc)
{

	SolverSolution<MMMSTPGurobiResult> result;
	
	std::string instance = argc[1];
	std::string optimal = argc[2];
	
	result.test (instance, optimal, 0);

	return 0;
}
