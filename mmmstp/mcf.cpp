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

//map contains key = e(i,j) onde i e j são nós do grafo real 
std::tuple<Map, std::vector<int>>
build_complete_graph (
	rca::Network & network, 
	group_t & group, 
	std::string opt) {

	// cout << __FUNCTION__ << endl;

	std::map<rca::Link,std::tuple<rca::Path, int, int>> map;

	std::vector<int> V = group.sources;
	V.insert (V.end(), begin(group.members), end(group.members));

	if (opt.compare ("cost") == 0) {
		complete_graph (network, V, map, all_shortest_path);
	} else {
		complete_graph (network, V, map, all_widest_path);	
	}

	std::vector<int> internodes;

	std::tuple<Map, std::vector<int>>
		result = std::make_tuple(map, internodes);

	return result;
}

std::tuple<Map, std::vector<int>> 
build_intermediate_nodes (
	rca::Network & network,
	group_t & group,
	std::string opt) 
{

	//map describing complete graph
	Map map;

	int NODES = network.getNumberNodes ();

	// for (int i = 0; i < NODES; ++i)
	// {
	// 	cout << i << " "<< network.get_neighboors (i).size () << endl;
	// }


	//mapped nodes of the graph
	//source S_i(i=1) is mapped to i=1 and so on..
	std::vector<int> V = group.sources;
	V.insert (V.end(), begin(group.members), end(group.members));

	int count = 0;
	for (auto s : group.sources){
		std::vector<int> prev = all_widest_path (s, 0, network);
		for (auto m : group.members) {
			rca::Link e(s, m, ++count);

			rca::Path p = get_shortest_path (s,m, network, prev);
			int cost = 0, bottleneck = std::numeric_limits<int>::max();
			//getting cost and bottleneck of the path
			for (auto iter =p.begin (); iter != p.end()-1; iter++) {
				cost += network.getCost (*iter, *(iter+1));
				if (bottleneck > network.getBand (*iter, *(iter+1))) {
					bottleneck = network.getBand (*iter, *(iter+1));
				}
			}
			map[e] = std::make_tuple(p,cost,bottleneck);
		}
	}

	for (auto d1 : group.members) {
		std::vector<int> prev = all_widest_path (d1, 0, network);
		for (auto d2 : group.members) {
			if (d1 < d2) {
				rca::Link e(d1, d2, ++count);

				rca::Path p = get_shortest_path (d1,d2, network, prev);
				int cost = 0, bottleneck = std::numeric_limits<int>::max();
				//getting cost and bottleneck of the path
				for (auto iter =p.begin (); iter != p.end()-1; iter++) {
					cost += network.getCost (*iter, *(iter+1));
					if (bottleneck > network.getBand (*iter, *(iter+1))) {
						bottleneck = network.getBand (*iter, *(iter+1));
					}
				}
				map[e] = std::make_tuple(p,cost,bottleneck);
			}
		}
	}

	//calculando nós intermediários
	std::vector<int> internodes(NODES);
	for (auto e : map) {
		rca::Path path = std::get<0>(e.second);
		for (auto v : path) {
			if (!group.is_source(v) && !group.is_member (v)) {
				internodes[v] = 1;
			}
		}
	}

	//nodes to be added to map
	std::vector<int> P;
	for (int i = 0; i < internodes.size (); ++i)
	{
		int degree = 0;
		for (int j = 0; j < internodes.size (); ++j) {
			if (i < j && internodes[i] == 1 && internodes[j] == 1) {
				if (network.getCost (i,j) != 0) {
					degree++;
				}
			}
		}
		if (degree > 3) {
			P.push_back (i);
		}
	}
		
	std::tuple<Map, std::vector<int>> result = std::make_tuple(map, P);

	return result;
}

std::vector<rca::Link> build_spanning_tree (
	rca::Network & network, 
	group_t & group,
	std::tuple<Map, std::vector<int>> & data, 
	//data includes map(edges of complete graph) and internodes
	std::string opt) 
{
	// cout << __FUNCTION__ << endl;

	std::vector<int> V = group.sources;
	V.insert (V.end(), begin(group.members), end(group.members));

	//addint shared nodes
	for (auto inter : std::get<1>(data)) {
		V.push_back (inter);
	}


	int NODES = V.size ();
	int EDGES = (NODES * (NODES - 1))/2;
	std::map<int,int> nodes;
	
	std::vector<int> N;
	for(int i=0; i < NODES; i++){
		nodes[V[i]] = i;
		if (i < group.sources.size ()) {
			N.push_back (i);
		}
	}
	rca::Network G;
	G.init (NODES, EDGES);

	float dec = 0.00001;
	for (auto edge : std::get<0>(data)) { //map conteins edges based on real vertex
		int v = nodes[edge.first.getX()];
		int w = nodes[edge.first.getY()];

		int cost = -1;

		if (opt.compare ("Z") == 0) {
			cost = std::get<2>(edge.second) * -1;
		} else {
			cost = std::get<1>(edge.second);			
		}
		int band = std::get<2>(edge.second);
		G.setCost (v, w, cost);
		G.setCost (w, v, cost);
		G.setBand (v, w, band);
		G.setBand (w, v, band);
		if (opt.compare ("Z") == 0) {
			rca::Link link(v, w, cost);
			G.insertLink(link);
		} else {
			rca::Link link(v, w, cost);			
			G.insertLink(link);
		}
		// rca::Link link(v, w, cost + dec);
		// G.insertLink(link);	
		G.addAdjacentVertex(v, w);
		G.addAdjacentVertex(w, v);
		dec += 0.00001;
	}

	std::vector<rca::Link> edgelist;
	spanning_minimal_tree (G, edgelist, N);

	// std::vector<rca::Link> GEdges = G.getLinksUnordered ();
	// cout << "\tComplet Graph: " << G.getLinksUnordered ().size () << endl;
	// for (auto p : map) {
	// 	cout << "\t" << p.first << ":" << std::get<0>(p.second) << endl;
	// }


	// cout << "\tMembers(unre): ";
	// for (auto v : nodes) {
	// 	cout << v.first << "(" << v.second << ") ";
	// }
	// cout << endl;
	// cout << "\tSpanning Minimal Tree: " << edgelist.size () << endl;
	// cout << "\tMembers(real): ";
	// for (auto v : V) {
	// 	cout << v << " ";
	// }
	// cout << endl;
	// int c = 0;

	// //edge list representa o valor do nó mapeado 0 (13) 1(3)
	// for (auto e : edgelist) {
	// 	bool found = true;
	// 	for (auto p: map) {
	// 		int x = p.first.getX();
	// 		int y = p.first.getY();
	// 		rca::Link l (V[e.getX()], V[e.getY()], 0);
	// 		int _x = V[e.getX()]; 
	// 		int _y = V[e.getY()];
	// 		if (x == l.getX() && y == l.getY()) {
	// 			cout << "\t"<< e << "(" << _x << "-" << _y <<"):";
	// 			cout << std::get<0>(p.second) << endl;
	// 			c++;
	// 			found = false;
	// 			break;
	// 		}
	// 	}
	// 	int _x = V[e.getX()]; 
	// 	int _y = V[e.getY()];
	// 	if (found) cout << "\t" <<e << "(" << _x << "-" << _y <<"): [ ] ";
	// 	if (std::find (begin(GEdges), end(GEdges), e) == end(GEdges)) cout << "not in G" << endl;
	// 	else cout << endl;
	// }
	// cout << "\tPaths: " << c << endl; 

	return edgelist;

}

std::vector<rca::Path> unpack_paths(
	rca::Network & network,
	group_t & group,
	std::vector<rca::Link> edgelist,
	std::tuple<Map, std::vector<int>> & data)
	//data includes map(edges of complete graph) and internodes
{

	// cout << __FUNCTION__ << endl;

	std::map<int,int> nodes;
	std::vector<int> V = group.sources;
	V.insert (V.end(), begin(group.members), end(group.members));

	//addint shared nodes
	for (auto inter : std::get<1>(data)) {
		V.push_back (inter);
	}

	int NODES = V.size ();
	for(int i=0; i < NODES; i++){
		nodes[V[i]] = i;
	}

	std::vector<rca::Path> result;
	for (auto x : edgelist) { //vertex are coded yet
		for (auto e : std::get<0>(data)) { //edge from map are mapped 
			//vertex in V are stored in the same order as they are readed
			int v = e.first.getX(); //from the code, getting the real vertex
			int w = e.first.getY();
			int v_ = V[x.getX()];
			int w_ = V[x.getY()];
			rca::Link y(v, w, 0);
			rca::Link z(v_,w_, 0);
 			if (z.getX() == y.getX() && z.getY() == y.getY()) {
 				// cout << "\t" << x << "("<< y<<"):" << std::get<0>(e.second) << endl;
				result.push_back (std::get<0>(e.second));
				break;
			}
		}
	}

	//returning the paths
	return result;
	
}

std::tuple<int,int,int> update_network (
	rca::Network & network, group_t & group,
	std::vector<rca::Path>& paths)
{

	// cout << __FUNCTION__ << endl;

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

	std::tuple<int,int,int> result;
	
	std::vector<rca::Link> links;

	//selection paths
	for (auto p : paths) {
		std::vector<rca::Link> curr;
		curr = path_to_edges (p, NULL);
		for (auto l : curr) {
			if (std::find(begin(links), end(links),l) == end(links)) {
				int band = network.getBand (l.getX(), l.getY());
				l.setValue (band * -1);
				links.push_back (l);

				// cout <<"\t"<< l << endl;
			}
		}
	}

	// cout << endl;

	//to get degrees
	std::vector<int> N(network.getNumberNodes ());
	std::vector<rca::Link> E;
	spanning_minimal_tree (network, links, E, group.sources, N);


	// for (int i = 0; i < N.size (); ++i) {
	// 	cout << i << " " << N[i] << endl;
	// }

	while (true) {

		auto iter = E.begin();
		auto end = E.end();
		bool resultado = false;

		int size = E.size ();
		while ( iter != end) {
			rca::Link l = *iter;
			if (N[l.getX()] == 1) {
				if (!group.is_member(l.getX()) && !group.is_source(l.getX())) {
					E.erase (iter);

					// cout << *iter << endl;
					N[l.getX()] -= 1;
					N[l.getY()] -= 1;
					break;
				}
			}
			if (N[l.getY()] == 1) {
				if (!group.is_member(l.getY()) && !group.is_source(l.getY())) {
					E.erase (iter);

					// cout << *iter << endl;
					N[l.getX()] -= 1;
					N[l.getY()] -= 1;
					break;
				}
			}
			iter++;
		}
		if (size == E.size ()) break;
	}

	// for (int i = 0; i < N.size (); ++i) {
	// 	cout << i << " " << N[i] << endl;
	// }

	int Z = std::numeric_limits<int>::max();
	for (auto e : E) {
		int band = network.getBand (e.getX(), e.getY());
		band -= group.tk;
		network.setBand (e.getX(), e.getY(), band);
		network.setBand (e.getY(), e.getX(), band);

		cerr << "\t" << e << endl;

		if (band < Z) {
			Z = band;
		}
	}

	cerr << "\tEND" << endl;

	result = std::make_tuple(0, Z, E.size ());
	return result;
}

void help () {
	cout << "Executa o algoritmo https://goo.gl/DAWGpe que faz roteamento multicast com múltipla"<< endl;
	cout << "fonte e uma sessão. O algoritmo foi adaptado para rodar com múltiplas"<< endl;
	cout << "sessões e também otimizar a capacidade residual" << endl;

	cout << "\nUsage" << endl;
	cout << "\n\tmcf <instance>[yuh] <sort>[asc|dec|normal] <objective>[Z|cost]";
	cout << "\n\tinstance\tinstancia do tipo yuh";
	cout << "\n\tsort\tindica a ordem dos grupos multicast";
	cout << "\n\tobjective\t indica se vai otimizar o custo ou capacidade residual Z";

	cout << "\nExample\n\tmcf teste.yuh normal Z\n";
}

int main(int argc, char const *argv[])
{

	rca::elapsed_time time;
	time.started ();

	rca::Network network;
	std::vector<group_t> mgroups;
	
	if (argc < 4) {
		help ();
		exit (0);
	}

	std::string file = argv[2];
	std::string sort = argv[4];
	std::string opt = argv[6];

	read_instance (file, mgroups, network);

	int cost = 0;
	int Z = std::numeric_limits<int>::max();
	float tempo11 = 0;
	for (auto g : mgroups) {
		
		// std::tuple<Map, std::vector<int>> 
		// map = build_complete_graph (network, g, opt);
		
		std::tuple<Map, std::vector<int>> 
		map = build_intermediate_nodes (network, g, opt);

		// exit (0);

		auto edgelist = 
			build_spanning_tree (network, g, map, opt);
		
		std::vector<rca::Path> paths = 
			unpack_paths (network, g, edgelist, map);

		//cost, Z, number of links
		std::tuple<int, int, int> result = 
			update_network (network, g, paths);

		if (opt.compare("Z") == 0){
			cost += std::get<2>(result); //getting the number of link
		} else {
			cost += std::get<0>(result);	
		}

		if (std::get<1>(result) < Z) {
			Z = std::get<1>(result);
		}

		// getchar ();

	}

	time.finished ();
	float tempo = time.get_elapsed ();
	
	cout << cost << " " << Z << " " << tempo << endl;

	return 0;
}
#endif