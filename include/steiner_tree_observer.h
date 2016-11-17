#include <iostream>
#include "edgecontainer.h"
#include "sttree.h"
#include "steiner.h"
#include "disjointset2.h"
#include "sttree_visitor.h"
#include "link.h"

namespace rca {
	
namespace sttalgo {

#ifndef _STEINER_TREE_OBSERVER_
#define _STEINER_TREE_OBSERVER_
	
/**
* Classe utilizada como observer para modificações na
* estrutura de uma árvore de Steiner.
* 
* O observer mantém um ponteiro para o manipulador de arestas
* e um ponteiro para uma estrutura de árvore de steiner.
*
* Sempre que uma árvore de Steiner é modificada pela inserção de aresta
* observer verifica se há criação de ciclo. Não permitindo adição de arestas
* que venham a criar ciclos.
*
* Após adicionar uma arestas o container de arestas é atualizado para
* refletir o estado real de uso da rede
*
* @author Romerito C. Andrade
*/
template<typename ContainerType, typename SteinerRepr>
class SteinerTreeObserver {

public:
	/**
	* Construtor padrão.
	*
	*/
	SteinerTreeObserver();

	/**
	* Construtor que recebe um container de arestas, uma
	* estrutura para armazenar árvores de steiner e um 
	* inteiro representando o número de arestas. 
	*
	* @param ContainerType container de arestas
	* @param STTree estrutura para armazenar árvore
	* @param int número de nós da rede
	*/
	SteinerTreeObserver(ContainerType & ec, SteinerRepr & st, int);

	/**
	* 
	* Construtor que recebe um container de arestas, uma
	* estrutura para armazenar árvores de steiner e um 
	* inteiro representando o número de arestas.
	*
	* Além disso, o construtor recebe uma referência para o grafo no 
	* qual as soluções estão sendo construídas.
	*
	* @param ContainerType container de arestas
	* @param STTree estrutura para armazenar árvore
	* @param rca::Network referência para um objeto rca::Network
	* @param int número de nós da rede
	*/
	SteinerTreeObserver(ContainerType & ec, SteinerRepr & st, rca::Network&,int);
	

	~SteinerTreeObserver () {
		m_ec = NULL;
		m_st = NULL;
		m_network = NULL;
		// if (dset != NULL) {
		// 	delete dset;
		// } 
	}

	/**
	* Objeto network passado ao método prunning da classe.
	*
	*
	* @param rca::Network referência para um objeto rca::Network
	*/
	void set_network (rca::Network & net);

	/**
	* Método utilizado para definir uma estrutura de árvore de 
	* steiner a ser manipulado pelo observer.
	*
	* @param STTree estrutura para armazenar árvore 
	* @param int número de nós da rede
	*/
	void set_steiner_tree (SteinerRepr &st, int);

	/**
	* Método para acessar o árvore de steiner mantida pelo observer
	* @return STTree estrutura que armazena árvore de steiner
	*/
	SteinerRepr & get_steiner_tree ();
	
	/**
	* Método para definir container a ser utilizado pelo obsever
	* @param ContainerType container de arestas
	*/
	void set_container (ContainerType & ec);

	/**
	* Método utilizado para acessar container de arestas
	* @return ContainerType container de arestas
	*/
	ContainerType & get_container ();
	
	/**
	* Método utilizado para adicionar uma aresta á árvore de steiner
	* Se a arestas não formar ciclo, ela é adicionar e o método retonar true
	*
	* @param int nó x
	* @param int nó y
	* @param int custo
	* @param int capacidade do link 
	*/
	bool add_edge (int, int, int, int);


	//adiciona mais um inteiro na interface, consumo do link
	/**
	* @param int x
	* @param int y
	* @param int custo
	* @param int trequest
	* @param int capacidade do link
	*/
	bool add_edge (int, int, int, int, int);
	
	/**
	* Método que retorna uma lista de arestas representando a 
	* árvore.
	*
	* @return std::vector<rca::Link> lista de arestas representando árvore
	*
	*/
	std::vector<rca::Link> getTreeAsLinks () const;
	
	/**
	* Método utilizado para fazer o prune de nós não terminais
	* que sejam nós folhas.
	*
	* Este método utiliza o algoritmo prunning da presente em sttree_visitor.h.
	* 
	* O primeiro parâmetro é o valor de consumo da árvore em relação a capacidade da
	* areasta.
	* 
	* O segundo parâmetro é valor máximo de capacidade da arestas. Quando uma aresta não
	* é utilizada por nenhuma árvore, então seu valor de capacidade é restaurado.
	*
	* O algoritmo é aplicado quando a capacidade das arestas é uniforme e o consumo
	* de banda também.
	*
	* @param int valor de uso liberado quando uma aresta é removida
	* @param int valor de capacidade máxima para quando a aresta não é utilizado por nenhuma árvore
	*/
	void prune (int, int);

private:
	ContainerType * m_ec;
	SteinerRepr * m_st;	
	DisjointSet2 * dset;
	rca::Network * m_network;
	
};

#endif

}

}

