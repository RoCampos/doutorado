#ifndef __MFC_H__
#define __MFC_H__

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>

#include "network.h"
#include "group.h"
#include "algorithm.h"

void build_intermediate_nodes ();

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
		network.setCost (v, w, c+dec);
		network.setCost (w, v, c+dec);
		network.setBand (v, w, groups);
		network.setBand (w, v, groups);
		rca::Link link(v, w, c+dec);
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

using Map = std::map<rca::Link,std::tuple<rca::Path, int, int>>;

Map build_complete_graph (
	rca::Network & network, 
	group_t & group) {

	std::map<rca::Link,std::tuple<rca::Path, int, int>> map;

	std::vector<int> V = group.sources;
	V.insert (V.end(), begin(group.members), end(group.members));

	complete_graph (network, V, map, all_widest_path);

	return map;
}

std::vector<rca::Link> build_spanning_tree (
	rca::Network & network, 
	group_t & group,
	Map & map) 
{

	std::vector<int> V = group.sources;
	V.insert (V.end(), begin(group.members), end(group.members));

	int NODES = V.size ();
	int EDGES = (NODES * (NODES - 1))/2;
	std::map<int,int> nodes;
	
	for(int i=0; i < NODES; i++){
		nodes[V[i]] = i;
	}
	rca::Network G;
	G.init (NODES, EDGES);

	float dec = 0.000000001;
	for (auto edge : map) {
		int v = nodes[edge.first.getX()];
		int w = nodes[edge.first.getY()];
		int cost = std::get<1>(edge.second);
		int band = std::get<2>(edge.second);
		G.setCost (v, w, cost);
		G.setCost (w, v, cost);
		G.setBand (v, w, band);
		G.setBand (w, v, band);
		rca::Link link(v, w, cost + dec);
		G.insertLink(link);		
		G.addAdjacentVertex(v, w);
		G.addAdjacentVertex(w, v);
		dec += 0.00000001;
	}

	std::vector<rca::Link> edgelist;
	std::vector<int> N = group.sources;
	spanning_minimal_tree (G, edgelist, N);
	return edgelist;

}

std::vector<rca::Path> unpack_paths(
	rca::Network & network,
	group_t & group,
	std::vector<rca::Link> edgelist,
	Map & map) 
{
	std::map<int,int> nodes;
	std::vector<int> V = group.sources;
	V.insert (V.end(), begin(group.members), end(group.members));

	int NODES = V.size ();
	for(int i=0; i < NODES; i++){
		nodes[V[i]] = i;
		V.push_back (i);
	}

	std::vector<rca::Path> result;
	for (auto x : edgelist) {
		for (auto e : map) {
			int v = nodes[e.first.getX()];
			int w = nodes[e.first.getY()];
			rca::Link y(v, w, 0);
 			if (x.getX() == y.getX() && x.getY() == y.getY()) {
				result.push_back (std::get<0>(e.second));
			}
		}
		
	}

	//returning the paths
	return result;
	
}

std::tuple<int,int> update_network (
	rca::Network & network, group_t & group,
	std::vector<rca::Path>& paths)
{

	std::tuple<int,int> result;


	int cost = 0;
	int Z = std::numeric_limits<int>::max();

	std::vector<rca::Link> links;

	for (auto p : paths) {
		std::vector<rca::Link> curr;
		curr = path_to_edges (p, NULL);
		for (auto l : curr) {
			if (std::find(begin(links), end(links),l) == end(links)) {
				links.push_back (l);
				cost += network.getCost (l.getX(), l.getY());

				int band = network.getBand (l.getX(), l.getY());
				band -= group.tk;

				network.setBand (l.getX(), l.getY(), band);
				network.setBand (l.getY(), l.getX(), band);

				if (network.getBand(l.getX(), l.getY()) < Z) {
					Z = network.getBand (l.getX(), l.getY());
				}

				cout << l << endl;
			}
		}
	}
	result = std::make_tuple(cost, Z);
	return result;
}


int main(int argc, char const *argv[])
{

	rca::Network network;
	std::vector<group_t> mgroups;
	
	std::string file = argv[1];
	std::string sort = argv[2];

	read_instance (file, mgroups, network);

	int cost = 0;
	int Z = std::numeric_limits<int>::max();
	for (auto g : mgroups) {
		Map map = build_complete_graph (network, g);
		auto edgelist = build_spanning_tree (network, g, map);
		std::vector<rca::Path> paths = unpack_paths (network, g, edgelist, map);
		std::tuple<int, int> result = update_network (network, g, paths);

		cost += std::get<0>(result);
		if (std::get<1>(result) < Z) {
			Z = std::get<1>(result);
		}
	}

	cout << cost << " " << Z << endl;

	return 0;
}
#endif