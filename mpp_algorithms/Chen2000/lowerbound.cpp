#include <stdio.h>
#include "lowerbound.h"

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
	
	
	
	return 0;
}

void print_partition (const LowerBound & lb);

int main (void){

	LowerBound lb;
	lb.init ("/home/romerito/workspace/Doutorado/instances/MMPP-Instances/n30/b30_1.brite");
	
	lb.create_partitions();
	
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