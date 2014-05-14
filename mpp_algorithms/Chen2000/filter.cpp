#include "filter.h"

using namespace rca;
using namespace std;

FilterSol::FilterSol (std::string file) {
	
	str_file = file;
	
}

void FilterSol::doFilter () {
	
	std::ifstream instance( str_file.c_str ());
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

void FilterSol::getVariable (char str[]) {
	
	for (int i=0; i < LINE_SIZE; i++) {
		
		if ( islower (str[i]) && str[i] == 'y') {
			
			stringstream number1;
			for (int j = i+2; j < LINE_SIZE; j++) {
				
				if ( isdigit(str[j]) ) {
					number1 << str[j];
				} else if ( ispunct (str[j]) ) {
					i = j;
					break;
				}
				
			}
			cout << number1.str () << ",";
			
			stringstream number2;
			for (int j = i+1; j < LINE_SIZE; j++) {
				if ( isdigit(str[j]) ) {
					number2 << str[j];
				} else {
					i = j;
					break;
				}
			}
			
			cout << number2.str () << ",";
			
			for (int j = i; j < LINE_SIZE; j++) {
				if (isdigit (str[j]) ) {
					putchar (str[j]);
					break;
				}
			}
			cout << ";\n";
			
		}
		
		
	}
	cout << endl;
	
}


int main (int argv, char**argc) {
		
	FilterSol filter (argc[1]);	
	filter.doFilter ();
	
	return 0;
}