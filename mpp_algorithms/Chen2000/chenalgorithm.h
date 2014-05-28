#ifndef CHEN_ALGORITHM_H
#define CHEN_ALGORITHM_H

#include <iostream>
#include <vector>
#include <string>

typedef struct sttree {
	
	
}STTree;

class Chen {

public: 
	Chen (std::string input);
	
	/*pre processing step*/
	void pre_processing ();
	
private:
	//matrix that stores the congestion over each egde
	std::vector< std::vector<int> > edges;
	
	//current max congestion
	double congestion;
	
	//the parameter that indicates the dilation
	int alpha;
	
	std::vector<STTree> tree;
	
};

#endif /*CHEN_ALGORITHM_H*/
