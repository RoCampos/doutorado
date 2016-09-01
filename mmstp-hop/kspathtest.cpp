#include <iostream>

#include "rca.h"

// binário para teste de caminhos entre dois vértices.
// explora todos os caminhos possíveis

// também é possível entrar com um limite no tamnho do caminho

int main(int argc, char const *argv[])
{

	srand (time(NULL));
	
	std::string file = argv[1];

	int H = atoi(argv[2]);

	rca::Network network; 
	std::vector<rca::Group> mgroups;
	get_problem_informations (file, network, mgroups);

	KShortestPath ks (&network);

	ks.init (5,28);

	while (ks.hasNext ()) {
		rca::Path p = ks.next ();
		if (p.size () - 1 <= H)
			cout << p << endl;
	}
	
	return 0;
}