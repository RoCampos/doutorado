#ifndef _BST_COST_H_
#define _BST_COST_H_

#include <iostream>
#include <memory>

#include "steinertree.h"
#include "strategy_make_tree.h"

class BreadthSearchCost : public TreeAlgorithmInterface
{

public:
	void make_tree (rca::Group&, rca::Network &, 
				  std::shared_ptr<SteinerTree> & st);
	
};

#endif