#include <iostream>

#include <algorithm>
#include <functional>

#include "network.h"
#include "steinertree.h"
#include "algorithm.h"
#include "path.h"

class KMB {

protected:
	
	void heuristic (rca::Network & net, SteinerTree & st, rca::Group & g);
	
};