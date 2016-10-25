#include <iostream>
#include <vector>
#include <map>

#include "config.h"

#include "sttree_visitor.h"
#include "minmax.h"
#include "rcatime.h"
#include "sttree_visitor.h"
#include "mpp_visitor.h"
#include "sttree_local_search.h"
#include "localsearch.h"

typedef std::pair<int,int> EdgePair;
typedef std::map<EdgePair, rca::Link> EdgeMap;
typedef rca::EdgeContainer<rca::Comparator, rca::HCell> Container;

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

int update_graph (
	DataSMT * data, 
	rca::Network & net ,
	int tk)
{
	int cost = 0;
	
	std::vector<rca::Link> links;

	for (auto l : data->links) {

		auto it = std::find (links.begin(), links.end(), l);
		if (it == links.end()) {

			int b = net.getBand (l.getX(), l.getY());
			net.setBand (l.getX(), l.getY(), b-tk);

			cost += net.getCost(l);

			links.push_back (l);
		}
	}

	data->links.clear ();
	data->links = std::move (links);

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
		
		if (network.isRemoved(edge)) continue;

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
		
		if (path.size () > 0) {
			links.insert (it, path.begin(), path.end());
		}

		path = path_to_edges (p2, &network);

		auto end = links.begin();
		if (path.size () > 0) {
			links.insert (end, path.begin(), path.end());
		}

		links.push_back (link);

	}

	data->links.clear ();
	data->links = std::move (links);

}

void
remove_top (rca::Network & network, 
	float perc_rem, rca::Group & group) 
{

	Container edgeContainer(network.getNumberNodes ());

	for(auto e : network.getLinks ()) {		
		int band = network.getBand(e.getX(), e.getY());
		edgeContainer.update_inline (e, 
			rca::OperationType::IN, 
			group.getTrequest (), 
			band);
	}

	int count = 0;
	int rem = ((perc_rem * (float)network.getNumberNodes ()));

	auto it = edgeContainer.get_heap ().ordered_begin ();
	auto end = edgeContainer.get_heap ().ordered_end ();
	for ( ; it != end; it++) { 	

		if (count++ == rem) return;

		int b = network.getBand (it->getX(), it->getValue());
		network.removeEdge (*it);
 		if ( !is_connected (network, group) ) {
 			network.undoRemoveEdge (*it);
 		}	
	}
} 

void print_result (int Z, int cost, 
	double time, std::string arg) {

	if (arg.compare ("full") == 0){
		cout << Z << " "<< cost << " ";
		cout << time << endl;
	}else {
		if (arg.compare ("cost") == 0) {
			cout << cost << endl;
		} else {
			cout << Z << endl;
		}
	}

}


int l1 (
	std::vector<steiner> &solution, 
	Container & container,
	std::vector<rca::Group> & mgroups,
	rca::Network & network, 
	int cost) 
{
	rca::sttalgo::LocalSearch ls(network, mgroups, container);	
	int ocost = cost;
	int z = container.top ();
	do {				
		ocost = cost;
		ls.apply (solution, cost, z);	
	} while (cost < ocost);

	return cost;
}

int l2 (
	std::vector<steiner> &solution, 
	Container & container,
	std::vector<rca::Group> & mgroups,
	rca::Network & network,
	int cost) 
{	
	rca::sttalgo::CycleLocalSearch cls(network, mgroups, container);	
	int z = container.top ();
	int ocost = cost;
	do {
		ocost = cost;
		cls.apply (solution, cost, z);	 	
	} while (cost < ocost);

	return cost;
}

int local_search (
	std::string option, 
	std::vector<steiner> &solution, 
	Container & container,
	std::vector<rca::Group> & mgroups,
	rca::Network & network, 
	int cost) 
{

	int custo = 0;

	if (option.compare("local") == 0) {
		custo = l1 (solution, container, mgroups, network, cost);
		return 	l2 (solution, container, mgroups, network, custo);
	} else if (option.compare ("localrev") == 0) {
		custo = l2 (solution, container, mgroups, network, cost);
		return 	l1 (solution, container, mgroups, network, custo);
	}

	return (0);
}

std::string commandLine() {
	std::string command = "--inst [brite|yuh]";
	command+="--rem [double] --reverse [yes|no] --sort [request|size]";
	command+="--local [local|localrev] --result[full, cost, re]";	
	return command;
}

int main(int argc, char const *argv[])
{
	
	if (message (argc, argv, commandLine()) ) {
		exit (1);
	}

	rca::Network network;
	std::vector<rca::Group> mgroups;
	std::string file = argv[2];

	float rem = atof (argv[4]);
	std::string reverse = argv[6];
	std::string sort = argv[8];
	std::string localsearch = argv[10];
	std::string full_res = argv[12];


	rca::reader::get_problem_informations (
		file, network, mgroups);

	rca::Network finalnetwork = network;
	Container container (network.getNumberNodes ());

	rca::elapsed_time time_elapsed;
	time_elapsed.started ();

	if (reverse.compare ("yes") == 0) {		 
		if (sort.compare("request") == 0) {			
			std::sort (mgroups.begin(), mgroups.end(), rca::CompareGreaterGroup());
		} else {			
			std::sort (mgroups.begin(), mgroups.end(), rca::CompareGreaterGroupBySize());
		}
	} else {
		if (sort.compare("request") == 0) {
			std::sort (mgroups.begin(), mgroups.end(), rca::CompareLessGroup());
		} else {
			std::sort (mgroups.begin(), mgroups.end(), rca::CompareLessGroupBySize());
		}
	}	 

	std::vector<steiner> solution;

	int cost = 0;

	for(auto&& group : mgroups) {
		
		std::vector<int> srcs = group.getMembers ();
		srcs.push_back (group.getSource());

		std::vector<int> bases;
		std::vector<int> costpath;
		std::vector<std::vector<int>> paths;

		//remove top
		remove_top (network, rem, group);

		rca::Network *ptr = network.extend (srcs);
		voronoi_diagram (*ptr, bases, costpath, paths);

		DataSMT * data =
			join_components (bases, paths, costpath, *ptr, srcs);
		
		//apply MST over network distance(Z over edges)
		//the calculation uses cost
		minimum_spanning_tree (data);

		//rebuild the solution	
		rebuild_solution (data, paths, network);

		cost += update_graph (data, network, group.getTrequest());

		steiner st = steiner(network.getNumberNodes(), 
				group.getSource(), 
				group.getMembers ());

		for(auto&& e : data->links) {
			st.add_edge (e.getX(), e.getY(), e.getValue());
			int b = finalnetwork.getBand (e.getX(), e.getY());
			container.update_inline (e, 
				rca::OperationType::IN, 
				group.getTrequest (), b);
		}

		solution.push_back (st);

		network.clearRemovedEdges ();

		delete data;
		delete ptr;
	}

	time_elapsed.finished ();
	
	int cost_res = local_search (localsearch, solution, 
		container, mgroups, finalnetwork, cost);
	int z = container.top ();

	print_result (z, cost_res, time_elapsed.get_elapsed (), full_res);

	if (full_res.compare ("full") == 0) {
		//PRINT FOR TEST
		std::vector<steiner> saida = std::vector<steiner> (solution.size());
		for (size_t i = 0; i < solution.size(); ++i)
		{
			saida[mgroups[i].getId()] = solution[i];
		}
		rca::sttalgo::print_solution2<steiner> (saida);	
	}

	return 0;
}

