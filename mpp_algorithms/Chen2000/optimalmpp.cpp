#include <iostream>
#include <string>
#include <fstream>

#include "reader.h"

using namespace std;

double preprocessing (std::string file); 

/**
 * argc[1] -- the instance representing the network
 * argc[2] -- the pre-processing file to be read
 * 
 */
int main (int argv, char**argc) {
	
	Reader reader (argc[1]);
	
	return 0;
}

double preprocessing (std::string file) {

	ifstream archive (file.c_str());
	
	double cost = 0.0;
	if (archive.good ()) {
		
	}
	
	return cost;
}