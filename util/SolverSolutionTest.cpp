#include "SolverSolutionTest.h"

bool MMMSTPGurobiResult::do_test (std::string instance, std::string result, int verb)
{
	m_verbose = verb;
	
	//reading instance
	Network * net = new Network;
	Reader r(instance);
	r.configNetwork (net);	
	std::vector<std::shared_ptr<rca::Group> > groups = r.readerGroup ();

	
	int obj = objective_test (net, groups, result);
	int cost2 = 0;
	for (int i=0; i < (int)groups.size (); i++) {
		cost2 += steiner_tree_test (net, groups[i].get(), result);
	}
	
	std::cout << result << " ";
	std::cout << obj << " " << cost(net, result) << " "<< cost2 << std::endl;
	
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

int MMMSTPGurobiResult::steiner_tree_test (rca::Network * net, 
										 rca::Group * group, 
										 std::string result)
{

	int NODES = net->getNumberNodes ();
	std::vector<int> nodes = std::vector<int> (NODES,0);
	
	int GROUP_ID = group->getId();
	
	if (m_verbose)
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
	
	//testing terminals number
	int count = count_terminals (nodes, group);
	assert (count == (group->getSize () + 1) );
	if (m_verbose){
	 std::cout << "\t - Terminals Test: " <<(count == (group->getSize() + 1))<< "\n";
	 std::cout << "\t"<<count << " --- " << group->getSize() + 1 << std::endl;
	}
	
	//making test of leaf no-terminal
	bool flag = non_terminal_leaf_test(nodes, group);
	assert ( flag == true );
	
	if (m_verbose)
		std::cout << "\t - Non-leaf with degree test: " << (flag) << std::endl;
	
	
	int connec = connectivity(group, dset, NODES);
	assert ( connec );
	
	if (m_verbose) {
		std::cout << "\tConnectivity test: ";
		//std::cout << ((dset.getSize () - non_used_vertex)) << std::endl;
		std::cout << connec  << std::endl;
	}
	
	return tree_cost (net, group, dset, result);
}

int MMMSTPGurobiResult::count_terminals (std::vector<int>&nodes, 
										 rca::Group *group)
{
	int count = 0;
	for (unsigned int j = 0; j < nodes.size (); j++) {
		
		//se o nó é utilizado,
		if ( nodes [j] > 0) {

			//e for um terminal (receptor ou emissor)
			if (group->isMember ( (int)j ) || 
				group->getSource () == (int)j)
			{
				//contar
				count++;
			}
		} 		
	}
	return count;
}

bool MMMSTPGurobiResult::non_terminal_leaf_test (std::vector<int> & nodes, 
												 rca::Group *group)
{	
	for (unsigned int j = 0; j < nodes.size (); j++) {
	
		if ( nodes [j] == 1) {
			
			if ( !(group->isMember ( (int)j ) ||
				 group->getSource () == (int)j) )
			{ 
				return false;
			}
		}		
	}	
	return true;
}

bool MMMSTPGurobiResult::connectivity (rca::Group *g, 
									   DisjointSet2& dset, 
									   int numberNodes)
{
	std::vector<int> members = g->getMembers ();
	std::sort (members.begin(), members.end());	
	int root = dset.find2 (members[0])->item;
	for (int i=1; i < (int)members.size (); i++){
		if (root != dset.find2(members[i])->item ) {
			return (0);
		}
	}
		
	return (1);
}

int MMMSTPGurobiResult::tree_cost (rca::Network *net, rca::Group *group, 
				  DisjointSet2& dset, std::string result)
{
	int tree_cost = 0.0;
	
	std::vector<int> members = group->getMembers ();
	std::sort (members.begin (), members.end());
	
	int root = dset.find2 (members[0])->item;
	int GROUP_ID = group->getId ();
	
	std::ifstream file (result);
	std::string line;
	while ( getline (file, line)) 
	{
		
		int v = -1;
		int w = -1;
		int g = -1;
		sscanf (line.c_str (), "%d - %d:%d;", &v, &w , &g);
		if (GROUP_ID == (g-1)) {			
			if (dset.find2 (v-1)->item == dset.find2(w-1)->item) {
				if (dset.find2 (v-1)->item == root){
					tree_cost += net->getCost (v-1, w-1);
				}
			}
		}
	}
	
	return tree_cost;
}

int MMMSTPGurobiResult::cost (rca::Network * net,std::string result)
{
	int sol_cost = 0.0;
	
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
	
	int m_verbose = atoi(argc[3]);
	
	result.test (instance, optimal, m_verbose);

	return 0;
}
