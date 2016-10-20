#include <iostream>
#include <vector>
#include <map>

#include "minmax.h"
#include "rcatime.h"

typedef std::pair<int,int> EdgePair;
typedef std::map<EdgePair, rca::Link> EdgeMap;

struct DataSMT
{
	rca::Network * G;
	EdgeMap edgeMap;
	std::map<int, int> vertex;
	std::map<int, int> invertex;
	std::vector<rca::Link> links;

	~DataSMT () {
		delete G;
	}

};

int update_graph (DataSMT * data, rca::Network & net)
{
	int cost;
	
	std::vector<rca::Link> links;

	for (auto l : data->links) {

		auto it = std::find (links.begin(), links.end(), l);
		if (it == links.end()) {

			int b = net.getBand (l.getX(), l.getY());
			net.setBand (l.getX(), l.getY(), b-1);

			links.push_back (l);
		}
	}

	// for(auto c : links) {
	// 	cout << c << endl;
	// }

	return cost;
}


int get_min_cap (
	rca::Network & net,
	rca::Path p1, 
	rca::Path p2, 
	rca::Link l) {

	//get_bottleneck from algoritmo.h
	int bp1 = get_bottleneck (net, p1);
	int bp2 = get_bottleneck (net, p2);
	bp1 = std::min (bp1, bp2);
	bp2 =  net.getBand(l.getX(), l.getY());
	bp1 = std::min (bp1,bp2);

	return bp1;	
}

DataSMT * join_components (
	std::vector<int> & bases,
	std::vector<std::vector<int>> & paths,
	std::vector<int> & costpath,
	rca::Network & network, 
	std::vector<int> & srcs) {

	std::map<int, int> vertex, invertex;

	for (size_t i = 0; i < srcs.size (); ++i)
	{
		vertex[ srcs[i] ] = i;
		invertex[ i ] = srcs[i];
	}

	rca::Network * G = new Network (srcs.size (), 0);
	
	//links que formam a rede de distância
	std::vector<rca::Link> tree;
	
	//usando para armazear a aresta que junta duas bases
	EdgeMap edgeMap;

	//all the data do convert to and form network distance
	DataSMT * data = new DataSMT;

	for(auto&& edge : network.getLinks ()) {
		
		int x = bases[edge.getX()];
		int y = bases[edge.getY()];
	
		//if x and y are in different bases
		if (x != y) {			

			int band = network.getBand (edge.getX(), edge.getY());

			//link on network distance
			rca::Link v (vertex[x], vertex[y], band);

			//path that may connect the bases x and y
			rca::Path p1(paths[edge.getX()]);
			rca::Path p2(paths[edge.getY()]);

			//bottleneck of bases x and y (path between them)
			int dist = get_min_cap (network, p1, p2, edge);

			if (G->getBand(v.getX(), v.getY()) == 0) {

				int cost = costpath[edge.getX()];
				cost 	+= costpath[edge.getY()];
				cost 	+= network.getCost (edge);

				G->setCost (v.getX(), v.getY(), cost);
				G->setCost (v.getY(), v.getX(), cost);
				G->setBand (v.getY(), v.getX(), dist);
				G->setBand (v.getX(), v.getY(), dist);

				v.setValue (cost);
				tree.push_back (v);

				//edges can be recovered from vertex and invertex
				//based on index
				edgeMap[EdgePair(v.getX(), v.getY())] = edge;

			} else {

				if (dist > G->getBand(v.getX(), v.getY())) {

					G->setBand (v.getY(), v.getX(), dist);
					G->setBand (v.getX(), v.getY(), dist);

					edgeMap[EdgePair(v.getX(), v.getY())] = edge; 
				}				
			}
		}
	}

	data->G = G;
	data->invertex = std::move(invertex);
	data->vertex = std::move(vertex);
	data->edgeMap = std::move(edgeMap);
	data->links = std::move(tree);

	return data;
}

void
minimum_spanning_tree (DataSMT * data)
{
	//tree resulting from data
	std::vector<rca::Link> result;
	DisjointSet2 ds (data->G->getNumberNodes ());

	//sorting the edges based on cost of the path(edge)
	std::sort (data->links.begin(), data->links.end());

	//bulding minimum spanning tree
	while ( !data->links.empty () ) {
		rca::Link l = data->links[0];
		if (ds.find2(l.getX()) != ds.find2(l.getY())) {
			ds.simpleUnion (l.getX(), l.getY());
			result.push_back (l);
		}
		auto it = data->links.begin();
		data->links.erase (it);
	}

	data->links.clear ();
	data->links = std::move(result);

}

void rebuild_solution (
	DataSMT * data, 
	std::vector<std::vector<int>> & paths,
	rca::Network & network) 
{

	std::vector<rca::Link> links;

	for(auto e : data->links) {

		//getting base vertex
		int bx = e.getX(),  by = e.getY();

		//pair to get eges that connects bases bx and bxy
		std::pair<int,int> p(bx, by);

		//complete path connecting two bases
		rca::Link link = data->edgeMap[p];
		rca::Path p1 (paths[link.getX()]);
		rca::Path p2 (paths[link.getY()]);
		
		auto path = path_to_edges (p1, &network);
		auto it = links.end();
		
		if (path.size () >= 1) {
			links.insert (it, path.begin(), path.end());
		}

		path = path_to_edges (p2, &network);

		auto end = links.begin();
		if (path.size () >= 1) {
			links.insert (end, path.begin(), path.end());
		}

		links.push_back (link);

	}

	data->links.clear ();
	data->links = std::move (links);

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
		
		std::vector<int> srcs = group.getMembers ();
		srcs.push_back (group.getSource());

		std::vector<int> bases;
		std::vector<int> costpath;
		std::vector<std::vector<int>> paths;

		rca::Network *ptr = network.extend (srcs);
		voronoi_diagram (*ptr, bases, costpath, paths);

		DataSMT * data =
			join_components (bases, paths, costpath, *ptr, srcs);
		
		minimum_spanning_tree (data);

		rebuild_solution (data, paths, network);

		update_graph (data, network);

		delete data;
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

	std::map<std::pair<int,int>, rca::Link> mapa;

	mapa[std::pair<int,int>(0,1)] = rca::Link(0,0,0);
	mapa[std::pair<int,int>(1,0)] = rca::Link(0,0,0);

	return 0;
}

