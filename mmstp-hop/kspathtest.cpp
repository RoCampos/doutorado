#include <iostream>

#include "network.h"
#include "group.h"
#include "reader.h"
#include "kspath.h"
#include "path.h"

// binário para teste de caminhos entre dois vértices.
// explora todos os caminhos possíveis

// também é possível entrar com um limite no tamnho do caminho

using namespace rca::reader;

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
		if (p.size () - 1 <= H) {
			cout << p << ": ";
			cout << p.getPosition (28) << " ";
			cout << p.getRevPosition (28) << endl;
		}
	}

	rca::Path path;
	path.push (23);
	path.push (1);
	path.push (5);
	

	rca::Path path2;
	path2.push (22);
	path2.push (12);
	path2.push (7);
	path2.push (1);


	std::vector<int> v;

	// path;

	path2.subpath (12, v);
	path2.reverse ();


	rca::Path pp (v);
	cout << path2 << endl;
	cout << pp << endl;
	

	
	return 0;
}