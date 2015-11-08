#ifndef _STEINER_TREE_H_
#define _STEINER_TREE_H_

#include <vector>
#include <fstream>
#include <iomanip>

#include "linkedlist.h"
#include "steinernode.h"
#include "edge.h"

/**
 * @deprecated
 * Esta classe representa uma árvore de Steiner.
 * 
 *
 * @author Romerito Campos
 * @date 07/25/2014
 * @version 0.2 
 */
class SteinerTree {
	
public:
	
	SteinerTree () {}
	
	SteinerTree(int nobj, int nnodes);
	
	//construtor usado para árvore de Steiner com 1 objetivo
	//as estruturas temporárias são inicializadas no construtor.
	SteinerTree(int nnodes);
	
	/**
	 * This constructor receive the number of nodes in the network,
	 * the source of a multicast group that is interpreted as a terminal and
	 * the list of destinations that is interpreted as terminals as well.
	 * 
	 */
	SteinerTree(int nnodes, int source, const std::vector<int> & terminals);
	
	//adiciona uma aresta da árvore
	bool addEdge(int i, int j, double value);
	
	//define estruturas, speed up the algorithm
	//void setTempStructures (std::vector<double>& objs, 
	//						std::vector<SteinerNode>& vertexList);
	
	//retorna a lista de nós da árvore
	std::vector<SteinerNode> getNodes () {return vertexList;}
	
	//retorna a lista de nós folhas
	LinkedList getCurrentLeafs () {return leafs;}
	
	//define um nó como terminal
	inline void setTerminal (int i){ vertexList[i].setTerminal (true);}
	
	inline double getCost () {return objectives[0];}
	
	//método para atualizar linked list de folhas
	bool updateLeafs (int);
	
	void prunning ();
	
	void xdotFormat ();
	
	void xdotToFile (std::string file);
	
private:
	int num_obj;
	int num_nodes;
	std::vector<double> objectives;
	//std::vector<SteinerNode> vertexList;
	//LinkedList leafs;

public:
	std::vector<SteinerNode> vertexList;
	LisfOfEdge listEdge;
	LinkedList leafs;
	
		
};



#endif