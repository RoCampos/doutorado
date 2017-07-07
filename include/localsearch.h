#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include <iostream>
#include <vector>
#include <utility>
#include <typeinfo>
#include <numeric>

#include "edgecontainer.h"
#include "network.h"
#include "steiner.h"
#include "link.h"
#include "group.h"
#include "algorithm.h" //get_shortest_path

enum EdgeType{
	IN = 0,
	OUT = 1
};

namespace rca {

	namespace sttalgo {


class LocalSearch {

typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> Container;
typedef typename rca::Network Network;
typedef typename std::vector<rca::Group> VGroups;
typedef typename std::vector<steiner> Solution;
typedef typename std::vector<rca::Link> VEdge;

public:
	LocalSearch (Network & net, std::vector<rca::Group> & groups);	
	LocalSearch (Network & net, std::vector<rca::Group> & groups, Container &);	
	
	void apply (Solution &, int& cost, int& res);

	~LocalSearch () {
		m_cg = NULL;
	}

	void clear_removed () {
		this->m_removed.clear ();
	}

	std::vector<rca::Link> get_removed () {
		return this->m_removed;
	}

private:

	/**
	*	
	* @param int tree_id
	* @param steiner árvore a ser modificada
	* @param int custo da árvore
	*/
	bool cut_replace (int x, int y, int id, steiner & tree, int& solcost);

	void inline_replace (steiner &, int&, 
		rca::Link& out, 
		rca::Link& in, 
		int tree_id);

	void update_container (Solution & solution);

	void inline_update (rca::Link&, EdgeType, int tree_id);

private:
	Container * m_cg;
	Network * m_network;
	std::vector<rca::Group> * m_groups;

	VEdge to_replace;
	VEdge m_removed;

	//the index represents the trees
	VEdge m_out;
	VEdge m_in;
	std::vector<int> m_id;

	int m_top;

};


class CycleLocalSearch {

typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> Container;
typedef typename rca::Network Network;
typedef typename std::vector<rca::Group> VGroups;
typedef typename std::vector<steiner> Solution;
typedef typename std::vector<rca::Link> VEdge;

public:

	CycleLocalSearch (Network & net, std::vector<rca::Group> & groups);
	CycleLocalSearch (Network & net, std::vector<rca::Group> & groups, Container &);

	void apply (Solution & solution, int & cost, int & res);

	~CycleLocalSearch ();

private:

	void update_container (Solution &);
	void get_vertex (std::vector<int> &, steiner &);

	bool cut_replace (int , std::vector<int>& , steiner & st, int& );
	
	void inline_replace (steiner &, int&, 
		rca::Link& out, 
		rca::Link& in, 
		int tree_id);

	void inline_update (rca::Link&, EdgeType, int tree_id);

	rca::Link get_out_link (rca::Path&, steiner &);

	rca::Path get_path (int x, int y, steiner & st) {

		path.clear ();
		m_marked.clear ();		

		rca::Link l (x,y,0);

		m_finished = false;		
		path = std::vector<int> (m_network->getNumberNodes (), -1);
		m_marked = std::vector<bool> (m_network->getNumberNodes ());
		m_marked[l.getX()] = true;

		dfs (l.getX(), l.getY(), st);

		return get_shortest_path (l.getX(), l.getY(), *m_network, this->path);

	}
	void dfs (int x, int y, steiner& st) {
		if (x == y) {			
			m_finished = true;
			return;
		}
		m_marked[x] = true;
		for (int adj : st.get_adjacent_vertices(x)) {
			if (!m_marked[adj] && !m_finished ) {				
				path[adj] = x;
				dfs (adj, y, st);
			}
		}

	}

private:

	// ----- for local search ---- 
	std::vector<bool> m_marked;
	bool m_finished = false;
	std::vector<int> path;

	Container * m_cg;
	Network * m_network;
	std::vector<rca::Group> * m_groups;

	VEdge to_replace;

	VEdge m_removed;

	int m_top;

	static int contador;

};

/**
* 
* Esta classe é utilizada para fazer a troca de arestas congestionadas
* por outras arestas que possam diminiuir a congestão na rede, contato que
* não se viole a restrição de tamanho aplicada aos caminhos.
*
* @author Romerito Campos
* @date 15/05/2016
*/
template<class Container, class SteinerRpr>
class PathExchange {
	typedef typename std::vector<std::pair<rca::Link, rca::Link>> Trocas;
	typedef typename std::vector<rca::Path> Paths;	
	

public:
	PathExchange(int limit) : m_path_size(limit) {}

	/**
	* Método utilizado para rodar a busca local que viza a melhoria
	* da congestão sem violar o tamanho dos caminhos.
	*
	*
	* @param vector<SteinerRpr> solução do problema
	* @param Container container de arestas
	* @param rca::Network objeto que modela a rede do problema
	* @param rca::Group objeto com informações dos grupos multicas
	*/
	bool run (std::vector<SteinerRpr>&, Container&, 
		rca::Network&, std::vector<rca::Group>&);

private:

	/**
	* Método que retorna os componentes conexos após a eliminição
	* de um arestas congestionada.
	*
	*
	* @param SteinerRpr árvore de steiner
	* @return vector<vector<int>> componets conexos
	*/
	std::vector<std::vector<int>> get_components (SteinerRpr& st);
	
	/**
	*
	* Método que verifica se os caminhos da árvore satisfazem o limite aplicado.
	*
	* O parâmetro c1 representa o nó da parte da árvore que contém a fonte.
	* O parâmetro c2 representa o nó da parte da arvore que não contém a fonte e
	* será reconecado a parte da árvore que contém a fonte.
	*
	* A conexão, se for possível será realizad por (c1, c2)
	*
	* O parâmetro Paths representa os caminhos existentes na arvore para cada 
	* no de demanda.
	*
	* O parâmetro distancia é utilizado para guardar a distância de cada nó para
	* o nó fonte.
	* 
	* @param int c1
	* @param int c2
	* @param Paths
	* @param vector<int>
	*
	* @return bool true quando é possível reconectar com (c1,c2)
	*/
	bool test_pathsize (int c1, int c2, 
		Paths &, std::vector<int> distancia);


	/**
	* Método que controla as trocas realizadas para evitar que
	* Um loop infinito se instale.
	*
	* Evita trocas cíclicas entre pares de links.
	*
	* @param rca::Link 
	* @param rca::Link
	*/
	bool control_trocas (rca::Link&, rca::Link&);

	/**
	* Método que encontra os caminhos de cada nó de destino para o nó fonte.
	*
	* O método calcula a distância entre fonte/destino.
	*
	* @param SteinerRpr árvore de steiner
	* @param Paths repositório dos caminhos
	* @param vector<int> repositório das distâcias
	* @param vector<int> nós de destino
	* @param int fonte
	*/
	void paths_distance (SteinerRpr&, 
		Paths&, 
		std::vector<int>& distancia, 
		std::vector<int>& members, 
		int source);


	/**
	* Método que faz a troca das arestas.
	*
	* @param SteinerRpr Árvore de Steiner
	* @param Container container de arestas
	* @param rca::Network objecto que representa a rede de problema
	* @param rca::Link aresta que será modificada
	* @param rca::Group grupo multicast
	*/
	bool core_search (SteinerRpr & st, 
		Container & container, 
		rca::Network& network, 
		rca::Link& old, 
		rca::Group& g);

	/**
	* Método que verfica se um novo link pode ser utilizado para substituir
	* um link congestinado.
	*
	* Dois testes são realizados: primeiro, verifica-se se os links não são iguais e
	* segundo, verifica-se se há banda suficiente no novo link.
	*
	* @param rca::Link novo link
	* @param rca::Link velho link
	* @param Container repositório que contra o uso das arestas
	* @param int requição de tráfego
	* @param int banda do novo link
	*/
	bool tocontinue (rca::Link& novo, rca::Link& old,
		Container& cg, int tk, int band) {
		return (novo != old) && cg.test_bandwitdh (novo, band, tk);		
	}

	/**
	* Método utilizado para checar se há possibilidade de execução
	* da mudança de arestas.
	*
	* O parâmetros c1 e c2 são os nós que vão conectar as duas partes da
	* árvore.
	*
	* O método check_change utiliza os métodos prepare_paths e test_pathsize.
	*
	* @param int c1
	* @param int c2
	* @param rca::Link aresta que sai
	* @param Paths caminhos da árvore
	* @param vector<int> lista de distâncias
	*/ 
	bool check_change (std::vector<std::vector<int>>&,
		int c1, int c2, int source, 
		rca::Link & old,
		Paths& paths, 
		std::vector<int>& distancia);

	/**
	* Método que aplica a mudança.
	*
	* Este método é de fato onde ocorre a mudança de arestas e a atualização
	* do Container e da árvore de Steiner em questão.
	*
	* @param SteinerRpr árvore de Steiner a ser modificada
	* @param rca::Link novo link a ser inserido na árvore
	* @param rca::Link old link que será removido da árvore
	* @param rca::Network objeto que representa a rede do problema
	* @param rca::Group grupo multicast do problema
	*/
	bool do_change (SteinerRpr&, Container&, 
		rca::Link& novo, rca::Link& old, 
		rca::Network&, rca::Group&);

	/**
	* Rotina autilizar para fazer pesquisa de um nó dentro de uma
	* lista.
	*
	* @param vector<int> lista de inteiros
	* @param int elemento a ser procurado
	*/
	bool find (std::vector<int>& lista, int node) {
		auto res = std::find (lista.begin (), lista.end(), node);
		return res != lista.end ();
	}

	//connector is the node that connect the subtree of node1 into
	//	subtree of connector.
	//node1 was the node connecting the subtree into the tree with 
	//the source
	//node2 is the new connector
	Paths prepare_paths (Paths&, int connector, int node1, int  node2);


private:
	Trocas m_trocas;

	int m_path_size;


};


} // sttalgo

} // rca

#endif // LOCALSEARCH_H


