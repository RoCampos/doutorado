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

};




#endif // LOCALSEARCH_H


