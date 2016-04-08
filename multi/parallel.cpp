#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <omp.h>

#include <bitset>

#include "network.h"
#include "group.h"
#include "reader.h"
#include "steiner.h"

#include "rcatime.h"

#include "steiner_tree_factory.h"
#include "steiner_tree_observer.h"
#include "sttree_local_search.h"

#include "localsearch.h"

typedef rca::EdgeContainer<rca::Comparator, rca::HCell> CG;
typedef rca::sttalgo::CycleLocalSearch CycleLocalSearch;	

rca::Network * m_network;
// #pragma omp threadprivate (network)

std::vector<rca::Group> groups;
double cost = 0.0;
double resi = 0.0;

std::vector<steiner> bestsolution;

std::vector<int> tabu_list;

void read_network (std::string const & file) {

	std::vector<std::shared_ptr<rca::Group>> g;
	
	m_network = new Network;

	MultipleMulticastReader reader(file);
	reader.configure_unit_values (m_network, g);
	for (std::shared_ptr<rca::Group> i : g) {
		groups.push_back (*i);
	}

	tabu_list = std::vector<int> (g.size ());

}

double update_container (steiner & tree, 
	CG &cg, rca::Group & g, rca::Network & net) {

	double tree_cost = 0;
	
	for (auto & edge : tree.get_all_edges ()) {

		rca::Link l (edge.first, edge.second, 0);
		
		if (cg.is_used (l)) {			
			int value = cg.value (l);
			value -= g.getTrequest ();								
			cg.erase (l);
			l.setValue (value);
			cg.push (l);				
		} else {
			int band = net.getBand(l.getX(), l.getY());				
			l.setValue (band-g.getTrequest ());				
			cg.push (l);
		}

		tree_cost += (int)net.getCost (l.getX(), l.getY());
	}

	return tree_cost;

}

void create_tabu_list () {
	unsigned long int uli = pow( 2.0, (double) groups.size () );
	unsigned long int value = rand () % uli;
	
 	std::bitset<32> str = std::bitset< 32 >( value );
	
	for (size_t i=0; i < tabu_list.size (); i++) {
		if (str[i] == 1) {
			tabu_list[i] = 1;
		} else {
			tabu_list[i] = 0;
		}
	}
}


void busca_local (std::vector<steiner> & sol, 
	rca::Network & net, CG & cg) 
{

	int cost2 = 0;

	for (auto & st: sol){
		cost += (int)st.get_cost ();
	}
	
	//performing cost refinement
	rca::sttalgo::LocalSearch ls(net, groups, cg);	
	int ocost = cost2;
	int z = cg.top ();
	do {				
		ocost = cost2;
		ls.apply (sol, cost2, z);	
	} while (cost2 < ocost);	

	CycleLocalSearch cls(net, groups, cg);	
	z = cg.top ();	
	ocost = cost2;
	do {
		ocost = cost2;
		cls.apply (sol, cost2, z);
	} while (cost2 < ocost);

	#pragma omp critical
	{
		cost = cost2;
		resi = cg.top ();
		bestsolution = sol;
	}

	net.clearRemovedEdges ();
}


void build_solution (std::vector<steiner> & sol, 
	rca::Network & network, CG & cg) {

	rca::sttalgo::ShortestPathSteinerTree<CG, steiner> *factory;
	factory = new rca::sttalgo::ShortestPathSteinerTree<CG, steiner> ();

	int NODES = network.getNumberNodes();
	int GROUPS= groups.size ();

	std::vector<int> index(GROUPS, 0);
	iota (index.begin(), index.end(), 0);
	
	std::random_shuffle (index.begin(), index.end());

	rca::sttalgo::SteinerTreeObserver<CG, steiner> ob;
	ob.set_container (cg);

	
	double cost2 = 0.0;
	#pragma omp parallel
	for (int i : index)
	{

		if (tabu_list[i] == 1 && bestsolution.size () != 0 ) {
			sol[i] = bestsolution[i];
			cost2 += update_container (bestsolution[i], cg, groups[i], network);
			continue;
		}


		steiner tree = steiner(NODES, 
							groups[i].getSource(), 
							groups[i].getMembers());

		ob.set_steiner_tree (tree, NODES);

		rca::sttalgo::remove_top_edges<CG> (cg, network, groups[i], 0);

		factory->build (ob, network, groups[i], cg);
		ob.prune (1,GROUPS);

		cost2 += ob.get_steiner_tree ().get_cost ();
		sol[i] = ob.get_steiner_tree ();

		network.clearRemovedEdges ();

	}


	#pragma omp critical
	{
		if (cg.top () > resi) {
			resi = cg.top ();
			cost = cost2;		

			bestsolution = sol;

			create_tabu_list ();

		} else if (cg.top () == resi && cost2 < cost) {
			resi = cg.top ();
			cost = cost2;

			bestsolution = sol;			

			create_tabu_list ();
		}
	}

	delete factory;
}



int main(int argc, char const *argv[])
{
	using std::cout;
	using std::endl;

	cout << "First running\n";	

	read_network (argv[1]);
	int num_iter = atoi (argv[2]);

	rca::elapsed_time _time_;	

	cout << omp_get_max_threads () << endl;
	omp_set_num_threads (6);
	cout << omp_get_max_threads () << endl;


	#pragma omp parallel shared (resi, cost)
	{		
		rca::Network net = *m_network;
		_time_.started ();
		#pragma omp parallel for
		for (int i = 0; i < num_iter; ++i)
		{
			CG cg;
			cg.init_congestion_matrix (net.getNumberNodes ());
			cg.init_handle_matrix (net.getNumberNodes ());


			std::vector<steiner> v(groups.size ());
			build_solution (v, net,cg);
			// busca_local (v, net, cg);

		}
		_time_.finished ();
	}
	cout << resi << " " << cost << " " << _time_.get_elapsed() << endl;

	return 0;
}