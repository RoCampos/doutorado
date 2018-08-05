#ifndef _L_SEARCH_
#define _L_SEARCH_

#include <vector>
#include <set>
#include <numeric>

#include "network.h"
#include "link.h"
#include "group.h"
#include "edgecontainer.h"
#include "sttree.h"
#include "steiner.h"
#include "sttree_visitor.h"

namespace rca {
	
namespace sttalgo {

	
/**
* Esta classe implementa um método para fazer busca em profundidade.
* 
* O método é especializado para iniciar e terminar no em pontos
* que formam uma aresta.
*
* Esta classe permite obter os predecessores de um nó para que o 
* o caminho possa ser recriado.
*
* Esta classe é utilizada em conjunto com @see cycle_local_search
* para obter um ciclo formado no grafo.
* 
*/
template<class NetworkType>
class depth_first_search{

	//TODO APLICAR VISITOR PARA OBTER OS PATHS EM UMA LISTA DE ARESTAS
	
public:

	/**
	* Método que inicia a busca em profundidade.
	* 
	* @param std::vector<rca::Link> árvore de steiner representada como links
	* @param rca::Group grupo multicast associado a árvore 
	* @param rca::Link link que formará o ciclo na árvore
	* @param rca::Network rede onde a árvore é formada
	*/
	void execute (std::vector<rca::Link>&, 
				rca::Group&, 
				rca::Link&, 
				rca::Network &);
	
	/**
	* Método que aplica recursividade na busca em profundidade
	*
	* NetworkType representa um tipo de rede que será utilizada.
	* Alguns algoritmo utilizam @see AdjacentNetwork
	* 
	* @param int nó atual para ser explorado.
	* @param rca::Group grupo multicast que se pretende conectar
	* @param NetworkType tipo de rede utilizada
	*/
	void dfs (int x, 
				rca::Group&,
				NetworkType&);

	/**
	* Método que retorna o ponto inicia da busca em profundidade.
	*
	* @return int nó inicial da busca em profundidade
	*/
	int getStart () {return x;}

	/**
	* Método que retorna o ponto final da busca em profundidade.
	*
	* @return int nó final da busca em profundidade
	*/
	int getTarget () {return y;}
	
	//return the predecessor list of visited vertex
	/*
	* Método que retorna a lista de predecessores
	* criados durante a busca em profundidade.
	*
	* @return std::vector<int> nós predecessores.
	*/
	std::vector<int> getPredecessorList () {return predecessor;}
	
private:	
	std::vector<int> predecessor;
	std::vector<int> color;
	
	int x; //start the search
	int y; //target of the search
	
	bool stop;
	
};
	
/**
 *
 * Esta struct é usada para encapsular uma busca local
 * sobre árvores de steiner. A melhoria é baseada no custo e
 * é otimizada com restrições na rede (capacidade das arestas).
 *
 * A otimização do custo não piora a capacidade residual das arestas
 * utilizados.
 * 
 * A classe @see depth_first_search é utilizada para auxiliar na busca.
 * 
 * @author Romerito C. Andrade
 */
template<typename Container, typename SolutionType = STTree>
struct cycle_local_search {

	/**	 
	 *
	 * Método utilizado para realizar a busca local em árvores multicast para melhorar
	 * o custo. O procedimento é baseado na criação de ciclos na árvores multicast.
	 * Os ciclos são removidos de maneira que alguma melhoria seja realizada.
	 *
	 * O melhoria ocorre, principalmente, quando se remove uma aresta que liga dois
	 * vértices não terminais (não pertencem ao grupo de destinos nem são nó fonte.)
	 * 
	 * @param int tree_id where the local search is perfomed
	 * @param std::vector<STTree> vector containing the trees to update the tree
	 * @param rca::Group group multicast related to tree tree_id
	 * @param Container template parameter: @see rca::EdgetContainer
	 */
	void execute ( int tree_id, 
				std::vector<SolutionType> &, 
				rca::Network& m_network, 
				std::vector<rca::Group>&, 
				Container&);
	
	/**
	 *  
	 * Este método é utilizado para retonar o ciclo onde a nova aresta é adicionada.
	 * Considerando uma aresta e=(x,w), o ciclo é formado na árvore passada como uma
	 * lista de arestas (std::vector).
	 *
	 * Então, o método encontra o ciclo e o retorna como uma lista de arestas (std::vector)
	 *
	 * @param std::vector<rca::Link> Árvore passada como uma lista de arestas
	 * @param rca::Group grupo multicast associado a árvore passada
	 * @param rca::Link aresta que cria o ciclo na árvore
	 * @param rca::Network grafo que representa a rede onde a árvore é construída.
	 */
	std::vector<rca::Link> 
	get_circle (std::vector<rca::Link>&, 
				rca::Group&, 
				rca::Link&, 
				rca::Network &);
	
	/**	 
	 *
	 * Este método faz chamadas ao método @see execute até que alguma melhoria seja
	 * encontrada na solução do problema de roteamento multicast com múltiplas sessões (PRMM).
	 *
	 * O método recebe um std::vector de @see STTree que representa uma solução do PRMM
	 * e tanta melhorar o custo da solução. 
	 *
	 * Além disso, recebe também a rede onde a solução é construída e o a lista de grupos
	 * multicast que representa os grupos.
	 *
	 * O Container de arestas é adicionado para garantir que o algoritmo não viole a
	 * capacidade residual da solução.
	 *
	 * O custo da solução antes da melhoria é passado como limite superior para evitar
	 * piora no custo da solução.
	 * 
	 * @param std::vector<STTree> lista de árvores que compõem uma solução.
	 * @param rca::Group lista de grupos multicast associados a solução
	 * @param Container template parameter: @see rca::EdgetContainer
	 * @param int O custo da solução antes da melhoria
	 */
	void local_search (std::vector<SolutionType> &, 
				rca::Network& m_network, 
				std::vector<rca::Group>&, 
				Container&, 
				int & cost);
	
};

} //end of sttalgo namespace
} //end of rca namespace

#endif