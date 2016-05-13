/*
 * path.h
 *
 *  Created on: 13/09/2012
 *      Author: romerito
 */

#ifndef PATH_H_
#define PATH_H_

#include <vector>
#include <ostream>
#include <iterator>
#include <algorithm>

namespace rca {

/**
 * Esta classe é usada para representar um caminho.
 * O caminho é um conjunto de inteiros armazendos
 * em vector que está ordenado de acordo com o caminho.
 *
 * @date 27/09/2012
 * @author Romerito Campos
 */
class Path {
public:
	/**
	 * Construtor padrão
	 */
	Path();
	~Path();

	Path (std::vector<int> const& path) {
		m_path = std::move (path);
	}

	/**
	 * Adicionar um vértice ao caminho.
	 * @param int
	 */
	void push (int);

	bool find_in (int node) {
		return (std::find (m_path.begin(), m_path.end(), node) != m_path.end());
	}

	/**
	 * Método para acessar uma posição do caminho como se faz
	 * com um vector;
	 */
	const int & operator[] (unsigned int pos) {
		return m_path[pos];
	}
	
	/**
	 * Método retorna o vértice que está na posição
	 * pos do caminho.
	 * @param int
	 * @return int
	 */
	int at (unsigned int pos) {
		return m_path[pos];
	}
	
	/**
	 * Método retorna o vértice que está na posição
	 * pos do caminho (const).
	 */
	const int at (unsigned int pos) const {
		return m_path[pos];
	}

	/**
	 * Método para imprimir o caminho na saida padrão.
	 * O formato da saída é o seguinte:
	 *  [ 0, 1, 2, 3 ] para indicar o caminho 0->1->2->3
	 *  @param ostream
	 *  @param Path
	 */
	friend std::ostream& operator << (std::ostream& os, const Path & path);

	/**
	 * Método para retornar o tamanho do caminho. Neste caso,
	 * o número de nós.
	 */
	unsigned int size () const {return m_path.size();}

	/**
	 * Retornar um iterator para o primeiro nó do caminho.
	 * Esta função é útil caso seja necessário iterar sobre o caminho
	 *
	 * @return iterator
	 */
	std::vector<int>::iterator begin () {return m_path.begin();}
	
	/**
	 * Retonar um const_iterator para a primeira posição do caminho.
	 * @return std::vector<int>::const_iterator
	 */
	std::vector<int>::const_iterator cbegin () {return m_path.begin();}
	

	/**
	 * Retornar um iterator para o último nó do caminho.
	 * Esta função é útil caso seja necessário iterar sobre o caminho
	 *
	 * @return iterator
	 */
	std::vector<int>::iterator end () {return m_path.end();}
	
	/**
	 * Retonar um const_iterator para a última posição do caminho.
	 * @return std::vector<int>::const_iterator
	 */
	std::vector<int>::const_iterator cend () {return m_path.end();}
	
	/**
	 * Retornar um iterator reverso para o primeiro nó do caminho.
	 * Neste caso, o primeiro nó será o último
	 * Esta função é útil caso seja necessário iterar sobre o caminho
	 *
	 * @return iterator
	 */
	std::vector<int>::reverse_iterator rbegin () {return m_path.rbegin();}

	/**
	 * Retornar um iterator reverso para o último nó do caminho.
	 * Neste caso, o último nó será o primeiro.
	 * Esta função é útil caso seja necessário iterar sobre o caminho
	 *
	 * @return iterator
	 */
	std::vector<int>::reverse_iterator rend () {return m_path.rend();}
	
	/**
	 * Subcaminho até a posição pos é extraído do caminho atual.
	 *
	 * @param int
	 * @param std::vector<int>
	 * @return bool
	 */
	bool subpath (int pos, std::vector<int> & path);
	
	/**
	 * Retorna o custo do caminho.
	 * @return double
	 */
	inline double getCost () const {return m_cost;}
	
	/**
	 * Define o custo do caminho.
	 * @param double
	 */
	inline void setCost (double cost) {m_cost = cost;}
	
	/**
	 * Operador de comparação <.
	 * @param Path
	 * @return bool
	 */
	bool operator< (const Path& path){
		return m_cost < path.getCost ();
 	}
	
	/**
	*	
	*
	* @param Path
	* @return bool
	*/
	const bool operator< (const Path& path) const{
		return m_cost < path.getCost ();
	}
 	
 	/**
	 * Operador de comparação ==.
	 * @param Path
	 * @return bool
	 */
 	bool operator== (const Path& path);
	
private:
	std::vector<int> m_path;
	double m_cost;

};

} /* namespace rca */
#endif /* PATH_H_ */
