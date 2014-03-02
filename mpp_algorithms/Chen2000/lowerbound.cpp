#include <stdio.h>
#include "lowerbound.h"
#include <algorithm>

void LowerBound::create_partitions () {
	
	int n = net.get()->getNumberNodes ();
	
	for (int i=0; i < n; i++) {
		
		Partition p;
		p.add_node (i);
		
		auto ptr = groups.begin ();
		for (; ptr != groups.end (); ptr++) {
				
			if ((*ptr)->isMember (i)) {
				
				p.add_group ( (*ptr)->getId() );
			}
		}
		
		partition.push_back ( make_shared<Partition> (p) );
	}
}

int LowerBound::sigma_mn (ui i, ui j) {
	
	if (i > partition.size ()-1  || j > partition.size ()-1 ) {
		printf ("Error in Line %d",__LINE__);
	}
	
	Partition m = *partition.at (i);
	Partition n = *partition.at (j);
	
	set<int> m_nodes = m.get_nodes ();
	set<int> n_nodes = n.get_nodes ();
	
	int counter = 0;
	for (int i : m_nodes) {
		for (int j : n_nodes) {
			if (net.get ()->getBand(i,j) > 0) {
				counter++;
			}
		}
	}
	
	return counter;
}

void LowerBound::betha_mn (ui m, ui n) {
	
	
}

void LowerBound::join_partition (ui m, ui n) {
	
	if (m > partition.size ()-1  || m > partition.size ()-1 ) {
		printf ("Error in Line %d",__LINE__);
	}
	
	partition.at (m).get ()->join (*partition.at (n).get ());
	
	auto it = find (partition.begin(), partition.end (), partition.at (n));
	partition.erase (it);
	
}
	


void print_partition (const LowerBound & lb);

int main (int argv, char** argc){

	LowerBound lb;
	std::string str = argc[1];
	lb.init (str);
	lb.create_partitions();
	
	lb.join_partition (0,1);
	cout << lb.sigma_mn (0, 3) << endl;
	
	print_partition (lb);
	
	
	return 0;
}

void print_partition (const LowerBound & lb) {
	
	//printing the partitions
	auto ptr = lb.partition.begin ();
	auto ptr_end = lb.partition.end ();
	
	for (; ptr != ptr_end; ptr++) {
		
		cout << "Partition Members";
		set<int> nodes = ptr->get()->get_nodes ();
		for (int j : nodes) {
			cout << " " << j;
		}
		cout << endl;
		
		cout << "Partition Groups";
		nodes = ptr->get()->get_groups ();
		for (int j : nodes) {
			cout << " " << j;
		}
		cout << endl;
		cout << "----------------------" << endl;
	}
}