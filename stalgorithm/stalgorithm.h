#include <iostream>

template <typename Algo, typename Net, typename ST, typename Group>
class STAlgorithm : private Algo{
	
	using Algo::heuristic;
	
public:
	
	void build (Net & net, ST & st, Group & g) {
		heuristic (net, st, g);
	}
	
};