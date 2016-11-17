/*
 * network.h
 *
 *  Created on: 12/09/2012
 *      Author: romerito
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include "matrix2d.h"
#include "disjointset2.h"
#include "link.h"

using dl::matrix2d;
using std::cout;
using std::endl;

namespace rca {
	
struct EdgeRemoved {	
	
	EdgeRemoved () : removed(false), pos(-1){};
	EdgeRemoved (const EdgeRemoved & c) {removed = c.removed; pos = c.pos;
		//std::cout << "copy constructor" << std::endl;
	};
	
	bool removed;
	int pos;
};

/**
 * Esta classe representa um rede com custo e capaciade associados
 * as arestas. Utiliza as implementações de matrix2d implementada
 * por Dann Luciano.
 *
 * Além disso, esta classe fornece um recurso para remoção lógica
 * de links de uma rede. Isto é útil quando se está testando
 * algoritmos que removem links para computar diferentes caminhos.
 *
 * @date 11/04/2015
 * @author Romerito Campos
 * @version 1.0
 */
class Network {
public:

	/**
	 * Construtor padrão da classe Network.
	 */
	Network ();

	/**
	 * Construtor da classe Network que recebe parâmetros indicando
	 * o número de nós da rede, assim como o número de arestas.
	 *
	 * O construtor cria duas matrizes: uma para custo e outr para
	 * capacidade das arestas. Isto é feito invocando o método init()
	 *
	 * @param unsigned
	 * @param unsigned
	 */
	explicit Network (unsigned nodes, unsigned edges);
	~Network ();

	/**
	 * Operador que imprime as informações da rede na saída padrão
	 *
	 * @param ostream
	 * @param Network
	 */
	friend std::ostream& operator << (std::ostream& os, const Network & network);


	Network * extend (std::vector<int> const & sources = std::vector<int>());

	void addPseudoEdges (std::vector<int> & srcs) {

		int MAXINT = std::numeric_limits<int>::max ();
		int NODES = this->getNumberNodes () - 1;
		for (auto s : srcs) {
			rca::Link link (NODES, s, 0);

			this->setCost(link.getX(),link.getY(),0);
			this->setCost(link.getY(),link.getX(),0);		
			this->setBand(link.getX(),link.getY(),MAXINT);
			this->setBand(link.getY(),link.getX(),MAXINT);
			this->addAdjacentVertex(link.getX(), link.getY());
			this->addAdjacentVertex(link.getY(), link.getX());
		}

	}

	void removePseudoEdges (std::vector<int> & srcs) {
		

		int NODES = this->getNumberNodes () - 1;
		for (auto s : srcs) {
			rca::Link link (NODES, s, 0);

			this->setCost(link.getX(),link.getY(),0);
			this->setCost(link.getY(),link.getX(),0);		
			this->setBand(link.getX(),link.getY(),0);
			this->setBand(link.getY(),link.getX(),0);
			// this->addAdjacentVertex(link.getX(), link.getY());
			// this->addAdjacentVertex(link.getY(), link.getX());
			std::vector<int> & tmp = this->m_adjacent_vertex[link.getX()];
			tmp.pop_back ();
			tmp = this->m_adjacent_vertex[link.getY()];
			tmp.pop_back ();

		}
	}



	/**
	 * Método para definir um valor de custo de uma aresta.
	 *
	 * @param unsigned row
	 * @param unsigned col
	 * @param double value
	 */
	void setCost (unsigned row, unsigned col, double value);

	/**
	 * Método para definir um valor de capacidade de uma aresta.
	 *
	 * @param unsigned row
	 * @param unsigned col
	 * @param double value
	 */
	void setBand (unsigned row, unsigned col, double value);

	/**
	 * Método para retornar o custo de uma aresta.
	 *
	 * @param unsigned row
	 * @param unsigned col
	 * @return double
	 */
	double getCost (unsigned row, unsigned col) {
		return m_costMatrix.at (row, col);
	}

	double getCost (unsigned row, unsigned col) const{
		return m_costMatrix.at (row, col);
	}
	
	/**
	* Método utilizado para retonar o custo de uma aresta.
	* 
	* @param rca::Link
	* @return double
	*/
	double getCost (rca::Link l) {
		return this->getCost (l.getX(), l.getY());
	}

	double getCost (rca::Link l) const {
		return m_costMatrix.at (l.getX(), l.getY());
	}

	/**
	 * Método para retornar a capacidade de uma aresta.
	 *
	 * @param unsigned row
	 * @param unsigned col
	 * @return double
	 */
	double getBand (unsigned row, unsigned col) {
		return m_bandMatrix.at (row, col);
	}

	double getBand (unsigned row, unsigned col) const {
		return m_bandMatrix.at (row, col);
	}

	/**
	 * Método para retornar o número de nós de uma rede.
	 * @return int
	 */
	int getNumberNodes () {
		return m_nodes;
	}

	int getNumberNodes () const{
		return m_nodes;
	}

	/**
	 * Método para retornar o número de arestas de uma rede.
	 *
	 * @return int
	 */
	int getNumberEdges () {
		return m_edges;
	}

	/**
	 * Método usado para inserir arestas em um conjunto de Aretas
	 * O conjunto de arestas está ordenado pela capacidade.
	 *
	 * @param Link
	 */
	void insertLink (const Link & link) ;

	/**
	 * Método para remover uma aresta de forma lógica.
	 * A remoção é realizado por adicinoar a aresta a uma
	 * lista de removidos.
	 *
	 * @param Link.
	 */
	void removeEdge (const Link &);

	/**
	 * Método para remover uma lista de aresta.
	 * A lista de aresta deve ser passada como um std::set.
	 *
	 * @param std::set<Link>
	 */
	void removeEdge (const std::set<Link>&);

	/**
	 * Método para restaurar uma lista de arestas
	 * removidas.
	 * A lista de aresta de ser passada como um std::set.
	 *
	 * @param std::set<Link>
	 */
	void undoRemoveEdge (const std::set<Link>&);

	/**
	 * Método para desfazer uma remoção de aresta.
	 * Esta operação remove a aresta de lista de arestas removidas.
	 *
	 * @param Link.
	 */
	void undoRemoveEdge (const Link &);

	/**
	 * Método para remove todos os links associados a um vértice.
	 *
	 * @param int
	 */
	void removeEdgeByNode (int);

	/**
	 * Método para adicionar todos os nós associados a um vértice que
	 * tenha sido removidos.
	 *
	 * @param int
	 */
	void undoRemoveEdgeByNode (int);

	/**
	 * Método para verificar se uma aresta está removida logicamente.
	 *
	 * @param Link
	 * @return bool
	 */
	bool isRemoved (const Link &) const;

	/**
	 * Método para limpar a lista de arestas removidas. Remove
	 * todas as arestas que foram marcadas como removidas.
	 *
	 */
	void clearRemovedEdges () {
		
		auto it = m_removeds.begin ();
		for ( ; it != m_removeds.end (); it++) {
			
			m_removeds_edge[it->getX()][it->getY()].removed = false;
			m_removeds_edge[it->getX()][it->getY()].pos = -1;
			
		}
		m_removeds.clear();
		
	}

	/**
	 * Método para iniciar as estruturas de dados da rede.
	 * Recebe dois parâmetros: unsigned nodes que define o
	 * tamanho da matriz de nós e matriz de arestas. O segundo
	 * parâmetro informa o número de arestas.
	 * 
	 * @param int nodes
	 * @param int edges
	 * 
	 */
	void init (unsigned nodes, unsigned edges);

	/**
	 * Método retorna todas as arestas ligadas ao vértice node.
	 * 
	 * @param int
	 * @return std::vector<int>
	 */
	const std::vector<Link> & getEdges (int node) const;

	/**
	 * Este método imprime todos as arestas da rede exceto áquelas
	 * que foram marcadas como removidas.
	 * A impressão é feita seguindo o padrão: "x"-"y" : "cost"
	 *
	 */
	void print ();

	/**
	 * Método para verificar se a rede está desconectada.
	 * Utiliza uma busca em profundidade, caso um nó da rede esteja
	 * inascessível então retorna false, caso contrário a rede está
	 * conectada.
	 *
	 * @return bool
	 */
	bool isConnected ();


	/**
	* Método para verificar se os terminais estão conectados entre si.
	* Este Método permite que haja nós na rede que não estão conectados,
	* É necessário verificar apenas se os terminais estão conecetados.
	*/
	bool isConnected2 ();

	/**
	 * Método que retorna os links já ordenados.
	 * Os links são ordenados em ordem crescentes por
	 * valor de capacidade.
	 *
	 * @return set
	 */
	std::set <Link> & getLinks () {return m_links;}

	/**
	 * Método para mostrar as arestas que estão marcadas como
	 * removidas da rede.
	 */
	void showRemovedEdges ();
	
	/**
	 * Método verifica se o vértice pos está removido temporariamente
	 * 
	 * @param int
	 * @return bool
	 */
	inline bool isVertexRemoved (int pos) {return m_vertex[pos];}
	
	/**
	 * Método utilizado para remover um vértice da rede.
	 * Recebe como entrada a posição do vértice (id)
	 * Os vértices são numerados de 0 a n-1.
     *
	 * Remover o vértices 10, basta passar 10 com parâmetro
	 *
	 * @param int 
	 */
	void removeVertex (int pos) {m_vertex[pos] = true;}
	
	/**
	 * Restaura status de removido do vértice pos.
	 * 
	 * @param int
	 */
	void unremoveVertex (int pos) {m_vertex[pos] = false;}
	
	/**
	 * Marca todos os vértices como não removidos.
	 * 
	 */
	void clearRemovedVertex () {m_vertex = std::vector<bool>(m_nodes,false);}

	
	/**
	 * O objetivo deste método é alimentar uma lista
	 * de adjacência presente no grafo.
	 * Assim melhor a complexidade do algoritmo que
	 * necessitam procurar por vértices adjacentes a outros.
	 * 
	 * @param int vertex
	 * @param int vertex adjacent to vertex
	 */
	void addAdjacentVertex (int v, int adjacent) {
	    m_adjacent_vertex[v].push_back(adjacent);    
	}
	
	/**
	 * Este método é utilizado para se obter os iterators da lista
	 * de adjacentes de um determinado vertex.
	 * 
	 * @param int vertex
	 * @param std::pair<c_iterator,c_iterator>
	 */
	typedef typename std::vector<int>::const_iterator c_iterator;
	void get_iterator_adjacent(int vertex, std::pair<c_iterator,c_iterator> & _pair) 
	{
	  _pair = std::make_pair (m_adjacent_vertex[vertex].begin (),
				 m_adjacent_vertex[vertex].end());
	}

	void get_iterator_adjacent(int vertex, 
		std::pair<c_iterator,c_iterator> & _pair) const 
	{
	  _pair = std::make_pair (m_adjacent_vertex[vertex].begin (),
				 m_adjacent_vertex[vertex].end());
	}

	// typedef typename std::vector<int>::const_iterator c_iterator;
	// void get_iterator_adjacent(int vertex, std::pair<c_iterator,c_iterator> & _pair) 
	// {
	//   _pair = std::make_pair (m_adjacent_vertex[vertex].begin (),
	// 			 m_adjacent_vertex[vertex].end());
	// }
	
	/**
	* Método utilizado para retonar o vizinho imediato do
	* vértice passado como parâmetro considerando o menor
	* custo da aresta que os liga.
	* 
	* @param int vertex
	* @return int 
	*/
	const int get_adjacent_by_minimun_cost (const int & vertex) ;
	
	/**
	* Método retorna o vértice mais próximo do vértice
	* passado como parâmetro. A métrica é o custo da areata.
	*
	* Considera um conjunto de areastas removidas do grafo
	* que são passadas como parâmetro.
	*
	* @param int
	* @param std::vector<rca::Link>
	* @return int
	*/
	const int get_adjacent_by_minimun_cost (const int & vertex, 
											std::vector<rca::Link>&) ;

	std::vector<int> get_neighboors (int const& vertex) {
		return m_adjacent_vertex[vertex];
	}
	
	const std::vector<Link> & getRemovedEdges () {
		return m_removeds;
	}

	
	std::vector<int> breadth_first_search (int start);

	
private:
	int m_nodes;
	int m_edges;
	matrix2d m_costMatrix;
	matrix2d m_bandMatrix;
	
	std::vector<bool> m_vertex;

	std::set < Link > m_links;
	std::vector < Link > m_removeds;
	
	//this varibles holds the vertex as adjacent list
	std::vector<std::vector<int>> m_adjacent_vertex;
	
	std::vector<std::vector<EdgeRemoved>> m_removeds_edge;

public:
	std::vector< std::vector<Link> > m_edgesByNodes;

};


class AdjacentNetwork {

public:
	
	AdjacentNetwork (int nodes, std::vector<Link>& links) {
		
		m_adjacent_vertex = std::vector<std::vector<int>>(nodes);
		m_nodes = nodes;
		//considerando que não links repetidos
		for (auto l : links) {
			this->addEdge (l.getX(), l.getY());
		}
	}
	
	int getNumberNodes () {
		return m_nodes;
	}
	
	/**
	 * Método para adicionar uma aresta.
	 * 
	 * @param int vértice i
	 * @param int vértice j
	 */
	inline void addEdge (int i, int j) {
		m_adjacent_vertex[i].push_back (j);
		m_adjacent_vertex[j].push_back (i);
	}
	
	void removeEdge (int i, int j);
	
	/**
	 * Verifica se uma aresta exisite.
	 * 
	 * @param int vértice i
	 * @param int vértice j
	 */
	inline bool hasEdge (int i, int j) {
		return std::find (m_adjacent_vertex[i].begin(), 
						  m_adjacent_vertex[i].end(), j) !=
						  m_adjacent_vertex[i].end();
	}
	
	/**
	 * Retorna lista de vértices saíndo de i.
	 * 
	 * @param int vértice i
	 */
	std::vector<int> outEdge (int i) {
		return m_adjacent_vertex[i];
	}
	
private:
	
	int m_nodes;
	int m_vertex;
	std::vector<std::vector<int>> m_adjacent_vertex;
	
};

} /* namespace rca */
#endif /* NETWORK_H_ */
