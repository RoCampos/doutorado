/*
 * path.cpp
 *
 *  Created on: 13/09/2012
 *      Author: romerito
 */

#include "path.h"

namespace rca {

Path::Path() : m_cost(0.0){

}

Path::~Path() {

}

void Path::push(int vertex) {
	m_path.push_back (vertex);
}

bool Path::subpath (int vertex, std::vector<int> & path) 
{

	for (int i=m_path.size () - 1; i >= 0; i--) {
		if (m_path[i] != vertex) {
			path.push_back (m_path[i]);
		} else {
			return true;
		}
	}
	
	return false;
	
}

bool Path::operator==(const Path& path)
{
	if (path.getCost() != m_cost || 
		path.size () != size () )
	{
		return false;
	}
	
		
	for (int i=0; i < path.size (); i++) {
		if (path.at (i) != m_path[i++]) {
			return false;
		}
	}
	
	return true;
}

std::ostream& operator <<(std::ostream& os, const Path& path) {

	std::vector<int>::const_reverse_iterator it = path.m_path.rbegin();
	os << "[ ";
	for (; it != path.m_path.rend(); ++it ) {
		os << *it << " ";
	}
	os << "]";
	return os;

}

} /* namespace rca */
