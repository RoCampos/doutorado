#include <iostream>

#include "steinerReader.h"
#include "time.h"
#include "heuristic.h"

/*
*
* recebe três argumentos de entrada.
* 1º informa a quantidade de caminhos.
* 2º informa a instância
* 3º informa a saida com o os caminhos pré-processados.
*/
int main (int argc, char** argv) {

	srand (time(NULL));
	
	/*Reading informations of instance*/
	std::string file(argv[2]);
	rca::SteinerReader reader (file);
	rca::Network *net = new rca::Network;	
	reader.configNetwork (net);
	rca::Group terminals = reader.terminalsAsGroup ();	

	terminals.addMember (terminals.getSource () );

	//function from heuristic
	init (net,&terminals);

	std::string k_paths(argv[1]);

	clock_t t1, t2;
	t1 = clock ();
	initHostInfo ( atoi (k_paths.c_str ()) );
	t2 = clock ();
	cout << "H:";
	cout << (double)(t2 - t1) / CLOCKS_PER_SEC * 1000 << " ms\n";

	std::string output(argv[3]);
	write_path (output);

	return 0;
}
