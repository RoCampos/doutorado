#include <iostream>

#include "transgenetic.h"
#include "include/steinerReader.h"

#include "time.h"

/*
* argumentos:
* argv[1] instância
* argv[2] info_a_priori
* argv[3] pop_size
* argv[4] generation
*/
int main (int argc, char** argv) {

	if (argc < 5) {
		cout << argc << endl;
		cout << "*** argumentos: \n* argv[1] instância\n* argv[2] info_a_priori\n* argv[3] pop_size\n";
		cout << "* argv[4] generation\n";
		exit (1);
	}

	srand (time(NULL));
	
	/*Reading informations of instance*/
	std::string file(argv[1]);
	rca::SteinerReader reader (file);
	rca::Network *net = new rca::Network;	
	reader.configNetwork (net);
	rca::Group terminals = reader.terminalsAsGroup ();	

	terminals.addMember (terminals.getSource () );

	//function from heuristic
	init (net,&terminals);

	std::string pop_size (argv[3]);
	Transgenetic TA( atoi( pop_size.c_str() ) );

	std::string host(argv[2]);
	read_path (host);

	std::string gen (argv[4]); //generations

	clock_t t1, t2;	
	TA.initialization ();

    t1 = clock ();    
	TA.run5 ( atoi (gen.c_str () ) );
	t2 = clock ();
	cout << " R:";
	cout << (double)(t2 - t1) / CLOCKS_PER_SEC * 1000 << " ms";
	
	cout << " B:";
	TA.bestInfo ();	

	return 0;
}
