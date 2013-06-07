#ifndef _KSHORTESTPATH_H_
#define _KSHORTESTPATH_H_

#include <iostream>
#include <set>
#include <map>

#include "network.h"
#include "path.h"
#include "algorithm.h"
#include "link.h"

struct PathNode {
	
	double cost;
	int index; //index of the path in the list all_paths
	
	bool operator< (const PathNode& path) const{
		return cost < path.cost;
	}
	
	bool operator==(const PathNode& path) const {
		return cost == path.cost && index == path.index;
	}

	
};

class KShortestPath {
	
public:
	KShortestPath (rca::Network *net);

	~KShortestPath () { }
	
	void init (int v, int w);//make the first shortest path
	rca::Path next (); //return the shortest path, create new candidates
	bool hasNext ();

	inline void clear () {
		all_paths.clear ();
		m_candidate_paths.clear ();
		m_derivation_paths.clear ();
		m_kpaths.clear ();
	}
	
private:
	
	rca::Network * network;
	
	std::vector<rca::Path> all_paths;
	
	//PathNode storage index to path and cost
	std::set<PathNode> m_candidate_paths;
	
	//map <index to path, derivation vertex>
	std::map<int,int> m_derivation_paths;
	
	//ordered output paths
	std::vector<int> m_kpaths;
};

#endif
