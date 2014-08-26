#ifndef _BSTMEAN_
#define _BSTMEAN_

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

class BreadthSearchMean : public TreeAlgorithmInterface
{

public:
	void make_tree (rca::Group&, rca::Network &, 
				  std::shared_ptr<SteinerTree> & st);
	
	inline void set_mean_modificator (double value) {
		m_mean_modificator = value;
	}
	
private:	
	//removendo arestas pela média
	void remove_edges_by_mean (rca::Network &,
							   rca::Group &, 
								std::vector<rca::Link>&);
	
	//this method update the priority_queue 
	void update_best_edge (int node, double value, priority_queue_edge&, rca::Network&);
	
	//variables
private:
	
	//value used to increase the value of mean.
	//the value of mean is used to separete edges to be removed
	double m_mean_modificator;
	
};

#endif 