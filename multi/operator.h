#ifndef OPERATOR_H
#define OPERATOR_H

#include <iostream>
#include <map>
#include <vector>

#include "network.h"
#include "path.h"
#include "group.h"
#include "algorithm.h"

int OBJ = 3;

struct ShortestPathTree
{
	ShortestPathTree (){}

	void execute (int source, 
		std::vector<int> const & members,
		rca::Network & network, 
		std::vector<rca::Path> & tree) 
	{
		std::vector<std::vector<int>> paths;
		std::vector<int> cpath;
		std::vector<int> bases;
		shortest_path (source, network, bases, cpath, paths);

		for (int m : members) {				
			paths[m].insert (paths[m].begin(), source);
			rca::Path path (std::move (paths[m]));
			tree.push_back (path);
		}
	}
};

struct WidestPathTree
{
	WidestPathTree () {}

	void execute (int source, 
		std::vector<int> const & members,
		rca::Network & network, 
		std::vector<rca::Path> & tree, int tk) 
	{
		std::vector<std::vector<int>> paths;
		std::vector<int> cpath;
		std::vector<int> bases;
		widest_shortest_path (source, network, bases, cpath, paths);

		for (int m : members) {			
			rca::Path path (std::move (paths[m]));			
			tree.push_back (path);
		}

		eval (network, tree, tk);
	}

	void eval (rca::Network & network, 
		std::vector<rca::Path> & tree, 
		int tk) 
	{
		std::vector<rca::Link> curr_tree;

		for (auto & p : tree) {

			for (size_t i = 0; i < p.size ()-1; ++i)
			{
				int v = p.at (i);
				int w = p.at (i+1);
				rca::Link l (v, w, 0);

				//if the edge is not in the curr_tree
				auto res = std::find (curr_tree.begin (), curr_tree.end(), l);
				if (res == curr_tree.end()) {
					curr_tree.push_back (l);
					int band = network.getBand (l.getX(), l.getY()) - tk;
					network.setBand (l.getX(), l.getY(), band);
					network.setBand (l.getY(), l.getX(), band);
				}
			}				
		}
	}

};

struct BestFirstSearch
{
	BestFirstSearch () {}

	void execute (int source, 
		std::vector<int> const & members,
		rca::Network const & network, 
		std::vector<rca::Path> & tree)
	{
		std::vector<std::vector<int>> paths;
		std::vector<int> cpath;
		std::vector<int> bases;
		best_first_search (source, network, bases, cpath, paths);

		for (int m : members) {			
			rca::Path path (std::move (paths[m]));
			tree.push_back (path);
		}
	}
};

void evaluate (Individual & ind, 
	rca::Network & network, 
	std::vector<rca::Group> const & mgroups, bool widest = false) 
{

	int Z = std::numeric_limits<int>::max ();
	int COST = 0;
	unsigned int HOP = 0;

	std::vector<rca::Link> residual;
	
	int croma = 0;
	for (auto const & tree  : ind.cromossoma) {		
		int tk = mgroups.at(croma).getTrequest ();
		std::vector<rca::Link> curr_tree;
		int curr_cost =  0;
		for (rca::Path p : tree.paths) {//test for hop size - max_path			
			if (p.size () > HOP) {
				HOP = p.size () - 1;
			}
			//iterating over edges of a path of the i-th multicast tree
			for (size_t i = 0; i < p.size ()-1; i++) {
				
				int v = p.at (i);
				int w = p.at (i+1);				
				rca::Link l (v, w, 0);

				//if the edge is not in the curr_tree
				auto res = std::find (curr_tree.begin (), curr_tree.end(), l);
				if (res == curr_tree.end()) {
					//add the cost
					curr_tree.push_back (l);
					COST += network.getCost (l);
					curr_cost += network.getCost (l);
					//update Z if edge l is not in residual
					auto res = std::find (residual.begin (), residual.end(), l);
					if (res == residual.end()) {
						int band = network.getBand (l.getX(), l.getY()); 
						l.setValue (band-tk);
						residual.push_back (l);
						if (widest) {
							network.setBand (l.getX(), l.getY(), l.getValue ());
							network.setBand (l.getY(), l.getX(), l.getValue ());
						}

						if (l.getValue () < Z) {
							Z = l.getValue ();
						}

					} else {
						int band = (int)res->getValue () - tk;
						res->setValue (band);
						if (res->getValue () < Z) {
							Z = res->getValue ();							
						}
					}
				}

			}
		}	

		ind.cost[croma] = curr_cost;
		croma++;
	}

	std::sort (residual.begin(), residual.end());
	ind.edges = std::move (residual);

	ind.objectives[0] = Z;
	ind.objectives[1] = COST;
	ind.objectives[2] = HOP;
}

bool dominate (
	std::vector<double> & obj1,
	std::vector<double> & obj2)
{

	int count = 0;
	if (obj1[0] > obj2[0]) {
		count++;
	}

	if (obj1[1] < obj2[1]) {
		count++;
	}

	if (obj1[2] < obj2[2]) {
		count++;
	}

	return (count > 0);
}

/**
* Este operador permite reorganizar os caminhos utilizando
* a raiz original da busca. Considerando que 'src' não é 
* a raiz original.
* A saída esperada é o conjunto dtree ordanado pelo src do grupo
* multicast 'i'
*
* @param int source do grupo
* @param int posição da fonte no grupo 'members' unido com 'sk'
* @param DTree estrutura que representa árvores como grupos
* @param vector<rca::Group> grupos multicast
* @param rca::Network rede do problema
*
*/
void reorganize (int src, 
	int pos, 
	int i,	
	DTree & dtree, 
	std::vector<rca::Group> const & mgroups,
	rca::Network const & network)
{

	if (src != mgroups[i].getSource ()) 
	{
		std::vector<int> in = std::vector<int> (network.getNumberNodes (),-1);

		rca::Path path (dtree.paths[dtree.paths.size ()-1]);
		for (size_t i=0; i < path.size ()-1; i++) {
			in[path[i]] = path[i+1];
		}

		for (size_t i = 0; i < dtree.paths.size ()-1; ++i)
		{				
			std::vector<int> newpath;
			rca::Path pp (dtree.paths[i]);
			
			int j = pp.size () -1;
			for (; j >= 0; --j)
			{
				if (in[pp[j]] != -1) {//encontra vértice
					j = pp[j]; //marca vertice em path raiz
					break;
				} else {
					newpath.push_back (pp[j]);
				}
			}
			
			size_t rp = path.getPosition (j);
			for (; rp < path.size (); rp++) {
				newpath.push_back (path[rp]);
			}
			rca::Path np(newpath);
			
			dtree.paths[i] = std::move (np);
		}
		dtree.paths.insert (dtree.paths.begin () + pos, path);
		dtree.paths.erase (dtree.paths.begin() + dtree.paths.size ());
		
	} else {
		for (auto & path : dtree.paths) {
			path.reverse ();
		}
	}

}


#endif // OPERATOR_H
