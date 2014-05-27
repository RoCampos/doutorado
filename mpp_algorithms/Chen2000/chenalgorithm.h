#ifndef CHEN_ALGORITHM_H
#define CHEN_ALGORITHM_H

#include <iostream>
#include <vector>
#include <string>

class Chen {

public: 
	Chen (std::string input);
	
private:
	std::vector< std::vector<int> > edges;
	double congestion;
	
};

#endif /*CHEN_ALGORITHM_H*/
