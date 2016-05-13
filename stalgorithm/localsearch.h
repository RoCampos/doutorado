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
#include "algorithm.h" //get_shortest_path

enum EdgeType{
	IN = 0,
	OUT = 1
};

namespace rca {

	namespace sttalgo {


class LocalSearch {

typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> Container;
typedef typename rca::Network Network;
typedef typename std::vector<rca::Group> VGroups;
typedef typename std::vector<steiner> Solution;
typedef typename std::vector<rca::Link> VEdge;

public:
	LocalSearch (Network & net, std::vector<rca::Group> & groups);	
	LocalSearch (Network & net, std::vector<rca::Group> & groups, Container &);	
	
	void apply (Solution &, int& cost, int& res);

	~LocalSearch () {
		m_cg = NULL;
	}

	void clear_removed () {
		this->m_removed.clear ();
	}

	std::vector<rca::Link> get_removed () {
		return this->m_removed;
	}

private:

	/**
	*	
	* @param int tree_id
	* @param steiner árvore a ser modificada
	* @param int custo da árvore
	*/
	bool cut_replace (int x, int y, int id, steiner & tree, int& solcost);

	void inline_replace (steiner &, int&, 
		rca::Link& out, 
		rca::Link& in, 
		int tree_id);

	void update_container (Solution & solution);

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

	int m_top;

};


class CycleLocalSearch {

typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> Container;
typedef typename rca::Network Network;
typedef typename std::vector<rca::Group> VGroups;
typedef typename std::vector<steiner> Solution;
typedef typename std::vector<rca::Link> VEdge;

public:

	CycleLocalSearch (Network & net, std::vector<rca::Group> & groups);
	CycleLocalSearch (Network & net, std::vector<rca::Group> & groups, Container &);

	void apply (Solution & solution, int & cost, int & res);

	~CycleLocalSearch ();

private:

	void update_container (Solution &);
	void get_vertex (std::vector<int> &, steiner &);

	bool cut_replace (int , std::vector<int>& , steiner & st, int& );
	
	void inline_replace (steiner &, int&, 
		rca::Link& out, 
		rca::Link& in, 
		int tree_id);

	void inline_update (rca::Link&, EdgeType, int tree_id);

	rca::Link get_out_link (rca::Path&, steiner &);

	rca::Path get_path (int x, int y, steiner & st) {

		path.clear ();
		m_marked.clear ();		

		rca::Link l (x,y,0);

		m_finished = false;		
		path = std::vector<int> (m_network->getNumberNodes (), -1);
		m_marked = std::vector<bool> (m_network->getNumberNodes ());
		m_marked[l.getX()] = true;

		dfs (l.getX(), l.getY(), st);

		return get_shortest_path (l.getX(), l.getY(), *m_network, this->path);

	}
	void dfs (int x, int y, steiner& st) {
		if (x == y) {			
			m_finished = true;
			return;
		}
		m_marked[x] = true;
		for (int adj : st.get_adjacent_vertices(x)) {
			if (!m_marked[adj] && !m_finished ) {				
				path[adj] = x;
				dfs (adj, y, st);
			}
		}

	}

private:

	// ----- for local search ---- 
	std::vector<bool> m_marked;
	bool m_finished = false;
	std::vector<int> path;

	Container * m_cg;
	Network * m_network;
	std::vector<rca::Group> * m_groups;

	VEdge to_replace;

	VEdge m_removed;

	int m_top;

};


template<class Container, class SteinerRpr>
class PathExchange {
	typedef typename std::vector<std::pair<rca::Link, rca::Link>> Trocas;
	typedef typename std::vector<rca::Path> Paths;	
	

public:
	PathExchange() {}

	bool run (std::vector<SteinerRpr>&, Container&, 
		rca::Network&, std::vector<rca::Group>&);

private:
	std::vector<std::vector<int>> get_components (SteinerRpr& st);
	
	bool test_pathsize (int c1, int c2, 
		Paths &, std::vector<int> distancia);

	bool control_trocas (rca::Link&, rca::Link&);

	void paths_distance (SteinerRpr&, 
		Paths&, 
		std::vector<int>& distancia, 
		std::vector<int>& members, 
		int source);


	bool core_search (steiner & st, 
		Container & container, 
		rca::Network& network, 
		rca::Link& old, 
		rca::Group& g);

	bool tocontinue (rca::Link& novo, rca::Link& old,
		Container& cg, int tk, int band) {
		return (novo != old) && cg.test_bandwitdh (novo, band, tk);		
	}

	bool check_change (std::vector<std::vector<int>>&,
		int c1, int c2, int source,
		Paths& paths, 
		std::vector<int>& distancia);

	bool do_change (steiner&, Container&, 
		rca::Link& novo, rca::Link& old, 
		rca::Network&, rca::Group&);

private:
	Trocas m_trocas;


};


} // sttalgo

} // rca

#endif // LOCALSEARCH_H


