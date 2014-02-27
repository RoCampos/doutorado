#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <memory>
#include "network.h"

#include <memory>

using namespace std;
using namespace rca;

/*
 * Partition.
 * 
 */
typedef struct _partition {
	
	std::set<int> _nodes;
	std::set<int> _groups;
	
	void join ( struct _partition p) {
	
		std::set<int> _set = p.get_nodes();
		for (int j : _set) {
			add_node (j);
		}
		
		_set = p.get_groups();
		for (int j : _set) {
			add_group (j);
		}
		
	};
	
	void add_node (int n) {
		_nodes.insert (n);
	};
	
	void add_group (int n) {
		_nodes.insert (n);
	};
	
	std::set<int> get_nodes () {
		return _nodes;
	};
	
	std::set<int> get_groups () {
		return _nodes;
	};
	
	
}Partition;

typedef struct lb {
	
	std::vector<shared_ptr<Partition>> partition;
	
}LowerBound;

void join (shared_ptr<Partition> p, shared_ptr<Partition> q);
void create (shared_ptr<Network>);
void lb_calculator (LowerBound & obj);
