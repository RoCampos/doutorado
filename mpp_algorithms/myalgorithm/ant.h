

#include <iostream>
#include <vector>
#include <ostream>
#include <algorithm>

#include "link.h"

namespace rca {

#ifndef _ANT_H_
#define _ANT_H_
/**
 * This class represents a Ant component
 * of ACO(Ant Colony Optimizatin). 
 * 
 * This components holds informations about
 * individual ant.
 * 
 * Besides, this class implements actions of
 * the ants.
 * 
 * @author Romerito Campos.
 * @date 29/08/2014 brasilian format
 */
class Ant {
	
public:
	Ant (int id) : m_id (id), m_current_pos(id) {
		m_nodes.push_back (m_id);
		mcpos = 0;
	};
	
	//method that makes the ant moving forward in the graph
	void move (int to);
	
	//method to join an ant to another in the search
	void join (const Ant & ant);
	
	//method to change the current_pos
	void back ();
	
	int get_current_position () const;
	
	int get_id () const;
	
	friend std::ostream& operator << (std::ostream & os, const Ant & ant);
	
	const std::vector<int>::const_iterator  nodes_begin () {
		return m_nodes.cbegin ();
	}
	
	const std::vector<int>::const_iterator nodes_end () {
		return m_nodes.cend ();
	}
	
private:
	//id of the node. The path start by the id	
	int m_id;
	
	//current position
	int m_current_pos;
	
	//index in m_nodes of the current_pos
	int mcpos;
	
	//nodes searched by m_id
	std::vector<int> m_nodes;
	
	//links in the solution
	std::vector<rca::Link> m_links;
	
	

	
};

#endif

}

