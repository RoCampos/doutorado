#ifndef MULTISOURCE_GROUP_H
#define MULTISOURCE_GROUP_H

namespace rca {
	namespace network {

typedef int source_t;
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
	
	int get_source_index (int source) {
		int idx = 0;
		for (int i : m_sources) {
			if (source == i) {
				return idx;
			}
			idx++;
		}
		return -1;
	}
	
	//stream id
	int m_id;
	
	int m_trequest;
	
	//list of source of the stream
	std::vector<source_t> m_sources;
	
	//destination of the stream
	rca::Group m_group;
	
	friend std::ostream & operator<< (std::ostream & out, stream_t const & t);
	
} stream_t;

	} // network
} // rca





#endif // MULTISOURCE_GROUP_H
