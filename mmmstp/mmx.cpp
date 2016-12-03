#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <ostream>
#include <algorithm>
#include <map>

#include "config.h"

#include "network.h"
#include "group.h"
#include "path.h" 
#include "algorithm.h"
#include "rcatime.h"


//----------------------DEFINITIONS -------------------//

struct tree_t {
	
	std::vector<rca::Path> paths;
	std::vector<rca::Link> links;

	rca::Path & find_path (int d) {
		for (auto & p : paths) {
			if (p.at (0) == d){
				return p;
			}
		}
	}
};

struct forest_t  {
	
	std::vector<tree_t> trees;
	std::map<int,int> srcs_id;
	std::map<int,int> srcs;

	forest_t () {}
	forest_t (std::vector<int>& _srcs) {
		for (size_t i = 0; i < _srcs.size (); ++i)
		{
			srcs_id[i] = _srcs[i];
			srcs[_srcs[i]] = i;
		}
	}

};

struct solution_t {
	std::vector<forest_t> forests;
};

struct result_t {
	
	result_t () {
		Z = std::numeric_limits<int>::max();
		Cost = 0;
		Time = 0;
	}

	friend std::ostream& operator<< (ostream & os, result_t const& ref)
	{
		os << ref.Z << "\t" << ref.Cost << "\t" << ref.Time << endl;
		return os;
	}

	int Z;
	int Cost;
	double Time;

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

void read_instance (
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
		network.setBand (v, w, groups);
		network.setBand (w, v, groups);
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


struct GroupLess {
	bool operator () (group_t const & g1, group_t const& g2)
	{
		return g1.tk < g2.tk;
	}
};

struct GroupGreater{
	bool operator () (group_t const & g1, group_t const & g2) {
		return g1.tk > g2.tk;
	}
};


//-------------------------- implementation ----------------//
void build (
	forest_t & forest,
	std::vector<vector<int>> & paths,
	group_t & gt,
	rca::Network & network, 
	result_t & result)
{

	int tk = gt.tk;
	std::vector<rca::Link> links;
	for (auto & m : gt.members) {

		rca::Path path(paths.at (m));
		int psize = path.size ();
		int s = forest.srcs[ path.at(psize-1) ]; //getting the source
		tree_t & tree = forest.trees.at (s);
		tree.paths.push_back (path);
		
		for (int i=0; i < psize-1; ++i) {
			rca::Link link (path[i], path[i+1],0);
			auto res = std::find (links.begin(), links.end(),link);
			if (res == links.end()) {				
				links.push_back (link);
				int band = network.getBand (link.getX(), link.getY());
				band -= tk;
				network.setBand (link.getX(), link.getY(), band);
				network.setBand (link.getY(), link.getX(), band);
				result.Cost += 1;

				if (band < result.Z) {
					result.Z = band;
				}
			}
		}
	}
}


std::string commandLine ()
{
	std::string command = "--inst [yuh]";
	command += " --reverse [yes|no|-]";
	return command;
}

int main(int argc, char const *argv[])
{
	
	if (message(argc, argv, commandLine())) {
		exit (1);
	}

	rca::Network network;
	std::vector<group_t> mgroups;
	
	std::string file = argv[2];
	std::string sort = argv[4];	

	read_instance (file, mgroups, network);

	if (sort.compare ("yes") == 0) {
		std::sort (mgroups.begin(), mgroups.end(), GroupLess());
	} else if (sort.compare("no") == 0) {
		std::sort (mgroups.begin(), mgroups.end(), GroupGreater());
	}

	rca::Network *ptr = network.extend ();

	result_t result;

	rca::elapsed_time time;
	time.started ();
	for (size_t i = 0; i < mgroups.size (); ++i)
	{
		std::vector<int> & srcs = mgroups.at (i).sources;

		std::vector<int> bases;
		std::vector<int> costpath;
		std::vector<std::vector<int>> paths;

		ptr->addPseudoEdges (srcs);
		voronoi_diagram (*ptr, bases, costpath, paths);
		forest_t finalforest (srcs);
		int size = srcs.size ();
		finalforest.trees = std::vector<tree_t> (size);
		build (finalforest, paths, mgroups.at (i), *ptr, result);
		ptr->removePseudoEdges (srcs);

	}
	time.finished ();
	result.Time = time.get_elapsed ();

	cout << result << endl;

	return 0;
}