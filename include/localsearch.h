#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include <iostream>
#include <vector>
#include <utility>
#include <typeinfo>

#include "edgecontainer.h"
#include "network.h"
#include "steiner.h"
#include "link.h"
#include "group.h"

enum EdgeType{
	IN = 0,
	OUT = 1
};

class LocalSearch {

typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> Container;
typedef typename rca::Network Network;
typedef typename std::vector<rca::Group> VGroups;
typedef typename std::vector<steiner> Solution;
// typedef typename std::vector<std::pair<int,int>> ListEdge;
typedef typename std::vector<rca::Link> VEdge;

public:
	LocalSearch (Network & net, std::vector<rca::Group> & groups);
	
	void apply (Solution &, int& cost, int& res);

	~LocalSearch () {
		delete m_cg;
	}

	void clear_removed () {
		this->m_removed.clear ();
	}

	std::vector<rca::Link> get_removed () {
		return this->m_removed;
	}

private:

	bool cut_replace (int, int, int id,  steiner & tree, int&);

	void do_replace (Solution & solution, int & cost);

	void inline_replace (steiner &, int&, 
		rca::Link& out, 
		rca::Link& in, 
		int tree_id);

	bool find (rca::Link e, EdgeType type ) {

		VEdge::iterator res, end;
		if (type == EdgeType::IN) {
			res = std::find (m_in.begin(), m_in.end(), e);				
			end = m_in.end();
		} else if (type == EdgeType::OUT) {
			res = std::find (m_out.begin(), m_out.end(), e);	
			end = m_out.end();
		}

		return res != end;
	}

	void update_container (Solution & solution) {
		m_cg = new Container;
		m_cg->init_handle_matrix ( m_network->getNumberNodes () );

		for (auto & tree : solution) {
			auto edges = tree.get_all_edges ();			
			for (const std::pair<int,int> & e : edges) {
				int cost = m_network->getCost (e.first, e.second);
				rca::Link l (e.first, e.second, cost);
				if (!m_cg->is_used (l)) {
					m_cg->push (l);
				}
			}
		}
	}

	void update_container2 (Solution & solution) {
		
		//atualizar Z e custo enquanto modifica solução.
		//remover update_sol de teste2

		m_cg = new Container;
		m_cg->init_handle_matrix ( m_network->getNumberNodes () );

		int LINKS = solution.size ();

		for (auto & tree : solution) {
			auto edges = tree.get_all_edges ();			
			for (const std::pair<int,int> & e : edges) {
				int cost = m_network->getCost (e.first, e.second);
				rca::Link l (e.first, e.second, cost);
				
				if ( !m_cg->is_used (l) ) {					
					
					to_replace.push_back (l);

					l.setValue (LINKS-1);
					m_cg->push (l);

				} else {
					int value = m_cg->value (l);
					m_cg->erase (l);
					l.setValue (value - 1);
					m_cg->push (l);
				}
			}
		}

		std::sort (to_replace.begin(), to_replace.end(), std::greater<rca::Link>());

	}

	void inline_update (rca::Link&, EdgeType, int tree_id);

private:
	Container * m_cg;
	Network * m_network;
	std::vector<rca::Group> * m_groups;

	VEdge to_replace;
	VEdge m_removed;

	//the index represents the trees
	VEdge m_out;
	VEdge m_in;
	std::vector<int> m_id;

};

#endif // LOCALSEARCH_H


