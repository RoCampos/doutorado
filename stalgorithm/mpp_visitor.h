#ifndef _MPP_VISITOR_H_
#define _MPP_VISITOR_H_

#include <vector>
#include <queue>

#include "network.h"
#include "group.h"
#include "sttree.h"
#include "link.h"
#include "edgecontainer.h"
#include "sttree_visitor.h"

typedef std::vector<STTree> MPPSolution;
typedef std::vector<rca::Group> MulticastGroup;
typedef std::tuple<int,int,rca::Link,rca::Link> TupleEdgeRemove;

using namespace rca;

class MppVisitor {
	
public:
	MppVisitor (MPPSolution * tree){
		m_trees = tree;
	}
	
	virtual void visit () = 0;
	
	void setNetwork (rca::Network * net) {
		m_network = net;
	}
	
	void setMulticastGroups (MulticastGroup & groups) {
		m_groups = groups;
	}
	
	void setMulticastGroups (std::vector<std::shared_ptr<rca::Group>> & groups) {
		for (std::shared_ptr<rca::Group> g : groups) {
			m_groups.push_back (*g);
		}
	}
	
protected:
	
	void prepare_trees () {
	
		m_temp_trees.clear ();
		
		int i=0;
		auto st = m_trees->begin ();
		//for (SteinerTree * st : m_trees) {
		for (; st != m_trees->end(); st++) {
			
			edge_t * perc = st->get_edge ();
			std::vector<rca::Link> links;
			while (perc !=NULL) {
				rca::Link link (perc->x, perc->y, 0);
				/**
				 * Se na iteração passada do visitor a aresta foi removida
				 * então, não a adiciona na solução que será processada
				 * após o prepare_trees()
				 */
				if (perc->in) { 
					links.push_back (link);
				}
				perc = perc->next;
			}
			m_temp_trees.push_back ( links );
			
			i++;
		}
	}

protected:
	rca::Network * m_network;
	MPPSolution * m_trees;
	MulticastGroup m_groups;
	std::vector<std::vector<rca::Link>> m_temp_trees;
	
};

class ChenReplaceVisitor : public MppVisitor
{

public:
	ChenReplaceVisitor (MPPSolution* tree) : MppVisitor (tree) {}
	
	/*
	 * Pega a menor capacidade residual (link)
	 * para todo link com aquela capacidade
	 * tenta-se substituí-lo por outro
	 */
	virtual void visit ();
	
	void setEdgeContainer (rca::EdgeContainer<Comparator, HCell> & ec) {
		m_ec = &ec;
	}
	
	void visitByCost ();
	
	
private:
	/*faz um corte no grafo divindo a árvore em duas*/
	std::vector<int> make_cut (int tree_id, const rca::Link&);	
	void replace (TupleEdgeRemove &);
	

	/*Auxiliar methods*/
	void getAvailableEdges (std::vector<int> &cut, 
							const rca::Link&, 
							std::vector<rca::Link>&);
	
	/**
	 * Método used to get edges with better cost than that on the trees
	 * 
	 */
	void getAvailableEdgesByCost (std::vector<int> &cut, 
								  const rca::Link&,
							   std::vector<rca::Link>&);
	
	
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
	
	
private:
	int m_capacity;
	rca::EdgeContainer<Comparator, HCell> * m_ec;
	
};

#endif