#include <iostream>
#include <string>

using namespace std;

double preprocessing (std::string file); 

/**
 * argc[1] -- the instance representing the network
 * argc[2] -- the pre-processing file to be read
 * 
 */
int main (int argv, char**argc) {
	
	Reader reader (argc[1]);
	reader.configNetwork ( m_net.get () );
	m_groups = reader.readerGroup ();
	
	return 0;
}