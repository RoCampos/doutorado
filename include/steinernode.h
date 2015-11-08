#ifndef _STEINERNODE_H_
#define _STEINERNODE_H_

#include <ostream>
#include "node.h"
#include "edge.h"

/**
 * @deprecated
 * Esta classe representa um nó de Steiner.
 * 
 * 
 * @author Romerito Campos
 * @date 04/20/2014
 */
class SteinerNode {
	
public:
	SteinerNode(int idx, int _value, bool _terminal);
	friend std::ostream & operator<< (std::ostream & out, const SteinerNode &st);
	
	inline void setIndex (int idx){index = idx;}
	inline int getIndex () {return index;}
	
	//incremento e decremento do grau dos nós
	void increaseDegree ();
	void decreaseDegree ();
	
	//retorna verdadeiro se o nó é folha ou não
	bool isLeaf () {return leaf;}
	
	//manipulação de nós terminais
	void setTerminal (bool _term){terminal = _term;}
	bool getTerminal (){return terminal;}
	bool isTerminal () {return terminal;}
	
	//ponteiro para nó na lista de folhas
	//se for folha da árvore em algum momento
	void setNode (Node*);
	Node* getNode (){return node;}
	
	inline int getDegree (){return degree;}
	
	inline void setFirstAdjc (int j){firstAdjacent = j;}
	inline int getFirstAdjc () {return firstAdjacent;}
	
	inline void setEdge_ (Edge * __edge){_edge = __edge;}
	inline Edge * getEdge_ (){return _edge;};
	
private:
	int index; //indice do vértice na lista
	int value;	
	bool terminal;//indica se é terminal
	int degree; //grau do vértice
	bool leaf; //indica se é folha	
	
	//a outra extremidade da aresta que adiona este nó
	int firstAdjacent; 
	Edge * _edge;
	
	Node *node; //pointer to node, with it is a leaf
	
};

#endif
