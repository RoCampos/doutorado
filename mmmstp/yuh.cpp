#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <ostream>
#include <algorithm>

#include "config.h"

#include "network.h"
#include "group.h"
#include "path.h" 
#include "algorithm.h"

struct tree_t {
	vector<rca::Path> paths;	
};

struct forest_t  {
	std::vector<tree_t> trees;
};

struct solution_t {
	std::vector<forest_t> forests;
};

struct group_t {
	group_t (int _id) {
		id = _id;
	}

	group_t () {}

	group_t (group_t const & g) {
		id = g.id;
		sources = g.sources;
		members = g.members;
		tk = g.tk;
	}
	group_t & operator=(group_t const & g) {
		id = g.id;
		sources = g.sources;
		members = g.members;
		tk = g.tk;
		return *this;
	}

	int id;
	std::vector<int> sources;
	std::vector<int> members;
	int tk;

	friend std::ostream& operator << (std::ostream & os, group_t const & gt) {
		os << "id: " << gt.id << std::endl;
		os << "sources:";
		for (size_t i = 0; i < gt.sources.size (); ++i)
		{
			os << " " << gt.sources[i];
		}
		os << "\nmembers:";
		for (size_t i = 0; i < gt.members.size (); ++i)
		{
			os << " " << gt.members[i];
		}
		os << "\ntk: " << gt.tk << std::endl;
		return os;
	}

};

void read_group (
	std::string const & file,
	std::vector<group_t> & mgroups,
	rca::Network & network) {

	std::ifstream instance (file.c_str(), std::ifstream::in);
	
	if (!instance.good ()) {
		std::cout << "File not opened" << endl;
		exit (1);
	}

	int nodes, edges, groups;
	instance >> nodes >> edges >> groups;
	
	network.init (nodes, edges);

	double dec = 0.000001;
	for (int i = 0; i < edges; ++i)
	{
		int v, w, c;
		instance >> v >> w >> c;
		network.setCost (v, w, dec);
		network.setCost (w, v, dec);
		network.setCost (v, w, groups);
		network.setCost (w, v, groups);
		rca::Link link(v, w, dec);
		network.insertLink(link);		
		network.addAdjacentVertex(v, w);
		network.addAdjacentVertex(w, v);
		dec += 0.000001;
	}
	
	std::string line;
	std::getline (instance, line);
	for (int i = 0; i < groups; ++i)
	{
		group_t gt;
		gt.id = i;

		std::getline (instance, line);
		std::istringstream iss (line);
		std::string str;
		int j = 1, srcs;
		while (std::getline (iss, str,' ')) {			
			if (j == 1) {
				gt.tk = atoi (str.c_str ());
			} else if (j == 2) {
				srcs = 2 + atoi (str.c_str ());
			} else if (j > 2 && j <= srcs) {
				gt.sources.push_back ( atoi(str.c_str () ) );
			} else if (j > srcs) {
				gt.members.push_back ( atoi(str.c_str () ) );
			}
			j++;
		}
		mgroups.push_back (gt);
	}
	instance.close ();
}

std::string commandLine ()
{
	std::string command = "--inst [brite|yuh]";
	command += " --single [yes|no]";
	command += " --reverse [yes|no]";
	command += " --param [sort|request]";
	return command;
}

// ------------------- IMPLEMENTATION GOES HERE ----------------- //

void widest_shortest_tree (
	forest_t & forest, 
	group_t const & gt,
	rca::Network & network) 
{

	for (size_t i = 0; i < gt.sources.size (); ++i)
	{	
		tree_t tree;
		std::vector<int> previous;
		std::vector<int> bases, costpath;
		std::vector<std::vector<int>> paths;		
		int s = gt.sources[i];
		widest_shortest_path (s, network, bases, costpath, previous, paths);
		for (auto & m : gt.members) {
			rca::Path path = get_shortest_path (s, m, network, previous);
			tree.paths.push_back (std::move(path));	
		}
		forest.trees.push_back (std::move (tree));
	}

}

void widest_path_forest (
	std::vector<group_t> & mgroups,
	rca::Network & network,
	solution_t & solution) 
{
	for (auto & g : mgroups) {
		forest_t forest;		
		widest_shortest_tree (forest, g, network);
		solution.forests.push_back (std::move(forest));
	}
}


int main(int argc, char const *argv[])
{

	rca::Network network;
	std::vector<group_t> mgroups;
	std::string file = argv[1];
	
	solution_t solution;

	read_group (file, mgroups, network);
	widest_path_forest (mgroups, network, solution);

	for (auto & sol : solution.forests) {

		for (auto & tree : sol.trees) {
			for (auto & path : tree.paths) {
				cout << path << endl;
			}		
		}
		cout << "End of Trees"<< endl;		
	}

	return 0;
}