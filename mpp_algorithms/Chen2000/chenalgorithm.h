#ifndef CHEN_ALGORITHM_H
#define CHEN_ALGORITHM_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <memory>

#include "group.h"
#include "network.h"
#include "reader.h"

typedef struct sttree {
	
	
}STTree;

class Chen {

public: 
	/**
	 * Construtor recebe dois argumentos.
	 * O primeiro argumento indica a instance do problema
	 * O Segundo argumento indica uma arquivo pré-processado
	 * referente a primeira instância.
	 */
	Chen (std::string _instance, std::string _input);
	
	/*pre processing step*/
	void pre_processing ();
	
private:
	/*Método que inicializa as variáveis do MPP*/
	void init ();
	
private:
	
	std::string input;
	std::string instance;
	
	//matrix that stores the congestion over each egde
	std::vector< std::vector<int> > edges;
	
	//current max congestion
	double congestion;
	
	//the parameter that indicates the dilation
	int alpha;
	
	std::vector<STTree> tree;
	
	/*MPP variables*/
	std::shared_ptr<rca::Network> m_net; //the network
	std::vector< std::shared_ptr<rca::Group> > m_groups; //multicast groups
	
};

#endif /*CHEN_ALGORITHM_H*/
