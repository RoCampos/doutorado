#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>

#include "network.h"
#include "group.h"
#include "algorithm.h"
#include "rcatime.h"

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

	bool is_source (int i) {
		return std::find (begin(sources), end(sources), i) != end(sources);
	}
	bool is_member (int i) {
		return std::find (begin(members), end(members), i) != end(members);
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
		network.setCost (v, w, groups);
		network.setCost (w, v, groups);
		network.setBand (v, w, groups);
		network.setBand (w, v, groups);
		rca::Link link(v, w, groups);
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

std::vector<rca::Link> 
spanning_tree (rca::Network& network, group_t & group, rca::Network &net2) {

	int NODES = network.getNumberNodes ();
	//sorting edges
	std::vector<rca::Link> edges = network.getLinksUnordered ();
	for (auto & e : edges) {
		e.setValue (network.getCost (e));
	}
	std::sort (begin(edges), end(edges), std::greater<rca::Link>());
	DisjointSet2 dij(NODES);

	std::vector<int> srcs = group.sources;
	std::vector<rca::Link> E;
	std::vector<int> V(NODES);

	cerr << "\tBEGIN" << endl;
	cerr << "\tSOURCE:";
	for (auto s : group.sources) {
		cerr << s << " ";
	}
	cerr << endl;

	cerr << "\tmembers:";
	for (auto m : group.members) {
		cerr << m << " ";
	}
	cerr << endl;

	while (!edges.empty()) {
		rca::Link curr = edges.at (0);
		
		int v = curr.getX(), w = curr.getY ();

		auto res = std::find(srcs.begin(), srcs.end(), v);
		auto res2 = std::find(srcs.begin(), srcs.end(), w);

		if (res != srcs.end() && res2 != srcs.end()) {
			edges.erase (edges.begin());
			continue;
		}

		int s1 = -1, s2 = -1;
		for (auto & i : srcs) { //O(S), S is the number of sources
			if (dij.find2(i) == dij.find2(v)) s1 = i;
			if (dij.find2(i) == dij.find2(w)) s2 = i;
		}

		//se a aresta une dois conjuntos com fontes, então não a considere
		if (s1 != -1 && s2 != -1) {
			edges.erase (edges.begin());
			continue;
		}

		if (dij.find2 (curr.getX()) != dij.find2(curr.getY())) {
			dij.simpleUnion (curr.getX(), curr.getY());
			V[curr.getX()]++;
			V[curr.getY()]++;
			E.push_back (curr);
		}
		edges.erase (edges.begin());
	}

	while (true) {

		auto iter = E.begin();
		auto end = E.end();
		bool resultado = false;

		int size = E.size ();
		while ( iter != end) {
			rca::Link l = *iter;
			if (V[l.getX()] == 1) {
				if (!group.is_member(l.getX()) && !group.is_source(l.getX())) {
					E.erase (iter);
					V[l.getX()] -= 1;
					V[l.getY()] -= 1;
					break;
				}
			}
			if (V[l.getY()] == 1) {
				if (!group.is_member(l.getY()) && !group.is_source(l.getY())) {
					E.erase (iter);
					V[l.getX()] -= 1;
					V[l.getY()] -= 1;
					break;
				}
			}
			iter++;
		}
		if (size == E.size ()) break;
	}
	// cout << endl;

	for (auto e : E) {
		cerr << "\t" << e <<":"<<e.getValue () <<endl;
	}

	cerr << "END" << endl;

	return E;
}


int main(int argc, char const *argv[])
{

	rca::elapsed_time time;
	time.started ();
	
	rca::Network network;

	std::vector<group_t> mgroups;
	std::string file = argv[2];
	std::string sort = argv[4];
	read_instance (file, mgroups, network);
	rca::Network network2(network.getNumberNodes (), network.getNumberEdges ());

	int cost = std::numeric_limits<int>::max();
	int size = 0;
	for (auto g : mgroups) {
		std::vector<rca::Link> f = spanning_tree (network, g, network2);
		size += f.size ();
		for (auto e : f) {
			int c = network.getCost (e.getX(), e.getY())-g.tk;
			network.setCost (e.getX(), e.getY(), c);
			network.setCost (e.getY(), e.getX(), c);
			if (c < cost) {
				cost = c;
			}
		}
	}

	time.finished ();
	float tempo = time.get_elapsed ();
	cout << cost << " " << size << " " << tempo << endl;
	return 0;
}



