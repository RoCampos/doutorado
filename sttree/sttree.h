#ifndef _STTREE_
#define _STTREE_

#include <iostream>
#include <vector>


/**
* Estrutura que representa um tipo aresta
*
* Aresta é representa como uma lista duplamente ligada.
* POssui um ponteiro para a próxima aresta e para a aresta anterior.
*
* @author Romerito C. Andrade
*/
typedef struct edge_t {

	int x;
	int y;
	double value;
	bool in;
	
	edge_t *prev;
	edge_t *next;
	
	/**
	* Construtor .
	*/
	edge_t () : x(-1), y(-1) {
		prev = NULL;
		next = NULL;
		
	}
	
	/**
	* Construtor com três parâmetros: nó x, nó y e o peso do aresta.
	*
	* @param int nó x
	* @param int no y
	* @param double peso do nó	
	*/
	edge_t  (int _x, int _y, double _value) : x(_x), y(_y), value(_value){
		prev = NULL;
		next = NULL;
		in = false;
	}
	
	/**
	* Destrutor.
	* 
	*/
	~edge_t () {
		prev = NULL;
		next = NULL;
		x = -1;
		y = -1;
	}
	
} edge_t;

/**
* Lista ligada de arestas.
*
* Cada nó da lista é um @see edge_t.
*
* @author Romerito C. Andrade
*/
typedef struct list_of_edge {
	
	edge_t * begin;
	
	/**
	* Construtor.
	*
	*/
	list_of_edge () { begin = NULL; }
	
	/**
	* Destrutor.
	*
	*/
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
	
	/**
	* Método que limpa a lista de arestas
	*
	*
	*
	*/
	void clear () {
		edge_t * aux = begin;
		while (aux != NULL) {
			edge_t * tmp = aux;
			aux = aux->next;
			delete tmp;
		}
		begin = NULL;
	}

	/**
	* Método que adiciona uma aresta a lista de arestas.
	*
	* @param edge_t arestas a ser adicionada a lista de arestas.
	*/
	void add_edge (edge_t *edge) {
		
		if (begin == NULL){
			edge->next = begin;
		} else {
			edge->next = begin;
			begin->prev = edge;
		}
		begin = edge;
	}
	
	/**
	* Método que remove uma aresta da lista de arestas
	*
	* @param edge_t arestas a ser removida.
	*/
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

/**
* Estrutura representa um nó folha da árvore de steiner.
* 
* Ele é usada para representar nós não terminais que sejam
* folha e, portanto, candidatos a serem removidos da árovore.
*
* A estrutura mantém um ponteiro para próximo para um próximo leaf
* e um ponteiro anterior para leaf anterior.
*
* A variável id é o nó que está marcado como leaf
*
* @author Romerito C. Andrade
*
*/
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

/**
* Lista de objetos do tipo @see leaf
*
* @author Romerito C. Andrade
*/
typedef struct list_of_leafs {
	
	leaf * begin;
	
	/**
	* Construtor
	*
	*/
	list_of_leafs () {
		begin = NULL;
	}
	
	/**
	* Método que retorna ponteiro para o primeiro
	* elemento da lista.
	* 
	* @return leaf
	*/
	leaf * first () {
		return begin;
	}
	
	/**
	* 
	* Destrutor.
	*
	*/
	~list_of_leafs () {
		leaf *aux = begin;
		while (aux != NULL) {
			leaf * tmp = aux;
			aux = aux->next;
			delete tmp;
		}
	}
	
	/**
	* Método para adicionar uma folha a lista de folhas.
	* 
	* 
	* @param leaf_t
	*/
	void add (leaf_t * l) {
		
		if (begin == NULL) {
			begin = l;
		} else {		
			l->next = begin;
			begin->prev = l;
			begin = l;
		}
	}
	

	/**
	* Método utilizado para remover o objeto leaf
	* da lista de leaf's
	*
	* @param leaf_t
	* @return leaf
	*/
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

/**
* Estrutura utilizada para guardar informações sobre os
* nós armazenados na árvore de Steiner.
*
* A estrutura mantém um id do nó - o valor do id é o valor do nó.
*
* O grau do nó também é mantido pelo estrutura, assim como um 
* a informação sobre o nó ser um terminal ou não.
*
* @author Romerito C. Andrade
*/
typedef struct node_t{
	
	int id;	
	int degree;
	bool terminal;
	std::vector<edge_t*> edges;
	leaf_t * leaf;
	
	/**
	* Construtor da classe.
	* 
	* Recebe como argumento o id do noó, o grau e um bool indicando
	* se o nó é terminal ou não.
	* 
	* @param int id do nó
	* @param int grau do nó
	* @param bool se true indica que o nó é terminal.
	*
	*/
	node_t (int _id, int _degree, bool _terminal = false) {
		id = _id;
		degree = _degree;
		terminal = _terminal;
		leaf = NULL;
	}
	
	/**
	* Destrutor.
	* 
	*/
	~node_t () {
		leaf = NULL;
		for (edge_t * t: edges) {
			t = NULL;
		}
	}
	
	/**
	* Método que adiciona a aresta pela qual
	* o nó é inserido na árvore de steiner.
	*
	* @param edge_t aresta que liga o nó a árvore de steiner.
	*/
	void add_adjacent_vertex (edge_t * edge) {
		edges.push_back (edge);
	}
	
	/**
	* Método que retona a posição do nó na lista
	* de ajacentes do nó.
	*
	* Cada nó tem sua lista de adjacentes, então os nós vizinho são adicionados
	* a esta lista. Este método retorna a posição dos vizinhos para manipulação.
	*
	* @param int nó que se pretende saber a posição na lista de adjacentes
	* @return int posição do nó
	*/
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
	
	/**
	* Método que retorna a aresta que adicionou o nó
	* na árvore de steiner.
	* 
	* Este método é utilizado quando o nó em questão é folha e não terminal, 
	* então a aresta que o conecta a ávore será removida.
	*
	* @return edge_t aresta que contenca o nó a árvore
	*/
	edge_t * remove_adjacent_vertex () {
		for (size_t i=0; i < edges.size(); i++) {
			if (edges[i]->in == true) {
				
				if ( edges[i]->x == id || edges[i]->y == id)
					return edges[i];
			}
		}
		return NULL;
	}
	
	/**
	* Método que adiciona uma leaf a estrtuura.
	* Cada nó quando criado registra seu leaf para que seja possível
	* removê-lo se ele for uma folha não terminal
	*
	* @param leaf_t 
	*/
	void add_leaf_node (leaf_t * _leaf) {
		leaf = _leaf;
	}
	
	/**
	* método que aumenta o grau do nó
	*
	*/
	void increase_degree (){degree++;}

	/**
	* Método que diminui o grau de um nó
	*
	*/
	void decrease_degree (){degree--;}

	/**
	* Método que retorna o grau de um nó.
	* @return int grau do nó 
	*/
	int get_degree (){return degree;}

	/**
	* Método que retorna se um nó é ou não terminal
	*
	*/
	bool is_terminal (){return terminal;}

} node_t;

/**
* Classe utilizada para representar uma estrutura para 
* armazenar uma árvore de steiner.
*
* 
*
* @author Romerito C. Andrade
*/
class STTree {
	
public:

	/**
	* Construtor padrão.
	*/ 
	STTree () {}

	/**
	* Construtor que recebe apenas o número de nós
	* @param int número de nós
	*/
	STTree (int nnodes);

	/**
	* Construtor que recebe dois parâmetros.
	*
	* @param int número de nós
	* @param std::vector<int> conjunto de nós terminais
	*/
	STTree (int nnodes, const std::vector<int> & terminals);

	/**
	* Construtor que recebe três parâmetros.
	* @param int número de nós
	* @param int fonte do grupo multicast
	* @param std::vector<int> grupo de nós terminais
	*/
	STTree (int nnodes, int source ,const std::vector<int> & terminals) {
		_init_nodes (nnodes);
		_init_terminals(terminals);
		m_nodes[source].terminal = true;
		
		m_source = source;
		
		m_cost = 0.0;
		
		m_edges.clear ();
	}
	
	/**
	* Construtor de cópia
	*
	*/
	STTree (const STTree & ref);
	
	/**
	* Operador de atribuição 
	*
	*/
	STTree & operator= (const STTree & ref);
	
	/**
	* Destrutor
	*/
	~STTree () {
#ifdef DEBUG1
		printf ("STTree destroyed(%x)\n", this);
#endif
	}
	
	/**
	* Método que retoarn o núermo de nós
	* @return int número de nós
	*/
	inline int get_num_nodes () {return m_nodes.size();}

	/**
	* Método que retorna a fonte do grupo multicast
	* @return int nó fonte do grupo multicast
	*/
	inline int get_source () {return m_source;}

	/**
	* Método que retorna os nós terminais
	* @return std::vector<int> nós terminais
	*/
	std::vector<int> get_terminals () {
		std::vector<int> term;
		for (size_t i=0;i < m_nodes.size (); i++) {
			if (m_nodes[i].terminal)
				term.push_back (i);
		}
		return term;
	}
	
	/**
	* Método utilizado para adicionar uma aresta a árvore
	* de steiner.
	* @param int nó x
	* @param int nó y
	* @param double peso da aresta
	*/
	void add_edge (int x, int y, double value);

	/**
	* Método que remove uma aresta da árvore
	*
	* @param int nó x
	* @param int nó y
	*/
	void remove_edge (int x, int y);
	
	/**
	* Método que retorna um objeto do tipo node_t associado
	* a um nó.
	*
	* Recebe um parâmetro que corresponde a posição do nó na lista de nós.
	* A posição corresponde ao valor do nó.
	*
	* @param int posição do nó na lista de nós.
	* @return node_t 
	*/
	node_t & get_node (int pos) {
		return m_nodes[pos];
	}
	
	/**
	* Método que retorna o custo da árvore
	* @return double custo 
	*/
	inline double get_cost () {
		return m_cost;
	}
	
	/**
	* Método que define o custo de uma árvore
	* @param double	
	*/
	inline void set_cost (double cost) {
		m_cost = cost;
	}
	
	/**
	* Método que acessa a lista de edge_t da árvore
	* 
	* @return edge_t lista de arestas da árvore.
	*/
	edge_t * get_edge () const{
		return m_edges.begin;
	}
	
	/**
	* Método que retorna a lista de arestas da árvore.
	* @return list_edge
	*/
	list_edge & get_edges () {
		return m_edges;
	}
	
	/**
	* Meodo que retorna a lista de leaf's da árvore.
	* @return list_leafs_t
	*/
	list_leafs_t & get_leafs () {
		return m_leafs;
	}
	
	/**
	* Método para fazer prune da árvore e remover nós folhas não terminais.
	*
	*/
	void prunning ();
	
	/**
	* Método para imprimir a árvore no formato do GraphViz.
	*
	*/
	void xdot_format ();


	/**
	* Método que retorna todas as arestas presentes na árvore
	* 
	* Este método foi criado para manter compatibilidade com a representação 'steiner'
	*
	* As arestas são retornadas como um vector de std::pair
	*
	* @return std::vector<std::pair<int,int>> lista de arestas.
	*/	
	std::vector<std::pair<int,int>> get_all_edges () {
		std::vector<std::pair<int,int>>	edges;
		edge_t * perc = this->get_edge ();
		while (perc != NULL) {					
			std::pair<int,int> par = std::make_pair (perc->x, perc->y);
			if (perc->in)
				edges.push_back ( par );
			perc = perc->next;		
		}	

		return edges;
	}
	
private:
	void _init_nodes(int nnodes) {
		for (int i = 0; i < nnodes; i++) {
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