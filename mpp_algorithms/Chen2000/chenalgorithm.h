#ifndef CHEN_ALGORITHM_H
#define CHEN_ALGORITHM_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <memory>

#include "group.h"
#include "network.h"

typedef struct sttree {
	
	
}STTree;

class Chen {

public: 
	Chen (std::string _input);
	
	/*pre processing step*/
	void pre_processing ();
	
private:
	
	std::string input;
	
	//matrix that stores the congestion over each egde
	std::vector< std::vector<int> > edges;
	
	//current max congestion
	double congestion;
	
	//the parameter that indicates the dilation
	int alpha;
	
	std::vector<STTree> tree;
	
	/*MPP variables*/
	shared_ptr<Network> m_net; //the network
	std::vector<shared_ptr<Group>> m_groups //multicast groups
	
};

#endif /*CHEN_ALGORITHM_H*/
