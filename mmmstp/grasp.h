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
		cg = copy.cg;
		m_trees = copy.m_trees;
	}
	
	sttree_t & operator= (sttree_t const & copy) {
		
		m_cost = copy.m_cost;
		m_residual_cap = copy.m_residual_cap;
		cg = copy.cg;
		m_trees = copy.m_trees;
		
		return *this;
	}
	
	sttree_t () {}
	
} steiner_tree_t;

class Grasp {
	
public:
	Grasp ();
	
	Grasp (rca::Network *net,std::vector<rca::Group>&);
	
	void set_iter (int iter) {m_iter = iter;}
	
	sttree_t build_solution ();
	
	void local_search (sttree_t* sol);
	
	void run ();
	
private:
	void update_usage (STTree & sttre);
	
	void reset_links_usage ();
	
private:
	/**/
	std::vector<rca::Link> m_links;
	
	/*Algorithm information*/
	int m_iter;
	
	/*problem information*/
	sttree_t m_strees;
	rca::Network * m_network;
	std::vector<rca::Group> m_groups;
};

#endif