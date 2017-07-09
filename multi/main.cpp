#include <iostream>
#include <string>

#include "individual.h"
#include "operator.h"
#include "generic_individual.h"

#include "network.h"
#include "group.h"
#include "reader.h"
#include "algorithm.h"
#include "link.h"

int main(int argc, char const *argv[])
{

	srand (time(NULL));

	rca::Network network;
	std::vector<rca::Group> mgroups;
	std::string file = argv[1];
	std::string conf = argv[2];

	std::vector<std::shared_ptr<rca::Group>> g;
	rca::reader::MultipleMulticastReader reader(file);
#ifdef MODEL_REAL
	reader.configure_real_values (&network,g);
#endif	
#ifdef MODEL_UNIT
	reader.configure_unit_values (&network,g);
#endif

	for (std::shared_ptr<rca::Group> i : g) {
		mgroups.push_back (*i);
	}

	int NODES = network.getNumberNodes ();

	Info::Problem::init (file, conf);

	BestFirstSearch stp;
	Individual ind (mgroups.size (), 3);
	for (size_t i=0; i < mgroups.size (); i++) {
		int src = mgroups[i].getSource ();
		std::vector<int> members = mgroups[i].getMembers ();
		DTree dtree;		
		members.push_back (src);
		int pos = rand () % members.size ();
		src = members.at (pos);
		members.erase (members.begin ()+pos);
		stp.execute (src, members, network, dtree.paths);	
		reorganize (src, pos, i, dtree, mgroups, network);
		ind.add_tree (i, dtree);			
	}
	evaluate (ind, network, mgroups);

	Individual ind2 (mgroups.size (), 3);
	for (size_t i=0; i < mgroups.size (); i++) {
		int src = mgroups[i].getSource ();
		std::vector<int> members = mgroups[i].getMembers ();
		DTree dtree;		
		members.push_back (src);
		int pos = rand () % members.size ();
		src = members.at (pos);
		members.erase (members.begin ()+pos);
		stp.execute (src, members, network, dtree.paths);	
		reorganize (src, pos, i, dtree, mgroups, network);
		ind2.add_tree (i, dtree);			
	}
	evaluate (ind2, network, mgroups);

	ind.str ();
	ind2.str ();

	Individual novo (mgroups.size (), 3);
	crossover_by_edge (ind, ind2, novo);
	evaluate (novo, network, mgroups);
	novo.str ();
	mutation (novo);
	novo.str ();

	return 0;
}
