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

/**
 * Esta classe representa um rede com custo e capaciade associados
 * as arestas. Utiliza as implementações de matrix2d implementada
 * por Dann Luciano.
 *
 * Além disso, esta classe fornece um recurso para remoção lógica
 * de links de uma rede. Isto é útil quando se está testando
 * algoritmos que removem links para computar diferentes caminhos.
 *
 * @date 27/07/2014
 * @author Romerito Campos
 * @version 0.2
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

	/**
	 * Método para retornar o número de nós de uma rede.
	 * @return int
	 */
	int getNumberNodes () {
		return m_nodes;
	}

	/**
	 * Método para retornar o número de arestas de uma rede.
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
	void clearRemovedEdges () {m_removeds.clear();}

	/**
	 * Método para iniciar as estruturas de dados da rede.
	 * Recebe dois parâmetros: unsigned nodes que define o
	 * tamanho da matriz de nós e matriz de arestas. O segundo
	 * parâmetro informa o número de arestas.
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
	 * Return true if a vertex is temporary removed from the 
	 * network.
	 */
	void removeVertex (int pos) {m_vertex[pos] = true;}
	
	/**
	 * Restaura status de não removido do vértice pos.
	 * 
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
	 * Método para retonar um iterator para a primeira posição
	 * dos nós adjacents de vertex.
	 * @param int vertex
	 */
	const std::vector<int>::const_iterator adjacent_begin (int vertex) const{
	    return m_adjacent_vertex[vertex].begin ();
	}
	
	/**
	 * Método para retonar um iterator para a última posição na 
	 * lista de adjacents de vertex.
	 * @param int vertex
	 */
	const std::vector<int>::const_iterator adjacent_end (int vertex) const{
	    return m_adjacent_vertex[vertex].end ();
	}
	
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

public:
	std::vector< std::vector<Link> > m_edgesByNodes;

};

} /* namespace rca */
#endif /* NETWORK_H_ */
