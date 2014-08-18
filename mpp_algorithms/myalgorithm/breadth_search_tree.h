#ifndef _BST_H_
#define _BST_H_

#include <iostream>
#include <memory>
#include <queue>

#include "network.h"
#include "steinertree.h"

#include "strategy_make_tree.h"

class BreadthSearchTree : public TreeAlgorithmInterface {

public:
  void make_tree (rca::Group&, rca::Network &, 
				  std::shared_ptr<SteinerTree> & st);
  
};

#endif