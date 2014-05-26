#include <stdio.h>
#include <climits>
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
	
	double sigma = sigma_mn (m, n);
	
	if (sigma == 0)
		return sigma;
	
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

int LowerBound::intsec_by_edge (ui m, ui n) {
	
	set<int> set_m = partition.at (m).get ()->get_nodes ();
	set<int> set_n = partition.at (n).get ()->get_nodes ();
			
	int intsec = 0;
	for (int v : set_m) {
		for (int w : set_n) {
			if (net.get ()->getBand (v,w) > 0.0) {
				intsec++;
			}
		}
	}
	return intsec;
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
		
		set<int> groups_m = ptr.get ()->get_groups();
		for (ui i : groups_m) {
			if (i == m) {
				counter++;
			}
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

double LowerBound::find_limit () {
	
	int denominator = 0;
	for (ui i=0; i < groups.size (); i++) {	
		//cout << group_by_partition (i) << " ";
		denominator += group_by_partition (i);
	}
	//cout << endl;
	
	double d_PI = delta_PI ();
	//cout << denominator << " " << d_PI << endl;
	return (denominator/d_PI);
}

void print_partition (const LowerBound & lb);

int main (int argv, char** argc){

	LowerBound lb;
	std::string str = argc[1];
	lb.init (str);
	lb.create_partitions();
	
	//parameters from Chen et. al.
	//print_partition (lb);
	
	int par_v = 1;
	int par_u = 0;
	
	double limit = 0.0;
	
	while (lb.partition.size () > 2) {

		//printf ("Limit %lf:\n", lb.find_limit ());
		limit = lb.find_limit ();
		
		double min_alpha = INT_MAX;
		int m = -1, n = -1;
		
		for (ui i=0; i < lb.partition.size (); i++) {
			
			for (ui j=(i+1); j < lb.partition.size (); j++) {
		
				if (lb.intsec_by_edge (i,j) == 0) continue;
				
				double alpha = lb.alpha_mn (i,j, par_v, par_u);
				//printf ("Alpha: (%d,%d) = %f\n",i,j,alpha);
				if (alpha < min_alpha) {
					min_alpha = alpha;
					m = i;
					n = j;
				}
				
			}
		}
		
		if (m == n) {
			cout << "error!" << endl;
			exit (1);
		}
		
		/*
		printf ("Alpha: %f\n", min_alpha);
		cout << "joining ";
		lb.partition.at (m)->print_nodes ();
		cout << "to ";
		lb.partition.at (n)->print_nodes ();
		cout << "=(";
		lb.partition.at (m)->print_nodes ();
		lb.partition.at (n)->print_nodes ();
		cout << ")"<<endl;
		*/
		lb.join_partition (m,n);
		
	}
	
	//printf ("%d\n",lb.intsec_by_edge (2,4));
	
	//lb.join_partition (0,1);
	/*
	cout << lb.sigma_mn (4,2) << endl;
	cout << lb.betha_mn (4,2) << endl;
	cout << lb.intersection (4,2) << endl;
	cout << lb.alpha_mn (2,4,1,0) << endl;
	cout << *lb.partition.at (2) << endl;
	cout << *lb.partition.at (4) << endl;
	
	print_partition (lb);
	*/
	
	printf ("Limit %lf:\n", limit);
	/**
	 * Print partition method.
	 */
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