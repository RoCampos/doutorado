#include <iostream>
#include <stdio.h>

#include "reader.h"
#include "network.h"
#include "algorithm.h"
#include "steiner_tree_observer.h"
#include "edgecontainer.h"
#include "rcatime.h"
#include "mpp_visitor.h"

#include "config.h"

#include "sttree_local_search.h"

using namespace rca;

typedef std::vector<int> previous_t;
typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> CongestionHandle;
typedef typename rca::sttalgo::SteinerTreeObserver<CongestionHandle, STTree> STobserver;
typedef typename rca::sttalgo::ChenReplaceVisitor<> ChenReplaceVisitor;

typedef rca::sttalgo::cycle_local_search<CongestionHandle> CycleLocalSearch;

typedef std::tuple<int,int> Result;

previous_t shortest_path2 (int v, int w, rca::Network & network);

rca::Path get_path (int v, int w, rca::Network & network, previous_t & prev){	
	int NODES = network.getNumberNodes ();	
	rca::Path path;
	int pos = 0;
	double pathcost = 0.0;
	int s = w;
	while (s != v) {
		path.push (s);
		s = prev[s];		
		if (s == -1 || s >= NODES){
			rca::Path path2;
			return path2; //se não há caminho
		}
		pathcost += (int)network.getCost (path[pos],s);		
		pos++;
	}
	path.push (s);
	return path;
}

void remove_top_edges (
	CongestionHandle & ob,
	Network & m_network, 
	Group & group, 
	int res) {
	
	auto it = ob.get_heap ().ordered_begin ();
	auto end = ob.get_heap ().ordered_end ();
	
	for ( ; it != end; it++) { 		
		m_network.removeEdge (*it);
		if (!is_connected (m_network, group)) {
			m_network.undoRemoveEdge (*it);
		} 	
	}	
}

void intensification (std::vector<STTree> & steiner_trees, 
					  CongestionHandle & cg,
					  Network& m_network, 
					  std::vector<Group>& m_groups,
					  Result & result)
{
	
	int cost = 0;
	int res = 0;
 	std::tie(res, cost) = result;
	
	ChenReplaceVisitor chen(&steiner_trees);	
	chen.setEdgeContainer (cg);
	chen.setMulticastGroups (m_groups);
	chen.setNetwork (&m_network);
	
	double c = cost;
	chen.visitByCost (c);

	cost = c;
	CycleLocalSearch cls;
	cls.local_search (steiner_trees, m_network, 
					m_groups, cg, cost);
	
	res = cg.top ();
	
	result = std::make_tuple<int,int> ((int)res, (int)cost);
}

std::string commandLine () {

	std::string str;
	str = "--inst ['bite'] --res ['int'] --localsearch ['yes'|'no']";
	str += " \n\t--reverse ['yes'|'no']";
	str += " \n\t--result ['full','res','cost',complete]";

	return str;
}

int main (int argc, char const *argv[]) 
{
	
	if (message (argc, argv, commandLine ())) {
		exit (1);
	}

	rca::Network m_network;
	std::vector<std::shared_ptr<rca::Group>> g;
	std::vector<rca::Group> m_groups;
	std::string file(argv[2]);
	
	int res = atoi(argv[4]);
	std::string localsearch = (argv[6]);
	std::string reverse = (argv[8]);
	std::string param = (argv[10]);
	std::string print = (argv[12]);
	
	rca::reader::MultipleMulticastReader reader(file);

#ifdef MODEL_REAL
	reader.configure_real_values (&m_network,g);
#endif
	
#ifdef MODEL_UNIT
	reader.configure_unit_values (&m_network,g);
#endif

	for (std::shared_ptr<rca::Group> i : g) {
		m_groups.push_back (*i);
	}
	
	int NODES = m_network.getNumberNodes ();
	
	CongestionHandle cg(NODES);
	
	STobserver ob;
	ob.set_container (cg);
	
	std::vector<STTree> steiner_trees;
	double cost = 0.0;
 	
	rca::elapsed_time _time_;	
	_time_.started ();

	if (reverse.compare ("yes") == 0) {
		if (param.compare ("request") == 0) {
			std::sort (m_groups.begin(), m_groups.end(), rca::CompareGreaterGroup());
		} else {
			std::sort (m_groups.begin(), m_groups.end(), rca::CompareGreaterGroupBySize());
		}
	} else {
		if (param.compare ("request") == 0) {
			std::sort (m_groups.begin(), m_groups.end(), rca::CompareLessGroup());	
		} else {
			std::sort (m_groups.begin(), m_groups.end(), rca::CompareLessGroupBySize());
		}
	}
	
	for (Group g : m_groups) {
				
		int source = g.getSource ();
		std::vector<int> members = g.getMembers();
		
		STTree steiner_tree(NODES, source, g.getMembers ());		
		ob.set_steiner_tree (steiner_tree, NODES);
		
		remove_top_edges (cg, m_network, g, res);
		
		previous_t prev = all_shortest_path (source, members[0] ,m_network);
		
		int trequest = g.getTrequest ();
		for (int m : members) {
			rca::Path path = get_path (source, m, m_network, prev);			
			auto rit = path.rbegin ();
			for (; rit != path.rend()-1; rit++) {
		
				int x = *rit;
				int y = *(rit+1);
				
				rca::Link l(x, y, 0);				
				int cost = m_network.getCost (l.getX(), l.getY());				
				int band = m_network.getBand (l.getX(), l.getY());
				ob.add_edge (l.getX(), l.getY(), cost, trequest, band);
			}			
		}
		
		m_network.clearRemovedEdges ();

		steiner_trees.push_back (ob.get_steiner_tree());		
		cost += ob.get_steiner_tree ().get_cost ();
	}
 	_time_.finished ();

	Result result( std::make_tuple<int,int>(cg.top(), (int)cost) );
	
	if (localsearch.compare ("yes") == 0)
		intensification (steiner_trees, cg, m_network, m_groups, result);	
	
	_time_.finished ();
	
	if (print.compare ("complete") == 0) {		
		std::vector<STTree> saida = std::vector<STTree> (steiner_trees.size());
		for (size_t i = 0; i < steiner_trees.size(); ++i)
		{
			saida[m_groups[i].getId()] = steiner_trees[i];
		}
		rca::sttalgo::print_solution2<STTree> (saida);

		std::cout << get<0>(result) << " " << get<1>(result);
		std::cout << " " << _time_.get_elapsed ()<< " \n";
	} else if (print.compare ("full")) {
		std::cout << get<0>(result) << " " << get<1>(result);
		std::cout << " " << _time_.get_elapsed ()<< " \n";	
	} else if (print.compare ("res")) {
		cout << get<0> (result) << endl;
	} else if (print.compare ("cost")) {
		cout << get<1> (result) << endl;
	}

	
	return 0;
}