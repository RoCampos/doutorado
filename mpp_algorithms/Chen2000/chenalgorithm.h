#ifndef CHEN_ALGORITHM_H
#define CHEN_ALGORITHM_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <climits>
#include <sstream>
#include <algorithm>
#include <functional>
#include <queue>

#include <memory>

#include "group.h"
#include "network.h"
#include "reader.h"

typedef struct sttree {
	
	int id;
	std::vector<rca::Link> edges;
	
	sttree (int _id):id(_id){}
	
	void addEdge (int i, int j) {
		rca::Link  l = rca::Link(i,j,0);
		edges.push_back(l);
	}
	
	void replace (rca::Link & link, rca::Link & nlink) {
		
		auto it = find (edges.begin(), edges.end(), link);
		if (it != edges.end()) {
			edges.erase (it);
		}		
		edges.push_back (nlink);
	}
	
	void print () {
		for (auto it = edges.begin(); it != edges.end(); it++) {
			cout << *it << endl;
		}
	}
	
}STTree;

class Chen {

public: 
	/**
	 * Construtor recebe dois argumentos.
	 * O primeiro argumento indica a instance do problema
	 * O Segundo argumento indica uma arquivo pré-processado
	 * referente a primeira instância.
	 */
	Chen (std::string _instance, std::string _input, int alpha);
	
	//imprimir links utilizados e a congestão
	void print_edges_xdot ();
	void print_information ();
	
	void run ();
	
private:
	/**Método para reconstruir uma árvore e atualizar
	 lista de arestas*/
	void update ();
	std::vector<int> cut_edge (STTree & st, rca::Link & link);
	void replace (STTree & st, rca::Link & link);
	
	/*Método que inicializa as variáveis do MPP*/
	void init ();
	
	/*Método para ordenar as arestas pela congestão*/
	std::vector<rca::Link> sort_edges ();
	
	/*métod que obtém a congestão máxima*/
	int get_max_congestion ();
	
	/*pre processing step*/
	void pre_processing ();
		
private:
	
	std::string m_input;
	std::string m_instance;
	
	//matrix that stores the congestion over each egde
	std::vector< std::vector<int> > m_edges;
	
	//current max congestion
	double m_congestion;
	
	//initial congestion before start the algorithm
	double m_init_congestion;
	
	//the parameter that indicates the dilation
	int m_alpha;
	
	//list of steiner trees
	std::vector<STTree> m_trees;
	
	/*MPP variables*/
	std::shared_ptr<rca::Network> m_net; //the network
	std::vector< std::shared_ptr<rca::Group> > m_groups; //multicast groups
	
};

#endif /*CHEN_ALGORITHM_H*/
