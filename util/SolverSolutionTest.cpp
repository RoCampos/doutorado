#include "SolverSolutionTest.h"

bool MMMSTPGurobiResult::do_test (std::string instance, std::string result, int objective)
{
	//reading instance
	Network * net = new Network;
	Reader r(instance);
	r.configNetwork (net);	
	std::vector<std::shared_ptr<rca::Group> > groups = r.readerGroup ();

	
	int obj = objective_test (net, groups, result);
	bool tree = steiner_tree_test (net, groups, result);
	std::cout << obj << " " << (tree ? "true": "false") << "\n";
	
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
										 list_groups groups, 
										 std::string result)
{

	std::vector<int> nodes = std::vector<int> (net->getNumberNodes (), 0);
	
	for (int i=0; i < groups.size (); i++) {
		
		//std::cout << *groups[i].get() << std::endl;
		
		std::cout << "\tChecking steiner tree( " << i << ")\n";
		
		//verify if leafs are terminals
		std::ifstream file (result);
		std::string line;
		while ( getline (file, line) )
		{	
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
					
			if ((g - 1) == i){
				nodes[w-1]++;
				nodes[v-1]++;
			} else {
				break;
			}
			
		}
		
		//checking terminals
		int count = 0;
		for (unsigned int j=0; j < nodes.size (); j++) {
			
			if (nodes[j] >= 0 && 
				(groups[i]->isMember( j ) || j == groups[i]->getSource()) )
			{
				count++;
			}			
		}
		//checking terminals
		assert (count == (groups[i]->size () + 1) );
		std::cout << "\tterminals number: "<< count << "\n";
		
		bool flag = true;
		for (unsigned int j=0; j < nodes.size (); j++) {
		
			if (nodes[j] == 1 && !(groups[i]->isMember ( j )) )
			{
				flag = false;
				break;
			}			
		}
		assert ( flag );
		std::cout << "\tthere is no non-terminals as leaf\n";
	
	}
	//count the number of terminals 
	//verify connectivty
	
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
