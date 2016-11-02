#ifndef _YUHCHEN_H_
#define _YUHCHEN_H_

#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>

#include "config.h"

#include "path.h"
#include "network.h"
#include "group.h"
#include "multisource_group.h"
#include "reader.h"
#include "steiner_tree_observer.h"
#include "edgecontainer.h"
#include "sttree_visitor.h"
#include "mpp_visitor.h"

#include "rcatime.h"
#include "sttree_local_search.h"

using rca::network::source_t;
using rca::network::stream_t;

/**
 * Tree representation.
 * Contains a source and the paths used in the tree.
 * 
 */
typedef struct tree_t {

	std::vector<rca::Path> m_paths;
	STTree m_tree;
	int m_source;
	
	rca::Path find_path (int destination) {
		auto it = m_paths.begin ();
		for ( ; it != m_paths.end(); it++) {
			
			if ( (*it)[0] == destination )
				return *it;
		}
		rca::Path p;
		return p;
	}
	
} tree_t;

//typedef std::vector<tree_t> forest_t;


typedef struct forest_t {
	
	forest_t () : m_cost (0), Z(0){}
	
	forest_t (forest_t const & t) 
	{ 
		m_trees = t.m_trees;
		m_id = t.m_id;
		m_cost = t.m_cost;
		Z = t.Z;
	}
	
	//trees for the stream (group)
	std::vector<tree_t> m_trees;
	
	//identify the stream (group)
	int m_id;
	
	int m_cost;
	
	int Z;
	
} forest_t;


typedef rca::EdgeContainer<rca::Comparator, rca::HCell> CongestinoHandle;
typedef rca::sttalgo::SteinerTreeObserver<rca::EdgeContainer<rca::Comparator, rca::HCell>, STTree> STObserver;
typedef typename rca::sttalgo::SteinerTreeObserver<CongestinoHandle, STTree> STObserver;
typedef rca::sttalgo::cycle_local_search<CongestionHandle> CycleLocalSearch;


/**
 * This class contais the variables and 
 * algorithms used in Yuh-Chen paper.
 * 
 * 
 * @data 04/20/2015
 */
class YuhChen {

public:
	YuhChen ();
	
	YuhChen (rca::Network *);
	
	YuhChen (std::string file);
	
	/**
	 * Este método é utilizado para configurar os streams(grupos).
	 * Ele é utilizdo no construtor que recebe o nome do arquivo.
	 * @stream_list é defindo em reader.h 
	 */
	void configure_streams (rca::reader::stream_list_t & s);
	
	void configure_streams (std::vector<rca::Group> &);
	
	/*
	 * This method set the stream w: a list of sources and
	 * a set of destinations.
	 * 
	 */
	void add_stream(int id,int req, std::vector<source_t> S, rca::Group D);
	
	/**
	 * 
	 * 
	 */
	stream_t & get_stream (int id) {
		if (id < (int)m_streams.size ()) {
			return m_streams[id];
		}
	}
	
	
	/**
	 * This method build a tree using dijkstra's algorithm
	 * adapted to find the maximum bottleneck path.
	 * 
	 * This method is diffent of the definition in the paper.
	 * Here we get all paths possible in the stream_id at once.
	 */
	forest_t widest_path_tree (int stream_id);
	
	/**
	 * This method returns a forest associated to the stream
	 * argument.
	 * This forrents contains dijoint tree.
	 * Each tree has a source and send informations to a set
	 * destination
	 */
	forest_t wp_forest (stream_t & stream);
	
	void run (int improve_cost = 0, std::string result = "");
	
private:
	/**
	 * This method gets the link with minimum bandwidth in a path
	 * @return rca::Link
	 */
	rca::Link get_bottleneck_link (rca::Path &);
	
	/**
	 * This method gets the next node in a path after current_node
	 */
	int next_node (rca::Path & path, int current_node);
	
	/**
	 * This method gets a forest from a set of paths
	 */
	forest_t to_forest (int stream_id, std::vector<rca::Path>);
	
	
	void update_usage (STTree & st, int trequest);
	
	/**
	 * This method is used to update the congestion handle.
	 * The congestioon handle will be used to improve the cost
	 * of a solution.
	 */
	int update_cg (STTree & st, int trequest, rca::Network & copy);
	
private:
	
	rca::Network * m_network;
	
	std::vector<stream_t> m_streams;
	
	CongestinoHandle * m_cg;
	
	int m_improve_cost;
	
};


#endif