#include "filter.h"

using namespace rca;
using namespace std;

FilterSol::FilterSol (std::string file, std::string outputfname) {
	
	str_file = file;
	str_output_file = outputfname;
	
}

/**
 * Método que realiza o filtro sobre um arquivo de saída gerado
 * pelo glpsol.
 * 
 * 
 */
void FilterSol::doFilter () {
	
	std::ifstream instance( str_file.c_str ());
	
	freopen (str_output_file.c_str (),"a",stdout);
	
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

/**
 * Método que recebe uma linha do arquivo gerado pelo glpsol.
 * Este método gera como saída uma aresta e o valor da variável binária
 * que indica que a mesma está ou não na solução.
 * 
 */
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
			
			stringstream number2;
			for (unsigned int j = i+1; j < str.length (); j++) {
				if ( isdigit(str[j]) ) {
					number2 << str[j];
				} else {
					i = j;
					break;
				}
			}
			
			//getting the value of binary varible that indicates if
			//the edge is in the tree
			stringstream inSolution;
			for (unsigned int j = i; j < str.length (); j++) {
				if (isdigit (str[j]) ) {
					inSolution << str[j];
					break;
				}
			}
			
			std::string variable = inSolution.str ();
			if ( atoi(variable.c_str () ) == 1) {
				cout << number1.str () << " - ";
				cout << number2.str () << ":";
				cout << inSolution.str ();
				cout << ";\n";
			}
		}
	}
	output_file << endl;	
}

/*void FilterSol::doFilter (boolean xdot) {
	
	
	
}*/


int main (int argv, char**argc) {
		
	FilterSol filter (argc[1], argc[2]);
	filter.doFilter ();
	
	return 0;
}