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
void median (std::string input, std::string output, int sample) {

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

		if (count == sample ) {
	
			std::sort ( values.begin (), values.end () );

			if ( sample % 2 == 0 ) {
				int pos = (sample / 2) - 1;
				_values = values[pos] + values[ pos + 1];

				file_output << (double)(_values/2)  << endl;

			} else {
				file_output << values [ (sample / 2 ) + 1 ]  << endl;
			}
			count = 0;

		}

	}
	
	

	file_input.close ();
	file_output.close ();
}

int min (std::string input) {

	std::ifstream file_input (input.c_str ()); //arquivo entrada

	std::vector<double> values;

	std::string text;
	while ( file_input.good () ) {
		file_input >> text;
		values.push_back ( atof( text.c_str() ) );
	}

	sort (values.begin (), values.end ());

	file_input.close ();

	return values[0];

}

