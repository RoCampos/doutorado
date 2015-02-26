#include "SolverDataGenerator.h"

void MultiCommidityFormulation::generate (rca::Network * network, 
										  rca::Group *g)
{
	
	std::cout << "set V := ";
	for (int i=0;i < network->getNumberNodes (); i++) {
		std::cout << i + 1 << " ";
	}
	std::cout << ";\n";

	std::cout << "\nset LINKS :=";
	for (int i=0; i < network->getNumberNodes (); i++) {
		for (int j=0; j < i; j++) {
			
			if (network->getCost (i,j) > 0.0) {
				std::cout <<"\n\t(" <<  i+1 << "," << j+1 << ")";
				std::cout <<"\n\t(" <<  j+1 << "," << i+1 << ")";
			}
			
		}	
	}
	std::cout << " ;\n";
	
	std::cout << "\nset T := ";
	for (int i=0; i < g->getSize (); i++) {
		std::cout << " " << g->getMember (i) + 1;
	}
	std::cout << " ;\n";
	
	std::cout << "param r := " << g->getSource ()+1 << ";\n";
	
	std::cout << "param : cost delay:=";
	for (int i=0; i < network->getNumberNodes (); i++) {
		for (int j=0; j < i; j++) {
			
			if (network->getCost (i,j) > 0.0) {
				std::cout <<"\n\t" <<  i+1 << "\t" << j+1;
				std::cout <<"\t" << network->getCost (i,j);
				std::cout <<"\t" << (int)network->getBand (i,j);
				
				std::cout <<"\n\t" <<  j+1 << "\t" << i+1;
				std::cout <<"\t" << network->getCost (i,j);
				std::cout <<"\t" << (int)network->getBand (i,j);
			}			

		}	
	}
	std::cout << ";\n";
}

void MultipleMulticastCommodityFormulation::generate (rca::Network * network, 
													  std::vector<std::shared_ptr<rca::Group>> & groups)
{
	//gerando o conjunto de vértices
	std::cout << "set VERTEX := ";
	for (int i=0;i < network->getNumberNodes (); i++) {
		std::cout << i + 1 << " ";
	}
	std::cout << ";\n";
	
	//gerando o conjunto de arestas
	std::cout << "\nset LINKS :=";
	for (int i=0; i < network->getNumberNodes (); i++) {
		for (int j=0; j < i; j++) {
			
			if (network->getCost (i,j) > 0.0) {
				std::cout <<"\n\t(" <<  i+1 << "," << j+1 << ")";
				std::cout <<"\n\t(" <<  j+1 << "," << i+1 << ")";
			}
			
		}	
	}
	std::cout << " ;\n";
	
	//gerando param de groups
	std::cout << "set GROUPS := ";
	for (unsigned int i=0; i < groups.size (); i++) {
		std::cout << " " << i + 1;
	}
	std::cout << ";\n";
	
	//gerando param de raízes
	std::cout << "param Mroot := \n";
	for (unsigned int i=0; i < groups.size (); i++) {
		std::cout << "\t" << i+1 << " " << groups[i]->getSource () + 1<<"\n";
	}
	std::cout << ";\n";
	
	std::cout << "set MEMBER := \n";
	for (unsigned int i=0; i < groups.size (); i++) {
		
		std::vector<int> members = groups[i]->getMembers ();
		for (unsigned int j = 0; j < members.size (); j++) {
			
			std::cout << "\t" << i+1 << " " << (members[j] + 1) << "\n"; 
			
		}
		
	}
	std::cout << ";\n";
	
	//gerando parametros de cost e delay
	std::cout << "param : cost cap:=";
	for (int i=0; i < network->getNumberNodes (); i++) {
		for (int j=0; j < i; j++) {
			
			if (network->getCost (i,j) > 0.0) {
				std::cout <<"\n\t" <<  i+1 << "\t" << j+1;
				std::cout <<"\t" << (int)network->getCost (i,j);
				std::cout <<"\t" << (int)network->getBand (i,j);
				
				std::cout <<"\n\t" <<  j+1 << "\t" << i+1;
				std::cout <<"\t" << (int)network->getCost (i,j);
				std::cout <<"\t" << (int)network->getBand (i,j);
			}			

		}	
	}
	std::cout << ";\n";

	std::cout << "param traffic := \n";
	for (unsigned int i=0; i < groups.size (); i++) {
		
		std::cout << "\t" << i+1 << " " << groups[i]->getTrequest () << "\n";
		
	}
	std::cout << ";\n";
	
}
