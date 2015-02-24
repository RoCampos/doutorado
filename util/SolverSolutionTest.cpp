#include "SolverSolutionTest.h"

bool MMMSTPGurobiResult::do_test (std::string instance, std::string result, int objective)
{
	//reading instance
	Network * net = new Network;
	Reader r(instance);
	r.configNetwork (net);	
	std::vector<std::shared_ptr<rca::Group> > groups = r.readerGroup ();

	
	int obj = objective_test (net, groups, result);
	
	
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
		std::stringstream ss (line);
		
		//getting v
		ss >> v;
		
		std::string str;
		ss >> str;
		ss >> str;
				
		//getting the groups
		char& r = str[ str.size ()-2 ];
		char * s = &r;
		int g = atoi(s);
		
		//geting w
		str.erase (str.end() - 3, str.end());
		w = stoi (str);
		
		//std::cout << v-1 << "|" << w-1 << "|"<<g-1<<"--";
		
		matrix[ v-1 ][ w-1 ] -= groups[g-1]->getTrequest ();
		std::cout << matrix[ v-1 ][ w-1 ] << std::endl;
		
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


int main (int argv, char**argc)
{

	SolverSolution<MMMSTPGurobiResult> result;
	
	std::string instance = argc[1];
	std::string optimal = argc[2];
	
	result.test (instance, optimal, 0);

	return 0;
}
