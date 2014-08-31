

#include <iostream>
#include <vector>

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
	Ant (int id) : m_id (id){};
	
	//method that makes the ant moving forward in the graph
	void move (int to);
	
	//method to join an ant to another in the search
	void join (const Ant & ant);
	
	int get_current_position () const;
	
	int get_id () const;
	
	
private:
	//id of the node. The path start by the id	
	int m_id;
	
	//current position
	int m_current_pos;
	
	//nodes searched by m_id
	std::vector<int> m_nodes;
	
	//links in the solution
	std::vector<rca::Link> links;
	
	
};

#endif

}

