#include <iostream>

#include "network.h"
#include "group.h"
#include "algorithm.h"
#include "steinertree.h"

#include <boost/heap/fibonacci_heap.hpp>

namespace rca {

#ifndef _EDGE_CONTAINER_
#define _EDGE_CONTAINER_

//struct to compare two edges in fibonnaci heap
template<class T>
struct not_equal{
  bool operator() (const T& l1, const T& l2) const
  {
	  //return ((p < link.p) && (link != *this)) || (link != *this);
	  return (l1 > l2);
  }
};

template<class T>
struct not_equal_rev{
  bool operator() (const T& l1, const T& l2) const
  {
	  //return ((p < link.p) && (link != *this)) || (link != *this);
	  return (l1 < l2);
  }
};

/**
* Tipo de operação de atualização container.
*/
enum OperationType{
	IN = 0,
	OUT = 1
};
	
//Matriz de inteiros
typedef typename std::vector<std::vector<int>> VMatrix;

//types used to control the heap_fibonnacti during algorithm execution
//Comparador normal. Ordem crescente no acesso. Min Heap
typedef typename boost::heap::compare<not_equal<rca::Link>> Comparator;

//comparador reverso. Para ordenar em ordem decrescente. Max-Heap
typedef typename boost::heap::compare<not_equal_rev<rca::Link>> ComparatorReverse;

//handle da aresta para min-heap. Permite que a aresta seja acessada e manipulada dentro
//do heap
typedef typename boost::heap::fibonacci_heap<rca::Link, Comparator>::handle_type edge_handle;
typedef typename boost::heap::fibonacci_heap<rca::Link, ComparatorReverse>::handle_type edge_handle_rev;

//typedef typename boost::heap::fibonacci_heap<rca::Link, Comparator> FibonnacciHeap;


//define um pair bool e um manipulador de aresta
//este manipulador indica se uma aresta está ou no heap
typedef typename std::pair<bool, edge_handle> HCell;
typedef typename std::pair<bool, edge_handle_rev> HCellRev;

//vetor de manipuladores de arestas
typedef typename std::vector<std::vector< HCell >> EHandleMatrix;


/**
 * Esta classe é utilizada para manter informações sobre
 * o nível de utilização das arestas. 
 * 
 * Ela mantém um heap de fibonacci mínimo que é utilizado para
 * guardar as arestas utilizadas na construção de uma solução.
 * 
 * Quando for necessário utilizar uma arestas mais de uma vez
 * a aresta que está heap é adicionada novamente a rede.
 *
 * O primeiro argumento do template indica o comparador utilizado.
 * Por padrão, utiliza-se Comparador que fornece um heap-min.
 * O segundo argumento do template define o manipulador de arestas.
 * Por padrão, utiliza-se HCell.
 *
 * Para se utilizar heap-max basta usar os argumentos: ComparatorRev e HCellRev
 * 
 * @author Romerito Campos.
 */
template <class Comp = Comparator, class Handle = HCell>
class EdgeContainer {
	
public:
	
	/**
	* Construtor padrão.
	*
	*
	*/
	EdgeContainer<Comp, Handle> (){}
	
	/**
	* Construtor de cópia.
	* 
	* @param EdgeContainer<Comp, Handle> Referência
	* 
	*/
	EdgeContainer<Comp, Handle> (EdgeContainer<Comp, Handle> const & ref){
		
		m_ehandle_matrix = ref.m_ehandle_matrix;
		m_matrix = ref.m_matrix;
		m_heap = ref.m_heap;
		
	}

	/**
	* Construtor que inicializa a matriz que guardar as informações
	* dos manipuladores.
	*
	* @param int número de nós da rede
	*/
	EdgeContainer<Comp, Handle> (int NODES) {
		this->init_congestion_matrix (NODES);
		this->init_handle_matrix (NODES);
	}
	
	/**
	* Operator de atribuição.
	*
	*
	* @param EdgeContainer<Comp, Handle>
	* @return EdgeContainer<Comp, Handle>
	*/
	EdgeContainer<Comp, Handle> operator=(EdgeContainer<Comp, Handle> const & ref){
		
		m_ehandle_matrix = ref.m_ehandle_matrix;
		m_matrix = ref.m_matrix;
		m_heap = ref.m_heap;
		
		return *this;
	}
	
	/**
	* Método para inicializar matrix que guarda informações
	* sobre handles de cada arestas.
	* 
	* A matrix possui ordem quadrada. O tamanho da matriz é
	* nodes x nodes.
	*
	* @param nodes
	*/
	void init_congestion_matrix (int nodes);
	
	/**
	* Método utiliza para iniciar a matriz de manipuladores.
	* Estes manipuladores permitem que se controle o uso das
	* arestas com boa complexidade de tempo.
	* 
	* Um Handle é um par de bool e um manipulador do heap de fibonacci
	* fornecido pela boost graph library.
	*
	* Para cada arestas adicionada ao heap de fibonacci um handle é mantido
	* na lista matrix de handles.
	*
	* Assim, é possível ter acesso constante a qualquer handle durante a 
	* utilização do container de arestas.
	* 
	* A matriz de manipuladores é de ordem nodes x nodes. 
	*
	* @param int número de nós
	*/
	void init_handle_matrix (int nodes);
	
	//used to prepare the nettwork to build the next tree
	//verify by connectivty between terminals
	int connected_level (rca::Group & group, rca::Network & Network);
	
	
	int connected_level_by_group (rca::Group & group, rca::Network &);
	
	/*acessor methods*/
	/**
	* Método utilizado para verificar se uma aresta já foi utilizada.
	* 
	* Utilizada neste contexto refere-se ao fato de ele ter sua capacidade
	* modificada devido estar fazendo parte de uma árvore multicast que compõe 
	* uma solução.
	*
	* @param rca::Link
	*/
	bool is_used (rca::Link &l) {
		return m_ehandle_matrix[ l.getX()][ l.getY() ].first;
	}
	
	//verifica se está sendo utilizado
	/**
	* Método utilizado para verificar se uma aresta já foi utilizada.
	* 
	* Utilizada neste contexto refere-se ao fato de ele ter sua capacidade
	* modificada devido estar fazendo parte de uma árvore multicast que compõe 
	* uma solução.
	*
	* @param int vértice
	* @param int vértice
	*/
	bool is_used (int x, int y) {
		return m_ehandle_matrix[x][y].first;
	}
	
	//retorna o min ou max do heap (Comparator ou ComparatorReverse)
	/**
	* Retorna o valor min( ou máximo) do heap.
	* Neste caso, será o valor mínimo dentre todas as arestas.
	* Valor mínimo é a capacidade residual ou congestão, a depender da 
	* utilização.
	*
	* @return int valor mínimo ou máximo.
	*/
	int top () {
		return m_heap.ordered_begin ()->getValue ();
	}
	
	/**
	* Retorna o valor de capacidade residual ou congestão para uma aresta
	* específica.
	*
	* Recebe como parâmetro uma referência para um objeto do tipo @see rca::Link
	*
	* @param rca::Link
	* @return int
	*/
	int value (rca::Link &l) {
		return (*(m_ehandle_matrix[ l.getX() ][ l.getY() ]).second).getValue ();
	}

	void update_inline (rca::Link & link, 
		OperationType type, int trequest, int band) 
	{

		int value;
		if (OperationType::OUT == type) {		
			value = (this->value (link) + trequest);
		} else if (OperationType::IN == type) {		
			if (this->is_used (link)) {
				value = (this->value (link) - trequest);
			} else {
				value = (band - trequest);
			} 
		}

		if (value ==  band) {
			this->erase (link);
		} else {
			if (this->is_used (link)) {
				this->erase (link);
				link.setValue (value);
			} else {
				link.setValue (value);
			}
			this->push (link);
		}

	}

	bool test_bandwitdh (rca::Link & novo, int band, int tk) {

		bool activated = false;		
		int top = this->top ();

		if (this->is_used (novo)) {
			if (this->value (novo) - tk > top) {
				activated=true;
			}
		} else if (band - tk > top ) {
			activated=true;
		}

		return activated;
	}

	/**
	* Método utilizado para atualizar a utilização de um link..
	* O link deve ser configurado para o novo valor fora do container e
	* passado para o método com o novo valor.
	* 
	*
	* @param rca::Link
	*/
	void update (const rca::Link & l) {
		
		m_heap.update (m_ehandle_matrix[l.getX()][l.getY()].second, l);
		
	}
	
	/**
	* Método utilizado para aumentar o valor de um link. 
	* Este método modifica o valor do link e altera o seu estado no container.
	*
	* Recebe como entrada  o @see rca::Link l e um inteiro com o valor a ser
	* utilizado.
	*
	* @param rca::Link
	* @param int novo valor que aumenta o valor anteior do link
	*/	
	void increase (rca::Link &l, int value) {
		l.setValue (this->value(l) + value);
		m_heap.increase (m_ehandle_matrix[l.getX()][l.getY()].second, l);
	}
	
	/**
	* Método utilizado para diminuir o valor do link no container.
	* Ele recebe como parâmetro o @see rca::Link l e um valor inteiro que
	* será seu novo valor.
	*
	* @param rca::Link 
	* @param int nova valor que será menor que o valor anterior do link
	*/
	void decrease (rca::Link &l, int value) {
		l.setValue (this->value (l) - value);
		m_heap.decrease (m_ehandle_matrix[l.getX()][l.getY()].second, l);
	}
	
	/**
	* Método utilizado para remover um link do heap.
	* 
	* O link será removido do heap. Em seguida, a matrix de handles
	* será reconfigurada na posição do link removido para assumir o valor padrão.
	*
	* Isto é necessário, pois o link pode ser utilizado novamente em algum momento.
	*
	* @param rca::Link
	*/
	void erase (rca::Link & l) {
	
		m_heap.erase (m_ehandle_matrix[l.getX()][l.getY()].second);
		m_ehandle_matrix[l.getX()][l.getY()].second = m_ehandle_matrix[0][0].second;
		m_ehandle_matrix[l.getX()][l.getY()].first = false;
	}
	
	/**
	* Método utilizado para inserir uma aresta no heap.
	* 
	* O método recebe um @see rca::Link e verifica se o mesmo
	* está sendo utilizado. Caso afirmativo, ele será atualizado 
	* utilizando @see update. Caso negativo, ele será inserido e 
	* a matriz de manipuladores será atualizada.
	* 
	* @param rca::Link
	*
	*/
	void push (rca::Link & l ) {
		
		//testa se já foi utilizado
		if ( !is_used (l) ) {			
			//senão insere-o
			m_ehandle_matrix[l.getX()][l.getY()].second = m_heap.push (l);
			
			//informando about usage
			m_ehandle_matrix[l.getX()][l.getY()].first = true;
		} else {
			//caso já exista atualize
			this->update (l);
		}
	}
	
	//passar valor negativo para indicar decremento
	/**
	* Método utilizado para inserir um @see rca::Link no heap.
	* 
	* Este método considera que o link terá uma modificação no valor
	* de peso (custo, capacidade residual).
	*
	* São passados dois parâmetros: um link e um valor de uso que será decrementado
	* do valor do link.
	*
	* @param rca::Link
	* @param int
	*
	*/
	void push (rca::Link & l, int usage) {
	
		if ( this->is_used (l) ) {
		
			int value = this->value (l);
			value += usage;
			
			this->erase (l); //erasing			
			l.setValue (value); //updating
			
		} 
		
		this->push (l); //inserting
		
	}
	
	/**
	* Método utilizado para retonar uma referência para o heap.
	* Esta referência pode ser utilizada para iterar sobre
	* as arestas presentes no heap.
	*
	* @return boost::heap::fibonacci_heap<rca::Link, Comp>
	*/
	boost::heap::fibonacci_heap<rca::Link, Comp> & get_heap () 
	{
		return m_heap;
	}
	
public:
	//this matrix holds the congestion
	VMatrix m_matrix;
	
	//this matrix holds the handles to heap elements
	//EHandleMatrix m_ehandle_matrix;
	std::vector<std::vector< Handle >> m_ehandle_matrix;
	
	//this heap holds the edges form minimun congestion to max.
	//FibonnacciHeap m_heap;
	boost::heap::fibonacci_heap<rca::Link, Comp> m_heap;

};

#endif

}

