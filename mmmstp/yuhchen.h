#ifndef _YUHCHEN_H_
#define _YUHCHEN_H_

#include "path.h"

typedef int source_t;

/**
 * Tree representation.
 * Contains a source and the paths used in the tree.
 * 
 */
typedef struct tree_t {

	std::vector<rca::Path> m_paths;
	int m_source;
	
};

typedef std::vector<tree_t> forest_t;

/**
 * This struct represents a stream w_k.
 * This stream can has a set of sources and
 * a set of destinations.
 * 
 */
typedef struct stream_t {
	
	std::vector<source_t> m_sources;
	rca::Group m_group;
		
}
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