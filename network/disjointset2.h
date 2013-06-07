#ifndef _DISJOINT_SET_H_
#define _DISJOINT_SET_H_

#include <iostream>
#include <vector>
#include <ostream>


/**
 * Esta estrutura representa uma partição
 * de um conjunto disjunto.
 * 
 * @author Romerito Campos
 * @date 2012/03/22
 */
struct NodeDs {

	int item;
	int rank;
	int count;
	NodeDs * parent;
	
	NodeDs (int _item) {
		item = _item;
		rank = 0;
		count = 1;
		parent = NULL;
	}
	
	NodeDs & operator=(const NodeDs & obj) {
		
		item = obj.item;
		parent = obj.parent;
		rank = obj.rank;
		count = obj.count;
		
		return *this;
	}
	
	~NodeDs () {
		parent = NULL;
	}
	
	bool operator!=(const NodeDs & obj) {
		return parent != obj.parent;
	}
	
	friend std::ostream & operator<<(std::ostream & out, NodeDs & obj) {
		
		int _parent = (obj.parent ? obj.parent->item : -1);
		out << "Item: "<< obj.item << " Parent: "<< _parent;
		return out;
	}
	
};

/**
 * Esta classe representa um Conjunto Disjunto.
 * Ela possui implementação das três funções básicas.
 * Make_set, Find_set, Uniou.
 * 
 * É utilizado o find com compressão de caminho.
 * 
 * @author Romerito Campos
 * @date 2012/22/02
 */
class DisjointSet2 {
	
	std::vector<NodeDs*> m_partition;
	
	int m_size;
	
	void make_set (int);
	
	int m_numberSets;
	
public:
	
	DisjointSet2 (int size);
	
	DisjointSet2 ();
	
	~DisjointSet2 ();
	
	//retorna partição que tem o item 
	NodeDs* find (int);
	
	//find usind compress path heuristic
	NodeDs* find2 (int);
	
	void addPartition (int);
	void init (int);
	void setSize (int size) {m_size = size;}
	
	int getSize () const {return m_numberSets;}
	
	//une dois conjuntos que tenhos os elementos
	bool simpleUnion (int, int);
	void print ();
	
};

#endif
