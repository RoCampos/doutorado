/*
 * network.cpp
 *
 *  Created on: 12/09/2012
 *      Author: romerito
 */

#include "network.h"

namespace rca {

Network::Network() : m_nodes(0), m_edges(0){

}

Network::Network(unsigned nodes, unsigned edges) {
	init (nodes,edges);
}

Network::~Network() {

}

void Network::setCost(unsigned row, unsigned col, double value) {
	m_costMatrix.assign(row,col,value);
}

void Network::setBand(unsigned row, unsigned col, double value) {
	m_bandMatrix.assign(row,col,value);
}

void Network::removeEdge(const Link& link) {

	if (m_removeds_edge[link.getX()][link.getY()].removed == false) {
		m_removeds.push_back (link);
		
		int pos = m_removeds.size () - 1;
		
		m_removeds_edge[link.getX()][link.getY()].removed = true;
		m_removeds_edge[link.getX()][link.getY()].pos = pos;
	}
	
	/*
	if (!isRemoved (link))
		m_removeds.push_back (link);
	*/
}

void Network::undoRemoveEdge(const Link& link) {
	
	if (m_removeds_edge[link.getX()][link.getY()].removed == true) {
		
		int pos = m_removeds_edge[link.getX()][link.getY()].pos;
		
		m_removeds_edge[link.getX()][link.getY()].removed = true;
		m_removeds_edge[link.getX()][link.getY()].pos = -1;
				
		m_removeds.erase ( m_removeds.begin () + pos);
		
	}	
	
	/*
	std::vector<Link>::iterator it;
	it = std::find (m_removeds.begin(), m_removeds.end(), link);

	if (it != m_removeds.end())
		m_removeds.erase (it);
	*/
}

void Network::undoRemoveEdge(const std::set<Link>& toUndoRemove) {

	std::set<Link>::const_iterator it = toUndoRemove.begin();
	for (; it != toUndoRemove.end (); ++it) {
		undoRemoveEdge(*it);
	}

}

void Network::removeEdge(const std::set<Link>& toRemove) {
	std::set<Link>::const_iterator it = toRemove.begin();
	for (; it != toRemove.end (); ++it) {
		removeEdge(*it);
		if (!isConnected())
			undoRemoveEdge(*it);
	}
}

void Network::insertLink (const Link & link) {
	m_links.insert(link);

	m_edgesByNodes[link.getX()].push_back(link);
	m_edgesByNodes[link.getY()].push_back(link);
}

void Network::print() {

	for (int i = 0; i < m_nodes; ++i) {
		for (int j = 0; j < i; ++j) {
			if (m_costMatrix.at(i,j) > 0 && !isRemoved(Link(j,i,0)))
				cout << i<< "--"<<j <<" : "<< m_costMatrix.at(i,j) << endl;
		}
	}
}

void Network::init(unsigned nodes, unsigned edges) {
	m_nodes = nodes;
	m_edges = edges;
	m_costMatrix = matrix2d (m_nodes,m_nodes,0.0);
	m_bandMatrix = matrix2d (m_nodes,m_nodes,0.0);

	m_edgesByNodes = std::vector< std::vector<Link> > (m_nodes);
	
	m_vertex = std::vector<bool> (m_nodes,false);
	
	m_adjacent_vertex = std::vector<std::vector<int>>(m_nodes);
	
	m_removeds_edge = std::vector<std::vector<EdgeRemoved>> (m_nodes);
	for (int i = 0; i < m_nodes; i++) {
		m_removeds_edge[i] = std::vector<EdgeRemoved>(m_nodes);
	}
	
}

bool Network::isRemoved(const Link& link) const{

	//return std::find(m_removeds.begin(),m_removeds.end(),link) !=
	//		m_removeds.end();
	return m_removeds_edge[link.getX()][link.getY()].removed;
}

std::ostream& operator <<(std::ostream& os,
		const Network& network) {

	std::set <Link>::const_iterator its = network.m_links.begin();
	for (; its != network.m_links.end(); ++its) {
		if (!network.isRemoved(*its))
			os << *its << endl;
	}

	return os;
}

const std::vector<Link>& Network::getEdges(int node) const {

	if (node < 0 || node > (m_nodes-1)) {
		exit (1);
	}

	return m_edgesByNodes[node];
}

void Network::removeEdgeByNode(int node) {

	if (node < 0 || node > (m_nodes-1)) {
		exit (1);
	}

	for (unsigned i = 0; i < m_edgesByNodes[node].size (); ++i) {
		removeEdge(m_edgesByNodes[node][i]);
		if (!isConnected()) {
			undoRemoveEdge(m_edgesByNodes[node][i]);
		}
	}
}

void Network::undoRemoveEdgeByNode(int node) {

	if (node < 0 || node > (m_nodes-1)) {
		exit (1);
	}

	for (unsigned i = 0; i < m_edgesByNodes[node].size (); ++i) {
		undoRemoveEdge(m_edgesByNodes[node][i]);
	}
}

bool Network::isConnected() {

	DisjointSet2 disjointSet (m_nodes);

	std::set <Link>::iterator its = m_links.begin();
	while (its != m_links.end()) {

		if (disjointSet.find2 ((*its).getX()) != disjointSet.find2 ((*its).getY())
			&& !isRemoved (*its))
		{
			disjointSet.simpleUnion ((*its).getX(),(*its).getY());
		}

		++its;
	}

	return (disjointSet.getSize () == 1);
}

bool Network::isConnected2 () {
	
	DisjointSet2 disjointSet (m_nodes);

}

void Network::showRemovedEdges() {

	std::vector<Link>::const_iterator it = m_removeds.begin();
	for (; it != m_removeds.end(); ++it) {
		if (isRemoved(*it))
			cout << *it << endl;
	}

}

const int Network::get_adjacent_by_minimun_cost (const int & vertex)  {

	double min = std::numeric_limits<double>::max ();
	int adjacent = -1;
	
	auto it = m_adjacent_vertex[vertex].cbegin ();
	auto end = m_adjacent_vertex[vertex].cend ();
	for (; it != end; it++) {
		rca::Link link (vertex, *it, 0.0);
		
		if (!isRemoved(link)) {
			double value = getCost (vertex,*it);
			if ( value < min ) {
				min = value;
				adjacent = *it;
			}
		}
	}
	
	return adjacent;
}

const int 
Network::get_adjacent_by_minimun_cost 
					(const int & vertex, 
					 std::vector<rca::Link> & toRemove) 
{
	
	double min = std::numeric_limits<double>::max ();
	int adjacent = -1;
	
	auto it = m_adjacent_vertex[vertex].cbegin ();
	auto end = m_adjacent_vertex[vertex].cend ();
	for (; it != end; it++) {
		rca::Link link (vertex, *it, 0.0);
		
		auto element = std::find(toRemove.begin(), toRemove.end(), link);
		
		if ( element == toRemove.end() ) {
			double value = getCost (vertex,*it);
			if ( value < min ) {
				min = value;
				adjacent = *it;
			}
		}
	}
	
	return adjacent;
	
}

} /* namespace rca */
