#ifndef _YUHCHEN_H_
#define _YUHCHEN_H_

#include <vector>
#include <memory>
#include <iostream>

#include "path.h"
#include "network.h"
#include "group.h"
#include "reader.h"
#include "steiner_tree_observer.h"
#include "edgecontainer.h"
#include "sttree_visitor.h"

typedef int source_t;

/**
 * Tree representation.
 * Contains a source and the paths used in the tree.
 * 
 */
typedef struct tree_t {

	std::vector<rca::Path> m_paths;
	int m_source;
	
} tree_t;

//typedef std::vector<tree_t> forest_t;

/**
 * This struct represents a stream w_k.
 * This stream can has a set of sources and
 * a set of destinations.
 * 
 * A stream can be viewed as group
 * But with more than one source.
 * 
 */
typedef struct stream_t {
	
	stream_t(){}
	
	stream_t (int id, int req, std::vector<source_t> source, rca::Group & g)
	: m_id (id), m_trequest (req)
	{
		m_sources = source;
		m_group = g;
	}
	
	//stream id
	int m_id;
	
	int m_trequest;
	
	//list of source of the stream
	std::vector<source_t> m_sources;
	
	//destination of the stream
	rca::Group m_group;
	
	friend std::ostream & operator<< (std::ostream & out, stream_t const & t);
	
}stream_t;


typedef struct forest_t {
	
	forest_t () : m_cost (0), Z(0){}
	
	//trees for the stream (group)
	std::vector<tree_t> m_trees;
	
	//identify the stream (group)
	int m_id;
	
	int m_cost;
	
	int Z;
	
} forest_t;


typedef rca::EdgeContainer<rca::Comparator, rca::HCell> CongestinoHandle;
typedef rca::SteinerTreeObserver<rca::EdgeContainer<rca::Comparator, rca::HCell>> STObserver;


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
	
	void configure_streams (stream_list & s);
	
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
	
	
private:
	
	rca::Network * m_network;
	std::vector<stream_t> m_streams;
};


#endif