#ifndef _BST_COST_H_
#define _BST_COST_H_

#include <iostream>
#include <memory>
#include <queue>
#include <functional>
#include "network.h"
#include "link.h"
#include "group.h"

#include "steinertree.h"
#include "strategy_make_tree.h"

typedef typename std::priority_queue<rca::Link, std::vector<rca::Link>, std::greater<rca::Link> > priority_queue_edge;
typedef typename std::vector<int>::const_iterator c_iterator;

class BreadthSearchCost : public TreeAlgorithmInterface
{

public:
	void make_tree (rca::Group&, rca::Network &, 
				  std::shared_ptr<SteinerTree> & st);
	
	//this method update the priority_queue 
	void update_best_edge (int node, double value, priority_queue_edge&, rca::Network&);
	
};

#endif