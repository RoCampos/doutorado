
#include <iostream>
#include <stdio.h>

#include "reader.h"
#include "network.h"
#include "algorithm.h"
#include "steiner_tree_observer.h"
#include "edgecontainer.h"
#include "rcatime.h"

#include "config.h"

#include "mpp_visitor.h"
#include "sttree_local_search.h"

using namespace rca;

typedef std::vector<int> previous_t;
typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> CongestionHandle;
typedef typename rca::sttalgo::SteinerTreeObserver<CongestionHandle, STTree> STobserver;

typedef rca::sttalgo::ChenReplaceVisitor<> ChenReplace;
typedef rca::sttalgo::cycle_local_search<CongestionHandle> CycleLocalSearch;

typedef std::tuple<int,int> Result;

void agm_heuristic (STobserver * ob, std::vector<rca::Link>&, 
					Network & m_network,int);

void create_list (std::vector<rca::Link>&, Network & m_network);

void update_usage (std::vector<rca::Link>&, STTree & st, int trequest);

void intensification (std::vector<STTree>&, 
					  CongestionHandle&, 
					  Network&, 
					  std::vector<Group>&,
					  Result &);

std::string commandLine () {
	std::string str;
	str = "--inst ['brite'] --res ['int'] --localsearch ['yes'|'no']";
	str += " \n\t--reverse ['yes'|'no']";
	str += " \n\t--result ['full','res','cost',complete]";
	return str;
}

int main (int argc, char const *argv[]) 
{

	if (message(argc, argv, commandLine())) {
		exit (1);
	}

	rca::Network m_network;
	std::vector<std::shared_ptr<rca::Group>> g;
	std::vector<rca::Group> m_groups;
	std::string file(argv[2]);	
	int res = atoi(argv[4]);	
	std::string  localsearch = (argv[6]);
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
	int GROUP_SIZE = m_groups.size ();
	
	CongestionHandle cg(NODES);
	
	STobserver ob;
	ob.set_container (cg);
	ob.set_network (m_network);
	
	std::vector<STTree> steiner_trees;
	double cost = 0.0;
 	
	std::vector<rca::Link> m_links;
	create_list (m_links, m_network); //from visitor
	
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
		int trequest = g.getTrequest ();
		STTree steiner_tree(NODES, source, g.getMembers ());
		
		ob.set_steiner_tree (steiner_tree, NODES);
		
		agm_heuristic (&ob, m_links, m_network, trequest);
		
		ob.prune (trequest, GROUP_SIZE);
		steiner_trees.push_back (ob.get_steiner_tree ());
		
		cost += ob.get_steiner_tree ().get_cost ();
		
		update_usage (m_links, ob.get_steiner_tree (), trequest);
	}

	Result result( std::make_tuple<int,int>(cg.top(), (int)cost) );
	
	if (localsearch.compare ("yes") == 0) {
		intensification (steiner_trees, cg, m_network, m_groups, result);	
	}
	
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

void update_usage (
	std::vector<rca::Link>& m_links, 
	STTree & sttree, int trequest)
{
	edge_t * e = sttree.get_edges ().begin;
	while (e != NULL) {	
		if (e->in) { 		
			rca::Link l(e->x, e->y,0);
			auto link = std::find(m_links.begin (), m_links.end(), l);
			link->setValue ( link->getValue () + trequest);
		}		
		e = e->next;
	}
	
	std::sort(m_links.begin(), m_links.end());	
}

void agm_heuristic (STobserver * ob, 
					std::vector<rca::Link>& m_links,
					Network & m_network, 
					int trequest)
{
	std::vector<rca::Link> links = m_links;
		
	int pos = 0;
	//O(E)
	while (!links.empty()) {		
		rca::Link link = links[pos];						
		int cost = m_network.getCost (link.getX(), link.getY());
		int band = m_network.getBand (link.getX(), link.getY());			
		if (m_network.isRemoved(link)){
			links.erase ( (links.begin () + pos) );
			continue;
		}		
		ob->add_edge (link.getX(), link.getY(), cost, trequest, band);
		links.erase ( (links.begin () + pos) );
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
	
	ChenReplace chen(&steiner_trees);	
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
