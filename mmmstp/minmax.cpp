#include <iostream>
#include <vector>

#include "minmax.h"
#include "rcatime.h"

int update_graph (rca::Path const& p, 
	rca::Network& net, 
	int tk, 
	std::vector<rca::Link> & links) {

	int cost = 0;

	for (int i = 0; i < p.size ()-1; ++i)
	{
		int v = p.at (i);
		int w = p.at (i+1);
		rca::Link l (v, w, (int)net.getCost (v, w));
		if (std::find(links.begin(), links.end(), l) == links.end()) {
			links.push_back (l);
			int band = net.getBand (v,w)-tk;
			net.setBand (v, w, band);
			net.setBand (w, v, band);

			cost += net.getCost (v,w);

		} 
	}
	return cost;
}

int main(int argc, char const *argv[])
{
	
	rca::Network network;
	std::vector<rca::Group> mgroups;
	std::string file = argv[1];

	rca::reader::get_problem_informations (
		file, network, mgroups);

	rca::elapsed_time time_elapsed;
	time_elapsed.started ();

	int cost = 0;
	for(auto&& group : mgroups) {
		
		std::vector<int> srcs = {};
		srcs.push_back (group.getSource());

		std::vector<int> bases;
		std::vector<int> costpath;
		std::vector<std::vector<int>> paths;

		rca::Network *ptr = network.extend (srcs);
		voronoi_diagram (*ptr, bases, costpath, paths);

		std::vector<rca::Link> arestas;
		for(auto m : group.getMembers ()) {
			rca::Path p (paths[m]);
			cost += update_graph (p, network, group.getTrequest (), arestas);
		}
		delete ptr;
	}

	time_elapsed.finished ();
	

	int Z = std::numeric_limits<int>::max();
	auto links = network.getLinks ();
	for (auto & e : links) {
		if (network.getBand (e.getX(), e.getY()) < Z) {
			Z = network.getBand (e.getX(), e.getY());
		}
	}
	cout << Z << " "<< cost << " ";
	std::cout << time_elapsed.get_elapsed () << std::endl;

	return 0;
}

