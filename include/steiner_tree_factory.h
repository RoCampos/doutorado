#ifndef _STTREE_FACTORY_H_
#define _STTREE_FACTORY_H_

#include <iostream>
#include <stack>

#include "sttree.h"
#include "steiner.h"
#include "network.h"
#include "group.h"
#include "link.h"
#include "path.h"
#include "edgecontainer.h"
#include "steiner_tree_observer.h"

#include "algorithm.h"

namespace rca {
	
namespace sttalgo {

/**
* Classe que representa uma fábrica de árvores de Steiner
*  
* Tem como base o padrão factory method.
*
* A classe é um template que tem um argumento. Este argumento
* é o container de arestas que permite controlar o uso das arestas
*
* @author Romerito C. Andrade
*/
template<class Container, class SteinerRepr>
class SteinerTreeFactory {

public:
	
	/**
	*
	* Destrutor 
	*
	*/
	virtual ~SteinerTreeFactory (){}
	
	/**
	 * Este método é utilizado para criar uma árvore de Steiner para o grupo
	 * multicast g. A árvore será construída na topologia representada pelo 
	 * objeto Network.
	 * 
	 * O Container cg é utilizado para controlar a construção de árvores em 
	 * situações nas quais há restrição de capacidade das arestas e, portanto, é
	 * necessário controlar o uso das mesmas.
	 * 
	 * @param STTree
	 * @param rca::Network
	 * @param rca::Group
	 * @param Container	 
	 */
	virtual void build (
		SteinerTreeObserver<Container, SteinerRepr> & sttree, 
		rca::Network & network, 
		rca::Group & g,
		Container& cg) = 0;
	
};


typedef std::pair<int,int> EdgePair;
typedef std::map<EdgePair, rca::Link> EdgeMap;

struct DataSMT
{
	rca::Network * G;
	EdgeMap edgeMap;
	std::map<int, int> vertex;
	std::map<int, int> invertex;
	std::vector<rca::Link> links;

	~DataSMT () {
		delete G;
	}

};

template<class Container, class SteinerRepr>
class MinmaxSteinerFactory : public SteinerTreeFactory<Container, SteinerRepr> {
public:

	MinmaxSteinerFactory (rca::Network & network) {
		this->m_ptr_net = network.extend ();
		// this->m_copy = network;
	}

	~MinmaxSteinerFactory () {
		delete this->m_ptr_net;
		this->m_ptr_net = NULL;
	}

	void build (
		SteinerTreeObserver<Container, SteinerRepr> & sttree, 
		rca::Network & network, 
		rca::Group & g,
		Container& cg);
	
	DataSMT* join_components (
		std::vector<int> &bases,
		std::vector<int> &costpath,
		std::vector<std::vector<int>> &paths,
		std::vector<int> &scrs);

	int get_min_cap (
		rca::Network & net,
		rca::Path & p1, 
		rca::Path & p2, 
		rca::Link  l);

	void minimun_spanning_tree (DataSMT*);
	
	//using network m_copy
	void rebuild_solution (DataSMT*, 
		std::vector<std::vector<int>> & paths, 
		rca::Network &network);

	rca::Network & get_network () {
		return (this->m_copy);
	}

private:
	rca::Network * m_ptr_net;
	rca::Network m_copy;

};

/**
 * Classe AGMZSteinerTree é uma classe que implementa a construção
 * de árvores de Steiner utilizando a ideia de contruir a árvore utilizando
 * o mínimo possível de arestas congestionadas.
 *
 * A construção é realizada considerando uma árvore geradora. 
 * O algoritmo tenta reduzir ao máximo o custo de criação da árvore.
 * 
 * A classe implementa o método build de SteinerTreeFactory e adiciona dois
 * métodos: update_usage e create_list.
 *
 * Para usar este algoritmo é necessário usar o método create_list para cirar um
 * vector e apartir dele criar uma árvore geradora.
 * O método update_usage é utilizado para controlar as arestas mais utilizadas.
 *
 * @author Romerito C. Andrade
 */
template <class Container, class SteinerRepr>
class AGMZSteinerTree : public SteinerTreeFactory<Container, SteinerRepr>
{

public:
	
	/**
	 * Este método é uma implementação do método build de SteinerTreeFactory.
	 * 
	 * Ele implementa a criação de árvores de Steiner utilizando o algoritmo de
	 * árvore geradora de Kruskal.
	 * 
	 * @param STTree
	 * @param rca::Network
	 * @param rca::Group
	 * @param Container 
	 */
	void build (
		SteinerTreeObserver<Container, SteinerRepr> & sttree, 
		rca::Network & network, 
		rca::Group & g,
		Container& cg);

	/**
	 * Este atualiza a lista de arestas mais utilizadas
	 * Esta lista é utilizada para fazer a construção da árvore geradora.
	 * Observe-se que a árvore geradora é criada com base nas arestas menos
	 * utilizadas.
	 * 
	 * @param std::vector<rca::Link> arestas ordenadas por capaciade residual
	 * @param rca::Group 
	 * @param STTree 
	 */
	void update_usage (	rca::Group &g,
						rca::Network & m_network,
						SteinerRepr & st);
	
// 	/**
// 	 * Este método é utilizado para passar os links utilizados na construção
// 	 * da árvore para um grupo multicast.
// 	 * 
// 	 * Este links será atualizados por update_usage.
// 	 * Após a construção de uma árvore.
// 	 * 
// 	 * @param std::vector<rca::Link>
// 	 */
// 	void set_links (std::vector<rca::Link>& links) {m_links = links;}
	
	/**
	 * Método para criar a lista de arestas.
	 * 
	 * @param rca::Network
	 */
	void create_list (rca::Network&);
	
private:
	//used to control link utilization while build the tree using AGM
	std::vector<rca::Link> m_links;
	
};

/**
* Classe que implementa a construção de árvores de steiner
* com base na construção de caminho mais curto.
*
* A construção utiliza o algoritmo de dijkstra e constrói caminho
* mais curto da fonte para todos os outros nós do grupo multicast.
*
*/
template <class Container, class SteinerRepr>
class ShortestPathSteinerTree : public SteinerTreeFactory<Container, SteinerRepr>
{

public:
	
	/**
	 * Esta implementação do método build faz utilização do algoritmo
	 * de dijsktra para construção de árvores multicast para um grupo
	 * multicast especifico.
	 *
	 * O algoritmo considera a restrição sobre a utilização de arestas
	 * tendo como base as informações contidas no Container de arestas.
	 * 
	 * @param STTree
	 * @param rca::Network
	 * @param rca::Group
	 * @param Container	 
	 */
	void build (
		SteinerTreeObserver<Container, SteinerRepr> & sttree, 
		rca::Network & network, 
		rca::Group & g,
		Container& cg);
	
};


template <class Container, class SteinerRepr>
class WildestSteinerTree : public SteinerTreeFactory<Container, SteinerRepr>
{

public:

	/**
	* Método utilizado construir árvores com base no algoritmo
	* de caminho aumentado.
	* 
	* @param STTree
	* @param rca::Network
	* @param rca::Group
	* @param Container
	*/
	void build (
		SteinerTreeObserver<Container, SteinerRepr> & sttree, 
		rca::Network & network, 
		rca::Group & g,
		Container& cg);
	
	/**
	 * Este método é utilizado para atualizar a largura de banda
	 * das arestas.
	 * 
	 * Este procedimento é necessário para registrar as arestas
	 * que estão sendo mais utilizadas para construção das árvores
	 * 
	 * @param rca::Group
	 * @param STTree
	 * @param rca::Network
	 */
	void update_band (rca::Group & g, 
					rca::Network& network,
					STTree & st);
	
};

/**
*	
*	This class represents a object who performs a search for a tree with path limited from 
*	source to each destination node.
*	
*	The search is based on breadth search first.
*	
* 	A start node is select randomly and all neighboors of this node is added. And the search	
*	follows until all terminals nodes are added (terminals means the tree as steiner tree).
*
*	
*
*	@author Romerito Campos
*/
template <class Container, class SteinerRepr>
class LimitedBreadthSearchFirst : public SteinerTreeFactory<Container, SteinerRepr>
{

	typedef SteinerTreeObserver<Container, SteinerRepr> Observer;

public:

	
	LimitedBreadthSearchFirst (int limit) {
		this->LIMIT = limit;
	}

	~LimitedBreadthSearchFirst () {}

	void build (
		Observer & sttree,
		rca::Network & network,
		rca::Group & g,
		Container & cg);

	/**
	*	This method returns the size of a path
	*	the paths start at the 'source' and finish at 'member' node.
	*
	*	@param int source of the path
	*	@param int member node
	*	@return int size of a path
	*/
	int get_path_length (int member, int source);

private:

	std::vector<int> m_pred;
	int LIMIT;
	
};


/**
*	
*	This class represents an algorithm to perform tree construction
*	for steiner tree under path limited size constraints.
*
*	The algorithm uses the factory 'LimitedBreadthSearchFirst' as subroutine.
*
*	For each group of terminals, GROUP_SIZE trees are built. One tree for each
* 	member. The construction is performed by 'LimitedBreadthSearchFirst' instance
*
*	Then, a internal procedure is used to select the path with less cost. The selected
*	path might respect a size limit for the path (in terms of hop).
*
*	@author Romerito Campos
*/
template <class Container, class SteinerRepr>
class PathLimitedSearchTree : public SteinerTreeFactory<Container, SteinerRepr>
{

public:
	PathLimitedSearchTree (int limit) 
	: m_limit (limit) {}

	void build (
		SteinerTreeObserver<Container, SteinerRepr> & sttree,
		rca::Network & network,
		rca::Group & g,
		Container & cg);

	// para modificar o algoritmo de construção de solução
	void set_tree_builder ();

private:

	/**
	*	This method select the path found for each multicast group
	*	the result is multicast tree (steiner tree) holding the terminals
	*	node connected.
	*/
	void build_result_tree (std::vector<std::vector<rca::Path>>&,
		rca::sttalgo::SteinerTreeObserver<Container,SteinerRepr>&, 
		rca::Network&, rca::Group&);

private:
	int m_limit;

};


} //namespace factory
} //namespace rca

#endif