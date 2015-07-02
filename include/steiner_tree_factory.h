#ifndef _STTREE_FACTORY_H_
#define _STTREE_FACTORY_H_

#include <iostream>

#include "sttree.h"
#include "network.h"
#include "group.h"
#include "link.h"
#include "path.h"
#include "edgecontainer.h"
#include "steiner_tree_observer.h"

#include "algorithm.h"

namespace rca {
	
namespace sttalgo {

template<class Container>
class SteinerTreeFactory {

public:
	
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
	 * @author Romerito Campos.
	 */
	virtual void build (SteinerTreeObserver<Container> & sttree, 
					rca::Network & network, 
					rca::Group & g,
					Container& cg) = 0;
	
};

/**
 * Classe AGMZSteinerTree é uma classe que implementa a construção
 * de árvores de Steiner utilizando a ideia de contruir a árvore utilizando
 * o mínimo possível de arestas congestionadas.
 * 
 * 
 */
template <class Container>
class AGMZSteinerTree : public SteinerTreeFactory<Container>
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
	 * @author Romerito Campos. 
	 */
	void build (SteinerTreeObserver<Container> & sttree, 
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
						STTree & st);
	
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

template <class Container>
class ShortestPathSteinerTree : SteinerTreeFactory<Container>
{

public:
	
	void build (SteinerTreeObserver<Container> & sttree, 
				rca::Network & network, 
				rca::Group & g,
				Container& cg);
	
};


template <class Container>
class WildestSteinerTree : SteinerTreeFactory<Container>
{

public:
	void build (SteinerTreeObserver<Container> & sttree, 
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

} //namespace factory
} //namespace rca

#endif