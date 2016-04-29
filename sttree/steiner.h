#ifndef STEINER_H
#define STEINER_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <deque>
#include <queue>

#include <map>

/**
* Este classe implementa a representação de uma árvore de Steiner utilizada
* um grafo. Este grafo é implementado utilizado uma lista de adjacência.
*
* Esta implementação permite realizar operações de modo mais eficiente, assim
* como permite usar menos memória.
*
* Algumas operações na estrutura da representação podem ser realizadas utilizando
* algoritmos em grafo que permite ótimo tempo de processamento.
*
*
* @author Romerito C. Andrade
*/
class steiner
{

public:
	steiner ();
	steiner (int numvertex);
	steiner (int numvertex, const std::vector<int> & terminals);
	steiner (int numvertex, int source, const std::vector<int> & terminals);
	steiner (const steiner &);
	steiner & operator=(const steiner &);
	~steiner();

	bool add_edge (int, int, double); //takes 2O(E)
	bool remove_edge (int,int); //takes 2O(E)
	bool find_edge (int,int); //takes 2O(E)

	const std::vector<int> & get_adjacent_vertices (int);

	void set_terminals (const std::vector<int> & terminals);
	const std::vector<int> get_terminals ();		
	bool is_terminal (int x);

	int get_num_edges ();
	int get_num_nodes ();
	double get_cost ();
	void set_cost (double cost);

	int get_degree (int x);

	std::vector<std::pair<int,int>> get_all_edges ();

	void print ();

	void print_test ();

	void prunning ();

	//return a path from source to member passed as parameter
	std::vector<int> get_path (int member, int source);

	

private:
	void get_edges (int node, 
		std::vector<std::pair<int,int>>& edges, 
		std::vector<bool> & marked)
	{

		marked[node] = true;
		for (auto adj : this->m_adjacentList[node]) {
			if (!marked[adj]) {
				edges.push_back (std::make_pair(node, adj));
				get_edges (adj, edges, marked);
			}
		}

	}

private:

	std::vector<std::vector<int>> m_adjacentList;
	std::vector<int> m_terminals;
	int m_edges;
	int m_nodes;
	double m_cost;


};



/**
* Class que encapsula o método prunning para remoção de arestas 
* que sejam folham e não terminal
*/
class Prune {

public:

	void prunning (steiner & tree);

	std::vector<std::pair<int,int>>::iterator begin () {
		return m_removed_edges.begin();
	}
	std::vector<std::pair<int,int>>::iterator end () {
		return m_removed_edges.end();
	}

private:

	std::vector<std::pair<int,int>> m_removed_edges;

};

/**
* Classe utilizada para computar os componentes conexos
* do grafo.
*
* A classe computa os componentes conexos e permite iterar
* sobre eles. Um std::map é utizado para armazenar os componentes
* conexos, permitindo criar e atualizar em tempo constante.
* Dois métodos - begin, end - são fornecidos para iteração sobre
* os componentes. O iterator retonardo é um iterator para um std::map<int, std::vector<int>>
* Assim, é possível obter o componentes e seu id.
* 
*
* @author Romerito C. Andrade
*/
class CC {

typedef typename std::map<int, std::vector<int>> MapComponents;

public:
	CC (steiner & tree) : m_count (0) {
		
		int NODES = tree.get_num_nodes ();
		m_marked = std::vector<bool> (NODES);
		m_id = std::vector<int> (NODES);
		m_size = std::vector<int> (NODES);
		// m_components.resize (NODES);

		for (int i : tree.get_terminals () ) {
			if (!this->m_marked[i]) {
				dfs (tree, i);
				this->m_count++;
			}
		}
	}

	int get_num_cc () {
		return this->m_count;
	}

	int get_id (int node) {
		return this->m_id[node];
	}

	int get_size (int node) {
		return this->m_size [this->get_id (node)];
	}

	MapComponents::iterator begin () {
		return this->m_components.begin();
	}

	MapComponents::iterator end () {
		return this->m_components.end();
	}

	void get_components_list (std::vector<std::vector<int>> & list) {
		for (auto iter = this->begin(); iter != this->end(); iter++) {
			list.push_back (iter->second);
		}
	}

private:
	//make depth-first search and get CC's
	void dfs (steiner & tree, int node) {
		this->m_marked[node] = true;
		this->m_id[node] = this->m_count;
		this->m_size[ this->m_count]++;

		this->m_components[this->m_count].push_back (node);

		for (int adj : tree.get_adjacent_vertices (node) ) {
			if (!this->m_marked[adj]) {
				this->dfs (tree, adj);
			}
		}
	}

private:
	std::vector<bool> m_marked;
	std::vector<int> m_id; //id do compoment contendo v
	std::vector<int> m_size; //m_size[]
	int m_count;

	MapComponents m_components;

};

#endif // STEINER_H