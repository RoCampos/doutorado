#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include "sttree.h"
#include "network.h"
#include "group.h"
#include "reader.h"
#include "edgecontainer.h"
#include "steiner_tree_observer.h"
#include "mpp_visitor.h"
#include "rcatime.h"

#ifndef _GRASP_H_
#define _GRASP_H_

typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> CongestionHandle;
typedef typename rca::SteinerTreeObserver<CongestionHandle> STobserver;

typedef struct sttree_t {
	
	std::vector<STTree> m_trees;
	int m_cost;
	int m_residual_cap; 
	CongestionHandle cg;
	
	sttree_t (sttree_t const & copy) 
	{
		m_cost = copy.m_cost;
		m_residual_cap = copy.m_residual_cap;
		cg = std::move(copy.cg);
		m_trees = copy.m_trees;
	}
	
	sttree_t & operator= (sttree_t const & copy) {
		
		m_cost = copy.m_cost;
		m_residual_cap = copy.m_residual_cap;
		cg = std::move(copy.cg);
		m_trees = copy.m_trees;
		
		return *this;
	}
	
	sttree_t () {}
	
	void print_solution ();
	
} steiner_tree_t;

class Grasp {
	
public:
	Grasp ();
	
	Grasp (rca::Network *net,std::vector<rca::Group>&, int budget = 0);
	
	void set_iter (int iter) {m_iter = iter;}
	void set_lrc (double lrc) {m_lrc = lrc;}
	void set_budget (int budget) {
		if (budget == 0)
			m_budget = INT_MAX;
		else 
			m_budget = budget;
		
	}
	void set_heur (double _heur) {m_heur = _heur;}
	
	void set_seed (int seed) {m_seed = seed;}
	
	sttree_t build_solution ();
	
	void shortest_path_tree (int id, STobserver*);
	void spanning_tree (STobserver * ob);
	
	void residual_refinament (sttree_t* sol, ChenReplaceVisitor&);
	void cost_refinament (sttree_t* sol, ChenReplaceVisitor&);
	
	void run ();
	
private:
	void update_usage (STTree & sttre);
	
	void reset_links_usage ();
	
	/**
	 * Remove arestas congestiondas para evitar uso pelo
	 * grupo idx
	 * 
	 * @param CongestionHandle
	 * @param int //next group
	 */
	void remove_congestioned_edges (CongestionHandle &, int idx);
	
private:
	/**/
	std::vector<rca::Link> m_links;
	
	/*Algorithm information*/
	int m_iter; //número de iterações
	double m_lrc; //tamanho da lista de aresta de spanning tree
	double m_budget; //valor de orçamento
	double m_heur; //define que heuristica utilizar
	
	int m_seed;
	
	/*problem information*/
	sttree_t m_strees;
	rca::Network * m_network;
	std::vector<rca::Group> m_groups;
};

#endif