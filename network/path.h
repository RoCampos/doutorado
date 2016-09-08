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
	* Este método é utilizado para reverter a ordem do caminho.
	*
	*/
	void reverse () {
		std::reverse (m_path.begin(), m_path.end());
	}

	/**
	* Este método é utilizado para associar um valor a uma 
	* dada posição
	*
	* @param int posição
	* @param int value a ser associado a posição pos
	*/
	void assign (unsigned int const pos, int const value) {
		m_path[pos] = value;
	}

	/**
	*
	* Método utilizad para fazer um caminho receber outro.
	* Basicamente é uma cópia de um novo caminho.
	* 
	* @param rca::Path 
	*/
	void assign (rca::Path const&path) {
		m_path = path.m_path;
	}
	

	/**
	* Método para adicionar um caminho a outro.
	* Evita que caminhos tenha nós repetidos:
	* 	caminho1: 0 1 2 3, caminho2: 3 2 1 3 4
	* Resultado: 0 1 2 4
	* 
	* @param rca::Path 
	*/
	void join (Path const & path) {		
		for (auto && v : path.m_path){
			auto begin = this->m_path.begin ();
			auto end = this->m_path.end ();
			if (std::find (begin, end, v) == end) {
				this->m_path.push_back (v);
			}
		}
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

 	/**
 	*	This function is used to get a reverse path
 	*	from the end of the path to node 'vertex' 
 	*	without consider the node 'vertex'
 	*
 	*	Consider the path 1-->2-->23 and the call 'revsubpath(23)'
 	*	the return is 1-->2.
 	*
 	*	The subpath is stored in the vector passed as paramater.
 	*	
 	*	@param vertex
 	*	@param std::vector<int> 
 	*/
 	bool revsubpath (int vertex, std::vector<int>&);


 	/**
 	*	This function returns the position of vertex
 	*	in the way it is stored in the internal structure
 	*	For example, the 1-->2-->23 is stored as [23,2,1]
 	*
 	*	So a cal to getPosition (23) returns 0.
 	*	@param int
 	*	@param int position
 	*/
 	int getPosition (int vertex);

 	/**
 	*	This function returns the position of the vertex
 	*	based on the representation. So if the path 1-->2-->23
 	*	is stored as [23,2,1]. 
 	*
 	*	The correct position of the vertex 23 is '3'. Considering
 	*	that the index start at 1.
 	*
 	*	@param int
 	*	@param int position
 	*/
 	int getRevPosition (int vertex);

	
private:
	std::vector<int> m_path;
	double m_cost;

};

} /* namespace rca */
#endif /* PATH_H_ */
