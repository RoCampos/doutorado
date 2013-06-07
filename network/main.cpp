#include <iostream>

#include "steinerReader.h"
#include "heuristic.h"
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
	
	init (net,&terminals);	

	KShortestPath kpaths2 (net);
	kpaths2.init ( ( terminals.getMember (0)-1) , (terminals.getMember (3) - 1) );

	while (kpaths2.hasNext() ) {

		rca::Path path = kpaths2.next ();
		cout << path << " : " << path.getCost () << endl;		
	}

	return 0;
}
