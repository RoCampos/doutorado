/*
 * link.h
 *
 *  Created on: 13/09/2012
 *      Author: romerito
 */

#ifndef LINK_H_
#define LINK_H_

#include <ostream>

namespace rca {

template<class T>
struct not_equal_link{
  bool operator() (const T& l1, const T& l2) const
  {
	  //return ((p < link.p) && (link != *this)) || (link != *this);
	  return (l1 < l2 && l1 != l2) || (l1 != l2);
  }
};

/**
 * Esta classe representa uma aresta.
 * Ela possui três atributos: dois vértices (int) e um peso
 * associado (double).
 *
 * A um mecanismo utilizada para facilitar a comparação de Link
 * e outras operações. Os vértices são nomeados como x e y na classe
 * então dado dois vértices que formam uma aresta o vértices com maior
 * valor fica no variável x e o outro na variável y.
 *
 * @date 27/09/2012
 * @author Romerito Campos
 */
class Link {
public:
	/**
	 * Construtor padrão.
	 */
	Link();

	/**
	 * Construtor com três argumentos.
	 * Dois inteiros indicando os vértices que forma a aresta
	 * e um peso do tipo double.
	 * @param int
	 * @param int
	 * @param double
	 */
	Link(int,int,double);

	/**
	 * Construtor de cópia.
	 * Recebe um objetivo do tipo Link e
	 * realiza uma cópia de seus valores.
	 *
	 * @param Link
	 */
	Link(const Link&);
	~Link();

	/**
	 * Operador== para verificar se dois Objetos do
	 * do tipo Link são iguais ou não. A igualdade é
	 * verificada apenas testando se os vértices são iguais,
	 * neste caso o peso é ignorado.
	 *
	 * @param Link
	 */
	bool operator==(const Link &) const;

	/**
	 * Operador!= verifica se dois objetivos do tipo
	 * Link são diferentes. O teste é realizado
	 * verificando se um há diferenças entre os vértices
	 * dos objetos. O peso também é ignorado.
	 *
	 * @param Link
	 */
	bool operator!=(const Link &) const;

	/**
	 * Este operador verifica se um Link tem um peso
	 * menor que outro.
	 * Este operador é util em alguns conteiner como o std::set
	 *
	 * @param Link
	 */
	bool operator<(const Link &) const;
	
	/**
	 * Este operador verifica se um Link tem um peso
	 * maior que outro.
	 * Este operador é util em alguns conteiner como o std::set
	 *
	 * @param Link
	 */
	bool operator>(const Link &) const;

	/**
	 * Operador de atribuição.
	 * Realiza atribuição de um objeto do tipo Link
	 * a outro objeto.
	 *
	 * @param Link
	 */
	Link & operator=(const Link &);

	/**
	 * Operador para impressão de um Link na saída padrão.
	 * A saída é no formato "x"--"y". Este tipo de saída é útil
	 * para uso com o Graphviz caso haja necessidade criar uma imagem de
	 * uma rede.
	 *
	 * @param std::ostream
	 * @param Link
	 */
	friend std::ostream & operator<< (std::ostream & out, const Link & obj);

	/**
	 * Método set para definir o valor de X.
	 * @param int
	 */
	void setX (int _v){x = _v;}

	/**
	 * Método set para definir o valor de Y
	 * @param int
	 */
	void setY (int _w){y = _w;}

	/**
	 * Método get para retornar o valor de X
	 * @return int
	 */
	inline const int & getX () const {return x;}

	/**
	 * Método get para retornar o valor de Y
	 * @return int
	 */
	inline const int & getY () const {return y;}

	/**
	 * Método get para retornar o peso do link.
	 * @return double
	 */
	inline const double & getValue () const {return p;}

	/**
	 * Método set para definir o valor de peso do Link
	 * @param double
	 */
	void setValue (double v){p = v;}

private:
	int x, y;
	double p;
};

/**
 * Está structu é usada para mapear um Link a uma Árvore de Steiner.
 * Ela consiste de um Link e um inteiro indicado um identificador de
 * árvore de Steiner. Este identificador é referente ao grupo multicast
 * que está sendo tratado.
 *
 *
 * @see SteinerVector
 * @date 27/09/2012
 * @author Romrito Campos
 */
struct TreeLink {

	TreeLink (const Link & link, int tree) : m_tree(tree)
	{
		m_link = link;
	}
	bool operator== (const TreeLink & obj) const {
		return obj.m_link == m_link && obj.m_tree == m_tree;
	}

	bool operator!= (const TreeLink & obj) const {
		return obj.m_link != m_link || obj.m_tree != m_tree;
	}

	bool operator< (const TreeLink & treeLInk) const {
		return m_tree < treeLInk.m_tree;
	}

	Link m_link;
	int m_tree;

};


} /* namespace rca */
#endif /* LINK_H_ */
