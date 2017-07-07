#ifndef _MPP_VISITOR_H_
#define _MPP_VISITOR_H_

#include <vector>
#include <queue>
#include <memory>

#include "network.h"
#include "group.h"
#include "sttree.h"
#include "steiner.h"
#include "link.h"
#include "edgecontainer.h"
#include "sttree_visitor.h"

typedef std::vector<STTree> MPPSolution;
typedef std::vector<rca::Group> MulticastGroup;
typedef std::tuple<int,int,rca::Link,rca::Link> TupleEdgeRemove;

namespace rca {
	
namespace sttalgo {

/**
* Esta classe representa um visitor abstrato para ser aplicado
* a uma solução do problema de roteamento multicast com múltiplas (PRMM)
* sessões.
*
* Esta classe define um método visit como virtual. As subclasses
* devem implementá-lo para fornecer a ação a ser aplicada a solução (PRMM)
*
* 
* @author Romerito C. Andrade
*
*/
template <class SolutionType = STTree>
class MppVisitor {
	
public:

	/**
	* Construtur da classe MPPVisitor recebe como entrada
	* uma solução do PRMM. 
	*
	* A solução consistem em uma lista de árvores multicast representadas
	* como @see STTree. Esta lista é armazenada em um std::vector.
	* 
	* @param MPPSolution ponteiro para vector de STTree's
	*/
	MppVisitor (std::vector<SolutionType> * tree){
		m_trees = tree;
	}
	
	/**
	* Método virtual que deve ser implementado.
	* 
	*
	*/
	virtual void visit () = 0;
	
	/**
	* Método que permite definir a instância da rede sob
	* a qual a solução foi construída. Isto permite que 
	* manipulações realizadas sob a solução que precisem 
	* acessar informações do grafo sejam realizadas de forma
	* eficiente.
	*
	* @param rca::Network ponteiro para uma isntância de Network
	*
	*/
	void setNetwork (rca::Network * net) {
		m_network = net;
	}
	
	/**
	* Método que permite definir os grupos multicast associados
	* a solução que será visitada.
	*
	* Ele recebe como parâmetro uma lista de @see rca::Group.
	*
	* @param MulticastGroup vector de rca::Group's multicast
	*/
	void setMulticastGroups (MulticastGroup & groups) {
		m_groups = groups;
	}
	

	/**
	* Método que permite definir os grupos multicast associados
	* a solução que será visitada.
	*
	* Ele recebe como parâmetro uma lista de @see rca::Group.
	*
	* Os grupos são passadas utilizados shared_ptr's.
	*
	* @param MulticastGroup vector de rca::Group's multicast
	*/
	void setMulticastGroups (std::vector<std::shared_ptr<rca::Group>> & groups) {
		for (std::shared_ptr<rca::Group> g : groups) {
			m_groups.push_back (*g);
		}
	}
	
protected:
	
	/**
	* método utilizado para tranformar uma STTree em uma lista de 
	* arestas para processsamento pelo visitor.
	* 
	*
	*/
	void prepare_trees () {
	
		m_temp_trees.clear ();
		
		int i=0;
		auto st = m_trees->begin ();
		//for (SteinerTree * st : m_trees) {
		for (; st != m_trees->end(); st++) {
			
			std::vector<rca::Link> links;
			for (auto & edge : st->get_all_edges ()) {
				int cost = (int) this->m_network->getCost (edge.first, edge.second);
				rca::Link l (edge.first, edge.second, cost);
				links.push_back (l);
			}


			m_temp_trees.push_back ( links );
			
			i++;
		}
	}

protected:
	rca::Network * m_network;
	std::vector<SolutionType> * m_trees;
	MulticastGroup m_groups;
	std::vector<std::vector<rca::Link>> m_temp_trees;
	
};

template <class SolutionType = STTree>
class ChenReplaceVisitor : public MppVisitor<SolutionType>
{

// using m_network;
// using m_trees;
// using m_groups;
// using m_temp_trees;

public:

	/**
	* Construtor da classe.
	*
	* Recebe como parâmetro uma vector de STRee's e repassa
	* para o construtor da classe base - MppVisitor.
	*
	* @param MPPSolutino ponteiro para vector de STTree's
	*/
	ChenReplaceVisitor (std::vector<SolutionType>* tree) : MppVisitor<SolutionType> (tree) {}
	
	/*
	 * Pega a menor capacidade residual (link)
	 * para todo link com aquela capacidade
	 * tenta-se substituí-lo por outro
	 * 
	 *
	 *
	 */
	virtual void visit ();
	
	/**
	* Método utilizad para fazer as substiuiçoes de arestas
	* com base na melhoria do custo da solução.
	* 
	* Inicialmente, aplica-se o método @see prepare_trees. Em seguida
	* para cada aresta, é verificado se há uma aresta que pode ser
	* sua substituta na solução. Caso afirmativo, então é feita a troca.
	*
	* 
	*/
	void visitByCost ();
	
	/**
	 * Método que realiza a operação de substituição de arestas
	 * enquanto for possível melhorar.
	 * 
	 * INternatmente, faz-se chamadas ao método @visitByCost.
	 * 
	 * @param int & cost
	 */
	void visitByCost (double & cost) {
		int tt = cost;		
		do {			
			cost = tt;
			this->visitByCost ();
			tt = this->get_solution_cost ();
		} while (tt < cost);		
	}
	
	/**
	* Método utilizado para definir o container de arestas que será
	* utilizado para controlar as alterações nas arestas.
	*
	* @param rca::EdgeContainer<Comparator, HCell>
	*/
	void setEdgeContainer (rca::EdgeContainer<Comparator, HCell> & ec) {
		m_ec = &ec;
	}
	
	/**
	* Método utilizado para retonar as arestas que foram removidas
	*
	* @return std::vector<rca::Link> lista de arestas removidas
	*/
	std::vector<rca::Link> get_replaced () {return m_replaced;}

	/**
	* método utilizado para limpar a lista de arestas removidas
	*
	*/
	void clear_replaced () {
		m_replaced.clear ();
	}
	

	/**
	* Método utilizado para retornar o custo de uma solução.
	* 
	* O custo da solução é atualizado a medida que as modificações 
	* são realizadas. Isto permite acessar o custo de forma eficiente.
	*
	* @return double custo atualizado da solução
	*/
	double get_solution_cost () 
	{
		return m_cost;
	}
	
	/**
	* Retorna o número de arestas que possui valor de capacidade residual
	* igual ao valor máximo da soluçaõ.
	*
	* @return int número de arestas com valor de capacidade residual máximo.
	*/
	int get_number_top_nodes () {
	
		const auto & heap = m_ec->get_heap ();
		auto it = heap.ordered_begin ();
		auto end = heap.ordered_end ();
		int top = m_ec->top ();
		int count = 0;
		for (; it != end; it++) {
			if (it->getValue () == top) {
				count++;
			} else if (it->getValue () > top) {
				break;
			}
		}
		
		return count;
	}
	
private:
	/*faz um corte no grafo divindo a árvore em duas*/
	std::vector<int> make_cut (int tree_id, const rca::Link&);	
	void replace (TupleEdgeRemove &);
	

	/*Auxiliar methods*/
	void getAvailableEdges (std::vector<int> &cut, 
							const rca::Link&, 
							std::vector<rca::Link>&, 
							int group_id);
	
	/**
	 * Método used to get edges with better cost than that on the trees
	 * 
	 */
	void getAvailableEdgesByCost (std::vector<int> &cut, 
								  const rca::Link&,
							   std::vector<rca::Link>&, 
							   int tree_id);
	
	
	/**
	 * Método para obter uma tupla para obter um tupla
	 * com as informações de remoção.
	 * 
	 * @param int represeta o id do número para acessar m_tree_temp
	 * @param rca::Link representa o link que vai ser subtituído
	 */
	std::tuple<int,int,rca::Link,rca::Link> get_tuple (int, rca::Link&);
	
	
	
	/*cria novas árvores de steiner
	 etapa final do replace*/
	void update_trees ();
	
	void push_replaced (rca::Link & _old) {
		auto it = std::find(std::begin(m_replaced), 
							std::end(m_replaced), 
							_old);	
		
		if (it == std::end(m_replaced))
			m_replaced.push_back (_old);		
	}
	
	
private:
	int m_capacity;
	rca::EdgeContainer<Comparator, HCell> * m_ec;
	
	std::vector<rca::Link> m_replaced;
	
	double m_cost;
	
};

} //end of sttalgo namespace
} //end of rca namespace

#endif