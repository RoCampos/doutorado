#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#include "reader.h"
#include "network.h"
#include "link.h"

using namespace std;

double preprocessing (std::string file);

Network * m_network;

/**
 * argc[1] -- the instance representing the network
 * argc[2] -- the pre-processing file to be read
 * 
 */
int main (int argv, char**argc) {
	
	Reader reader (argc[1]);
	string str(argc[2]);
	
	m_network = new Network;
	
	reader.configNetwork (m_network);
	
	preprocessing (str);
	
	return 0;
};

double preprocessing (std::string file) {

	ifstream archive (file.c_str());
	double cost = 0.0;
	
	std::vector<double> trees;
	
	std::vector<rca::Link> alltrees;
	
	if (archive.good ()) {
			
		std::string str;
		double costbytree = 0.0;
		
		while ( getline (archive, str) ) {
						
			if ( str.empty () ) {
				
				trees.push_back (costbytree);
				costbytree = 0.0;
				
			} else {
			
				istringstream line (str);
				int x;
				int y;
			
				//getting the first vertex
				line >> x;
				line >> str;
				line >> str;
				
				//getting the second vertex
				str = str.substr (0, str.size () - 2);
				y = atoi (str.c_str() );
				
				--x;
				--y;
				
				double c = m_network->getCost (x, y);
				rca::Link link (x, y, c);
				
				//inserting the link in the alltrees structure
				auto it = std::find (alltrees.begin(), alltrees.end(), link);
				if ( it == alltrees.end()) {
					alltrees.push_back (link);
					
					cost += link.getValue();
					
				}
				
				costbytree += link.getValue();
			
			}
			
		}
		
	}//endif
	
	cout << cost;
	auto lperc = trees.begin();
	for ( ; lperc != trees.end(); ++lperc) {
		cout <<"\t"<<*lperc;
	}
	cout << endl;
	
	return cost;
}