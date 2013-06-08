#include "readFile.h"

using namespace std;

//le o arquivo para configuração
void read_file (std::string input,std::string output) 
{

	std::ifstream file_input (input.c_str () , std::ifstream::in ); //arquivo entrada
	std::ofstream file_output (output.c_str () , std::ofstream::app); //arquivo saida.

	while ( file_input.good () ) {
		
		std::string text;
		file_input >> text;

		if (text.compare("ms") == 0 ) {
			
			file_input >> text;			

			file_output << text.substr (2) << endl;

		}

	}

	file_input.close ();
	file_output.close ();

}

void average (std::string input, std::string output) 
{

	std::ifstream file_input (input.c_str ()); //arquivo entrada
	std::ofstream file_output (output.c_str ()); //arquivo saida.

	double sum = 0.0;
	int count = 0;

	while ( file_input.good () ) {
		
		std::string text;
		file_input >> text;
	
		sum += atof (text.c_str () );

		count++;

		if (count == 20 ) {
	
			file_output << sum/count << endl;
			count = 0;
			sum = 0.0;
		}

	}

	file_input.close ();
	file_output.close ();

}

//implementação considerando apenas 20 execuções
void median (std::string input, std::string output) {

	std::ifstream file_input (input.c_str ()); //arquivo entrada
	std::ofstream file_output (output.c_str ()); //arquivo saida.

	int count = 0;
	std::vector<double> values;

	while ( file_input.good () ) {
		
		std::string text;
		file_input >> text;
	
		double _values =  atof (text.c_str () );
		values.push_back ( _values );

		count++;

		if (count == 20 ) {
	
			std::sort ( values.begin (), values.end () );

			_values = values[9] + values[10];

			file_output << (double)(_values/2)  << endl;
			count = 0;

		}

	}
	
	

	file_input.close ();
	file_output.close ();
}

