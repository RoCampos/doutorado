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

double LowerBound::betha_mn (ui m, ui n) {
	
	int sigma = sigma_mn (m, n);
	
	int itsctn = intersection (m,n);
	
	return (itsctn / sigma);
}

int LowerBound::intersection (ui m, ui n) {
	
	std::set<int> _m = (*partition.at (m)).get_groups ();
	std::set<int> _n = (*partition.at (n)).get_groups ();
	
	set<int> out;
	
	std::set_intersection (_m.begin (), _m.end (), 
						  _n.begin (), _n.end (),
						  std::inserter (out, out.begin() ));
	
	return ( out.size () );
}

double LowerBound::alpha_mn (ui m, ui n, int par_u, int par_v) {
	
	double betha = betha_mn (m,n);
	
	int tk_m = (*partition.at (m)).get_groups ().size ();
	int tk_n = (*partition.at (n)).get_groups ().size ();
	
	return (betha + par_u *(tk_m + tk_n) + par_v);
}

int LowerBound::group_by_partition (ui m) {

	int counter = 0;
	
	for (auto ptr : partition) {
		
		auto pt = ptr.get ()->get_groups().find (m);
		
		if ( pt != ( ptr.get ()->get_groups().end ()) ) {
			counter++;
		}
		
	}
	
	return (counter-1);
}


void LowerBound::join_partition (ui m, ui n) {
	
	if (m > partition.size ()-1  || m > partition.size ()-1 ) {
		printf ("Error in Line %d",__LINE__);
	}
	
	partition.at (m).get ()->join (*partition.at (n).get ());
	
	auto it = find (partition.begin(), partition.end (), partition.at (n));
	partition.erase (it);
	
}

int LowerBound::delta_PI () {
	
	int delta = 0;
	for (ui i = 0; i < partition.size (); i++) {
		
		for (ui j = (i+1); j < partition.size (); j++) {
			
			set<int> set_m = partition.at (i).get ()->get_nodes ();
			set<int> set_n = partition.at (j).get ()->get_nodes ();
			
			for (int v : set_m) {
				for (int w : set_n) {
					if (net.get ()->getBand (v,w) > 0.0) {
						delta++;
					}
				}
			}
			
		}
		
	}
	
	return (delta);
}

void print_partition (const LowerBound & lb);

int main (int argv, char** argc){

	LowerBound lb;
	std::string str = argc[1];
	lb.init (str);
	lb.create_partitions();
	
	
	
	//lb.join_partition (0,1);
	//cout << lb.sigma_mn (0, 3) << endl;
	//cout << lb.betha_mn (0,1) << endl;
	//cout << lb.intersection (0,1) << endl;
	//cout << lb.alpha_mn (0,1,1,0) << endl;
	
	//print_partition (lb);
	
	//printf ("Delta Value: %d\n",lb.delta_PI ());
	
	
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