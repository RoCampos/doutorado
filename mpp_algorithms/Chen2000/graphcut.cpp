#include <iostream>
#include <memory>
#include "network.h"
#include "reader.h"

using namespace std;
using namespace rca;

//função para inicializar variáveis
void init (std::string str);

//função que cria uma restrição para um corte no grafo
void create_constraints (int, int);

//ponteiro para objeto Network
shared_ptr<Network> net;

int main (void) {

	int NODES = net.get ()->getNumberNodes ();
	
	//número de nós no corte
	int i = 1;
	bool _continue = true;
	
	while ( _continue ) {
		
		for (int i = 0; i < NODES; i++) {
			
		}
		
		i++;
		
	}
	
	return 0;
}

void init (std::string str) {
	
	Network _net;
	net = make_shared<Network> (_net);
	
	Reader reader(str);
	reader.configNetwork ( net.get() );
	
}

void create_constraints (int node, int corte) {
	
	std::string str("");
	
}