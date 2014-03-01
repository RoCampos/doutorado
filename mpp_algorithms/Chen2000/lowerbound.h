#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <memory>
#include <string>
#include "network.h"
#include "reader.h"
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
		_groups.insert (n);
	};
	
	std::set<int> get_nodes () {
		return _nodes;
	};
	
	std::set<int> get_groups () {
		return _groups;
	};
	
} Partition;


/**
 * LowerBound struct to store the partition and the Network
 * 
 */
typedef struct lb {
	
	shared_ptr<Network> net;
	vector<shared_ptr<Partition>> partition;
	
	vector<shared_ptr<Group>> groups;
	
	void init (std::string file) {
		Network _net;
		net = make_shared<Network> (_net);
		
		Reader reader(file);
		reader.configNetwork ( net.get() );
		
		groups = reader.readerGroup ();
	};
	
	void create_partitions();
	
	void joint_partition (int i, int j);
	
	/*encontra o valor de limite, utiliza as funções betha_mn, alpha_mn e sigma_mn*/
	double find_limit ();
	
	/*define o valor de beta*/
	double betha_mn (int,int);
	
	/*calcula o valor de alpha segundo Chen et.al.*/
	double alpha_mn (int,int);
	
	/*Número de arestas que separam a partição m de n*/
	int sigma_mn (int,int);
	
}LowerBound;

