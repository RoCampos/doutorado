#ifndef _BST_H_
#define _BST_H_

#include <iostream>
#include <memory>

#include "network.h"
#include "steinertree.h"

#include "properties.h"
#include "strategy_make_tree.h"

class BeadthSearchTree : public TreeAlgorithmInterface {

public:
  void make_tree (int group_id, std::shared_ptr<SteinerTree> & st);
  
};

#endif