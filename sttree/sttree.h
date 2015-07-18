#ifndef _STTREE_
#define _STTREE_

#include <iostream>
#include <vector>

typedef struct edge_t {

	int x;
	int y;
	double value;
	bool in;
	
	edge_t *prev;
	edge_t *next;
	
	edge_t () : x(-1), y(-1) {
		prev = NULL;
		next = NULL;
		
	}
	
	edge_t  (int _x, int _y, double _value) : x(_x), y(_y), value(_value){
		prev = NULL;
		next = NULL;
		in = false;
	}
	
	~edge_t () {
		prev = NULL;
		next = NULL;
		x = -1;
		y = -1;
	}
	
} edge_t;

typedef struct list_of_edge {
	
	edge_t * begin;
	
	list_of_edge () { begin = NULL; }
	
	~list_of_edge () {
#ifdef DEBUG1
		printf ("list_of_edge destroyed(%x)\n", this);
#endif
		edge_t * aux = begin;
		while (aux != NULL) {
			edge_t * tmp = aux;
			aux = aux->next;
			delete tmp;
		}
	}
	
	void clear () {
		edge_t * aux = begin;
		while (aux != NULL) {
			edge_t * tmp = aux;
			aux = aux->next;
			delete tmp;
		}
		begin = NULL;
	}

	void add_edge (edge_t *edge) {
		
		if (begin == NULL){
			edge->next = begin;
		} else {
			edge->next = begin;
			begin->prev = edge;
		}
		begin = edge;
	}
	
	void remove_edge (edge_t * edge) {
		
		if ( edge == begin ) {
			
			begin = edge->next;
			edge->next = NULL;
			edge->prev = NULL;
			begin->prev = NULL;
			
			delete edge;
			edge = NULL;
			
		} else if (edge->next == NULL) {
			
			edge->prev->next = edge->next;
			edge->prev = NULL;
			delete edge;
			edge = NULL;
			
		} else {
			
			edge->next->prev = edge->prev;
			edge->prev->next = edge->next;
			edge->prev = NULL;
			edge->next = NULL;
			delete edge;
			edge = NULL;
			
		}
		edge = NULL;
	}
	
}list_edge;

typedef struct leaf {
	
	int id;
	
	leaf (int _id) : id(_id){
		next = NULL;
		prev = NULL;
	}
	
	~leaf () {
		next = NULL;
		prev = NULL;
	}
	
	leaf * next;
	leaf * prev;
	
} leaf_t;

typedef struct list_of_leafs {
	
	leaf * begin;
	
	list_of_leafs () {
		begin = NULL;
	}
	
	leaf * first () {
		return begin;
	}
	
	~list_of_leafs () {
		leaf *aux = begin;
		while (aux != NULL) {
			leaf * tmp = aux;
			aux = aux->next;
			delete tmp;
		}
	}
	
	void add (leaf_t * l) {
		
		if (begin == NULL) {
			begin = l;
		} else {		
			l->next = begin;
			begin->prev = l;
			begin = l;
		}
	}
	
	leaf * remove (leaf_t *l) {
		
		if (l == begin) {
			
			begin = l->next;
			delete l;
			l = NULL;
			
		} else if (l->next == NULL) {
			
			l->prev->next = l->next;
			l->prev = NULL;
			delete l;
			l = NULL;
		} else {
			
			l->next->prev = l->prev;
			l->prev->next = l->next;
			l->prev = NULL;
			l->next = NULL;
			delete l;
			l = NULL;
		}
		
		return l;
	}
	
} list_leafs_t;

typedef struct node_t{
	
	int id;	
	int degree;
	bool terminal;
	std::vector<edge_t*> edges;
	leaf_t * leaf;
	
	node_t (int _id, int _degree, bool _terminal = false) {
		id = _id;
		degree = _degree;
		terminal = _terminal;
		leaf = NULL;
	}
	
	~node_t () {
		leaf = NULL;
		for (edge_t * t: edges) {
			t = NULL;
		}
	}
	
	void add_adjacent_vertex (edge_t * edge) {
		edges.push_back (edge);
	}
	
	int get_edge_pos (int y) {
	
		int _x = (id > y ? id : y);
		int _y = (id < y ? id : y);
		
		for (size_t i=0; i < edges.size(); i++) {
			if (edges[i] != NULL) {
				if ( (edges[i]->x = _x) && (edges[i]->y == _y) ) {
					return i;
				}
			}
		}
		return -1;
	}
	
	edge_t * remove_adjacent_vertex () {
		for (size_t i=0; i < edges.size(); i++) {
			if (edges[i]->in == true) {
				
				if ( edges[i]->x == id || edges[i]->y == id)
					return edges[i];
			}
		}
		return NULL;
	}
	
	void add_leaf_node (leaf_t * _leaf) {
		leaf = _leaf;
	}
	
	void increase_degree (){degree++;}
	void decrease_degree (){degree--;}
	int get_degree (){return degree;}
	bool is_terminal (){return terminal;}
} node_t;

class STTree {
	
public:
	STTree () {}
	STTree (int nnodes);
	STTree (int nnodes, const std::vector<int> & terminals);
	STTree (int nnodes, int source ,const std::vector<int> & terminals) {
		_init_nodes (nnodes);
		_init_terminals(terminals);
		m_nodes[source].terminal = true;
		
		m_source = source;
		
		m_cost = 0.0;
		
		m_edges.clear ();
	}
	
	/*Copy Constructor*/
	STTree (const STTree & ref);
	/*assingment operator*/
	STTree & operator= (const STTree & ref);
	
	~STTree () {
#ifdef DEBUG1
		printf ("STTree destroyed(%x)\n", this);
#endif
	}
	
	inline int getNodes () {return m_nodes.size();}
	inline int getSource () {return m_source;}
	std::vector<int> getTerminals () {
		std::vector<int> term;
		for (int i=0;i < m_nodes.size (); i++) {
			if (m_nodes[i].terminal)
				term.push_back (i);
		}
		return term;
	}
	
	void add_edge (int x, int y, double value);
	void remove_edge (int x, int y);
	
	node_t & get_node (int pos) {
		return m_nodes[pos];
	}
	
	inline double getCost () {
		return m_cost;
	}
	
	inline void setCost (double cost) {
		m_cost = cost;
	}
	
	edge_t * get_edge () const{
		return m_edges.begin;
	}
	
	list_edge & get_edges () {
		return m_edges;
	}
	
	list_leafs_t & get_leafs () {
		return m_leafs;
	}
	
	void prunning ();
	
	void xdotFormat ();
	
private:
	void _init_nodes(int nnodes) {
		for (size_t i = 0; i < nnodes; i++) {
			m_nodes.push_back (node_t(0,0,false));
			m_nodes[i].id = i;
		}
	}
	void _init_terminals (const std::vector<int> & terminals) {
		for (const int & member : terminals) {
			m_nodes[member].terminal = true;
#ifdef DEBUG1
	std::cout << member << std::endl;
#endif			
		}
	}
	
	void _leaf_traits (int node);
	
	
private:
	std::vector<node_t> m_nodes;
	double m_cost;
	
	list_leafs_t m_leafs;
	list_edge m_edges;
	
	int m_source;
	
};

#endif