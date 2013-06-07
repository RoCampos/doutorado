#ifndef _STEINER_TREE_H_
#define _STEINER_TREE_H_

#include <vector>

#include "linkedlist.h"
#include "steinernode.h"
#include "edge.h"

class SteinerTree {
	
public:
	
	SteinerTree(int nobj, int nnodes);
	
	//adiciona uma aresta da árvore
	void addEdge(int i, int j, int value);
	
	//define estruturas, speed up the algorithm
	void setTempStructures (std::vector<double>& objs, 
							std::vector<SteinerNode>& vertexList);
	
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