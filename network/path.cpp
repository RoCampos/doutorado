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

bool Path::revsubpath (int node, std::vector<int>& path) {
	for (int i = 0; i < m_path.size (); ++i)
	{
		if (m_path[i] != node) {
			path.push_back (m_path[i]);
		} else {
			return true;
		}
	}
	std::reverse (path.begin(), path.end());
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

int Path::getPosition (int node) {
	
	for (int i = 0; i < this->m_path.size (); ++i)
	{
		if (this->m_path[i] == node) {
			return i;
		}
	}

	return -1;

}

int Path::getRevPosition (int node) {

	int size = this->m_path.size ();
	for (int i = size-1; i >= 0; i--) {
		if (this->m_path[i] == node) {
			return size - i;
		}		
	}
	return -1;
}

} /* namespace rca */
