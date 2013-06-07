#include <iostream>

#include "steinerReader.h"
#include "time.h"
#include "heuristic.h"

int main (int argc, char** argv) {


	std::string file(argv[1]);
	rca::SteinerReader reader (file);
	rca::Network *net = new rca::Network;	
	reader.configNetwork (net);
	rca::Group terminals = reader.terminalsAsGroup ();	

	terminals.addMember (terminals.getSource () );

	//function from heuristic
	init (net,&terminals);

	std::string _file(argv[2]);
	read_path (_file);

	write_path ("saida2.txt");

	return 0;
}
