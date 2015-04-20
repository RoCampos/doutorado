#ifndef _YUHCHEN_H_
#define _YUHCHEN_H_

#include <vector>
#include <memory>
#include <iostream>

#include "path.h"
#include "network.h"
#include "group.h"
#include "reader.h"

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

typedef std::vector<tree_t> forest_t;

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
	
	int m_id;
	std::vector<source_t> m_sources;
	rca::Group m_group;
	
	friend std::ostream & operator<< (std::ostream & out, stream_t const & t);
	
}stream_t;
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
	
	
	/*
	 * This method set the stream w: a list of sources and
	 * a set of destinations.
	 * 
	 */
	void set_stream(int id, std::vector<source_t> S, rca::Group D);
	
	/**
	 * 
	 * 
	 */
	stream_t & get_stream (int id) {
		if (id < m_streams.size ()) {
			return m_streams[id];
		}
	}
	
	
	/**
	 * This method build a tree using dijkstra's algorithm
	 * adapted to find the maximum bottleneck path
	 */
	tree_t widest_path_tree (source_t & source);
	
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