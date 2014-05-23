#include "filter.h"

using namespace rca;
using namespace std;

FilterSol::FilterSol (std::string file, std::string outputfname) {
	
	str_file = file;
	str_output_file = outputfname;
	
}

void FilterSol::doFilter () {
	
	std::ifstream instance( str_file.c_str ());
	
	output_file.open (str_output_file.c_str (), std::ofstream::out | std::ofstream::app);
	
	if ( !output_file.good () ) {exit(1);}
	
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
	output_file.close ();
	
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
			
			output_file << number1.str () << " - ";
			
			cout << number1.str () << " - ";
			
			stringstream number2;
			for (int j = i+1; j < LINE_SIZE; j++) {
				if ( isdigit(str[j]) ) {
					number2 << str[j];
				} else {
					i = j;
					break;
				}
			}
			
			output_file << number2.str () << ":";
			
			cout << number2.str () << ":";
			
			for (int j = i; j < LINE_SIZE; j++) {
				if (isdigit (str[j]) ) {
					stringstream cost;
					cost << str[j];
					
					cout << cost.str ();
					output_file << cost.str ();
					
					break;
				}
			}
			output_file << ";\n";
			cout << ";\n";
		}
		
	}
	cout << endl;
	
	output_file << endl;
	
}


int main (int argv, char**argc) {
		
	FilterSol filter (argc[1], argc[2]);
	filter.doFilter ();
	
	return 0;
}