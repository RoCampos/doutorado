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

#ifndef _GRASP_H_
#define _GRASP_H_

typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> CongestionHandle;
typedef typename rca::SteinerTreeObserver<CongestionHandle> STobserver;

class Grasp {
	
public:
	Grasp ();
	
	Grasp (rca::Network *net,std::vector<rca::Group>&);
	
	void set_iter (int iter) {m_iter = iter;}
	
	void build_solution ();
	
	void local_search ();
	
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
	std::vector<STTree> m_strees;
	rca::Network * m_network;
	std::vector<rca::Group> m_groups;
};

#endif