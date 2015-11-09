#ifndef _DISJOINT_SET_H_
#define _DISJOINT_SET_H_

#include <iostream>
#include <vector>
#include <ostream>


/**
 * Esta estrutura representa uma partição
 * de um conjunto disjunto.
 * 
 * @author Romerito Campos
 * @date 2012/03/22
 */
struct NodeDs {

	int item;
	int rank;
	int count;
	NodeDs * parent;
	
	NodeDs (int _item) {
		item = _item;
		rank = 0;
		count = 1;
		parent = NULL;
	}
	
	/**
	* Operadodor de atribuição.
	* 
	* 
	* @param NodeDs
	* @return NodeDs - ponteiro
	*/
	NodeDs & operator=(const NodeDs & obj) {
		
		item = obj.item;
		parent = obj.parent;
		rank = obj.rank;
		count = obj.count;
		
		return *this;
	}
	
	/**
	* Destrutor da partição
	* 
	*/
	~NodeDs () {
		parent = NULL;
	}
	
	bool operator!=(const NodeDs & obj) {
		return parent != obj.parent;
	}
	
	friend std::ostream & operator<<(std::ostream & out, NodeDs & obj) {
		
		int _parent = (obj.parent ? obj.parent->item : -1);
		out << "Item: "<< obj.item << " Parent: "<< _parent;
		return out;
	}
	
};

/**
 * Esta classe representa um Conjunto Disjunto.
 * Ela possui implementação das três funções básicas.
 * Make_set, Find_set, Union find.
 * 
 * É utilizado o find com compressão de caminho.
 * 
 * @author Romerito Campos
 * @date 2012/22/02
 */
class DisjointSet2 {
	
	std::vector<NodeDs*> m_partition;
	
	int m_size;
	
	void make_set (int);
	
	int m_numberSets;
	
public:
	
	/**
	* Construtor da classe DisjointSet2.
	* Recebe como entrada o número de partições
	* 
	* @param int número de partições
	*/
	DisjointSet2 (int size);
	
	/**
	* Construtor padrão da classe DisjointSet2.
	* 
	* Este construtor é utilizado em conjunto com
	* o método @see init para criação das partições
	*
	*/
	DisjointSet2 ();
	
	/**
	* Destrutor todas as partições
	*
	*/
	~DisjointSet2 ();
	
	/**
	* Retorna um ponteiro para struct @see NodeDs que contém 
	* o elemento passado como parâmetro.
	* 
	* Nesta implementação o valor de parâmetro representa o id do
	* nó que se pretende saber a partição em que está.
	*
	* @param int
	* @return NodeDs
	*/
	NodeDs* find (int);
	
	//find usind compress path heuristic
	/**
	* Operação find utiliza compressão de caminho para otimizar novas
	* buscas pelo mesmo elemento.
	* 
	* Recebe como entrada um inteiro que representa o valor a ser verificado
	* a partição.
	*
	* Retorna um ponteiro para struct que contém o nó passado como parâmetro.
	*
	* @param int
	* @return NodeDs
	*/
	NodeDs* find2 (int);
	
	/**
	* Este método é utilizado quando se contrói um conjunto disjunto
	* utilizado o construtor padrão.
	* Com ele é possível adicionar novas partições
	* 
	* O parâmetro representa o identificador da nova partição.
	*
	* @param int 
	*/
	void addPartition (int);

	/**
	* Método utilizado em conjunto com o construtur padrão.
	* 
	* Este método inicializa as estruturas de dados da classe
	* conjuntos disjuntos. 
	* O número de partições criadas é igual o valor passado como
	* parâmetro.
	*
	* @param int 
	*/
	void init (int);

	/**
	* Método para difinir o tamanho do conjunto disjunto.
	* Recebe como entrada um inteiro.
	*
	* @param int
	*/
	void setSize (int size) {m_size = size;}
	
	/**
	* Método que retorna o número de partições.
	* @return int
	*/
	int getSize () const {return m_numberSets;}
	
	//une dois conjuntos que tenhos os elementos
	/**
	* Este método é utilizado para fazer o merge
	* de duas partições.
	* 
	* Ele recebe como entrada os id's dos conjuntos.
	* Internamente faz o find das partições e em seguida
	* o merge.
	*
	* Esta operação diminui o número de partições presentes
	* na estrutura.
	* 
	* @param int partição um
	* @param int partição dois
	*/
	bool simpleUnion (int, int);
	void print ();
	
};

#endif
