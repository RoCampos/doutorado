#include <iostream>

#include "transgenetic.h"
#include "steinerReader.h"

#include "time.h"

/*
* argumentos:
* argv[1] instância
* argv[2] info_a_priori
* argv[3] pop_size
* argv[4] generation
*/
int main (int argc, char** argv) {

	long int t = time (NULL);
	srand (t);

	if (argc < 8) {
		cout << argc << endl;
		cout << "*** argumentos: \n* argv[1] instância\n* argv[2] info_a_priori\n* argv[3] pop_size\n";
		cout << "* argv[4] generation\n";
		exit (1);
	}
	
	/*Reading informations of instance*/
	std::string file(argv[1]);
	rca::SteinerReader reader (file);
	rca::Network *net = new rca::Network;
	reader.configNetwork (net);
	rca::Group terminals = reader.terminalsAsGroup ();	

	terminals.addMember (terminals.getSource () );
	
	init (net, &terminals);

	addHostInfo (atoi (argv[5]) , atoi(argv[6]), atoi(argv[7]) );
	
	//std::string pop_size (argv[3]);
	//Transgenetic TA( atoi( pop_size.c_str() ) );
	
	//read_path (argv[2]);
	//TA.initialization ();

	
	return 0;
}
