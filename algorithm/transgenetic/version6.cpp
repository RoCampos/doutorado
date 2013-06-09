#include <iostream>

#include "transgenetic.h"
#include "include/steinerReader.h"

#include "time.h"

int main (int argc, char** argv) {

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

	Transgenetic TA(60);

	std::string host(argv[2]);
	read_path (host);

	clock_t t1, t2;	
	TA.initialization ();

	t1 = clock ();    
	TA.run6 (300,0.6);
	t2 = clock ();
	cout << " R:";
	cout << (double)(t2 - t1) / CLOCKS_PER_SEC * 1000 << " ms";
	
	cout << " B:";
	TA.bestInfo ();	

	return 0;
}
