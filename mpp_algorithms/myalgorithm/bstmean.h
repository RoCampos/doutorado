#ifndef _BSTMEAN_
#define _BSTMEAN_

#include <iostream>

#include "network.h"
#include "link.h"
#include "group.h"

#include "steinertree.h"
#include "strategy_make_tree.h"

typedef typename std::priority_queue<rca::Link, std::vector<rca::Link>, std::greater<rca::Link> > priority_queue_edge;
typedef typename std::vector<int>::const_iterator c_iterator;

class BreadthSearchMean : public TreeAlgorithmInterface
{

public:
	void make_tree (rca::Group&, rca::Network &, 
				  std::shared_ptr<SteinerTree> & st);
	
private:	
	//removendo arestas pela média
	double remove_edges_by_mean (rca::Network &);
	
	//this method update the priority_queue 
	void update_best_edge (int node, double value, priority_queue_edge&, rca::Network&);
	
};

#endif 