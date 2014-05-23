#include "filter.h"

using namespace rca;
using namespace std;

FilterSol::FilterSol (std::string file, std::string outputfname) {
	
	str_file = file;
	str_output_file = outputfname;
	
}

void FilterSol::doFilter () {
	
	std::ifstream instance( str_file.c_str ());
	
	freopen (str_output_file.c_str (),"w",stdout);
	
	int isReady = 0;
	if (instance.good () ) {
		
		while ( instance.good () ) {
			
			char line[256];
			instance.getline (line, 256);
			
			std::string str_tmp(line);
			
			if (str_tmp.find ("Activity") != string::npos) {
				isReady++;
			} else if (str_tmp.find ("Integer feasibility") != string::npos ) {
				break;
			}
			
			if (isReady == 2) {
				getVariable (line);
			}
			
		}
		
	}
	
}

void FilterSol::getVariable (std::string str) {
	
	for (unsigned int i=0; i < str.length (); i++) {
		
		if ( islower (str[i]) && str[i] == 'y') {
			
			//getting the first number
			stringstream number1;
			for (unsigned int j = i+2; j < str.length (); j++) {
				
				if ( isdigit(str[j]) ) {
					number1 << str[j];
				} else if ( ispunct (str[j]) ) {
					i = j;
					break;
				}
			}
			
			cout << number1.str () << " - ";
			
			stringstream number2;
			for (unsigned int j = i+1; j < str.length (); j++) {
				if ( isdigit(str[j]) ) {
					number2 << str[j];
				} else {
					i = j;
					break;
				}
			}
			
			cout << number2.str () << ":";
			
			//getting the value of binary varible that indicates if
			//the edge is in the tree
			stringstream inSolution;
			for (unsigned int j = i; j < str.length (); j++) {
				if (isdigit (str[j]) ) {
					inSolution << str[j];
					break;
				}
			}
			cout << inSolution.str ();			
			cout << ";\n";
		}
		
	}
	//cout << endl;
	
	output_file << endl;
	
}


int main (int argv, char**argc) {
		
	FilterSol filter (argc[1], argc[2]);
	filter.doFilter ();
	
	return 0;
}