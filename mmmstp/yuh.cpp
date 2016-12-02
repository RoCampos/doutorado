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

std::string commandLine ()
{
	std::string command = "--inst [brite|yuh]";
	command += " --single [yes|no]";
	command += " --reverse [yes|no]";
	command += " --param [sort|request]";
	return command;
}

// ------------------- IMPLEMENTATION GOES HERE ----------------- //

rca::Link get_bottleneck_link (
	rca::Network & network, 
	rca::Path & path) 
{

	auto it = path.cbegin ();
	int min_bottleneck = INT_MAX;
	rca::Link l;
	for ( ; it != path.cend() -1; it++) {		
		int band = network.getBand( *it , *(it+1) );		
		if ( band < min_bottleneck) {
			min_bottleneck = band;
			l = rca::Link( *it , *(it+1), band);
		}
	}	
	return l;

}

int next_node (rca::Path & path, int current_node)
{
	
	auto it = path.begin ();
	for (;it != path.end(); it++) {
		
		if (*it == current_node)
			break;
	}
	
	return (it != path.end() ? *(it+1) : -1);
	
}

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

void WPforest (
	group_t & g,
	rca::Network & network,
	forest_t & finalforest) 
{
	
	forest_t forest(g.sources);

	widest_shortest_tree (forest, g, network);

	std::vector<int> & srcs = g.sources;
	std::vector<rca::Path> group_sol;

	for (auto& m : g.members) {

		int current_node = m;

		rca::Path path;
		path.push (current_node);

		while (true) {
			tree_t & tree = forest.trees.at (0);			
			rca::Path & path_m_s = tree.find_path (m);

			int next_vertex = next_node ( path_m_s , current_node);
			rca::Link next_width = get_bottleneck_link (network, path_m_s);

			for (size_t i = 1; i < forest.trees.size (); ++i)
			{
				tree_t & tree_i = forest.trees.at (i);
				rca::Path & path_m_i = tree_i.find_path (m);
				rca::Link link_i = get_bottleneck_link (network, path_m_i);

				if (link_i.getValue () > next_width.getValue ()) {				
					next_vertex = next_node ( path_m_i, current_node );
					next_width.setValue ( link_i.getValue() );					
				}
			}

			//updating current node
			current_node = next_vertex;			
			//adding current_node to the path
			path.push (current_node);

			auto res = std::find (srcs.begin (), srcs.end(), current_node);
			if (res != srcs.end()) {
				group_sol.push_back (path);

				int s = forest.srcs[current_node];
				tree_t & t = finalforest.trees.at (s);
				t.paths.push_back (path);				
				break;
			}			
		}		
	}
}

int update_usage (
	rca::Network & network,
	group_t & group,
	forest_t & forest)
{

	int Z = std::numeric_limits<int>::max ();

	int tk = group.tk;
	for (auto & tree : forest.trees) {
		std::vector<rca::Link> links;
		for (auto & path : tree.paths) {
			for (size_t i=0; i < path.size ()-1; i++) {
				rca::Link l (path.at (i), path.at (i+1), 0);

				auto res = std::find (links.begin (), links.end(), l);
				if (res == links.end()) {
					links.push_back (std::move(l));
					int band = network.getBand (l.getX(), l.getY());
					band = band - tk;
					network.setBand (l.getX(), l.getY(), band);
					network.setBand (l.getY(), l.getX(), band);

					if (band < Z) {
						Z = band;
					}
				}
			}
		}

		tree.links = std::move (links);
	}
	return Z;
}


int main(int argc, char const *argv[])
{

	rca::Network network;
	std::vector<group_t> mgroups;
	std::string file = argv[1];
	
	solution_t solution;

	read_group (file, mgroups, network);

	int Z = std::numeric_limits <int>::max();

	for (size_t i = 0; i < mgroups.size (); ++i)
	{
		forest_t finalforest(mgroups.at (i).sources);		
		int size = mgroups.at (i).sources.size ();
		finalforest.trees = std::vector<tree_t> (size);
		WPforest (mgroups.at (i), network, finalforest);
		int curr_Z =  update_usage (network, 
								mgroups.at (i), 
								finalforest);

		if (curr_Z < Z) {
			Z = curr_Z;
		}
	}


	cout << Z << endl;

	
	

	return 0;
}