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


#define ui unsigned int
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
	
	std::set<int> get_nodes () const {
		return _nodes;
	};
	
	std::set<int> get_groups () const {
		return _groups;
	};
	
	friend std::ostream& operator<<(std::ostream& out, const _partition &p);
	
} Partition;

std::ostream& operator<<(std::ostream& out, const Partition &p) {
	
	out << "Nodes" << endl;
	set<int> nodes = p.get_nodes ();
	for (int i : nodes){
		out << i << endl;
	}
	
	nodes = p.get_groups ();
	if (nodes.size () > 0) {
		out << "Groups" << endl;
	
		for (int i : nodes) {
			out << i << endl;
		}
	}	
	return out;
}

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
	
	void join_partition (ui, ui);
	
	/*encontra o valor de limite, utiliza as funções betha_mn, alpha_mn e sigma_mn*/
	double find_limit ();
	
	/*define o valor de beta*/
	double betha_mn (ui,ui);
	
	/*calcula o valor de alpha segundo Chen et.al.*/
	double alpha_mn (ui, ui, int, int);
	
	/**
	 * Método para Calcular o número de arestas que ligam as partições
	 * int m, int n.
	 * @return int
	 */
	int sigma_mn (ui,ui);
	
}LowerBound;

