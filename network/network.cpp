/*
 * network.cpp
 *
 *  Created on: 12/09/2012
 *      Author: romerito
 */

#include "network.h"
#include <cassert>

namespace rca {

Network::Network() : m_nodes(0), m_edges(0){

}

Network::Network(unsigned nodes, unsigned edges) {
	init (nodes,edges);
}

Network::~Network() {

}

Network * Network::extend (std::vector<int> const & sources) {

	int NODES = this->getNumberNodes ();
	int EDGES = this->getNumberEdges ();
	int MAXINT = std::numeric_limits<int>::max();

	Network * net = new Network(NODES+1, EDGES+sources.size ());

	for (auto link : this->m_links) {

		if (this->isRemoved (link)) {
			net->removeEdge(link);
			continue;
		}

		net->m_removeds_edge[link.getX()][link.getY()].removed = false;
		net->m_removeds_edge[link.getY()][link.getX()].removed = false;

		int cost = this->getCost (link.getX(), link.getY());
		int band = this->getBand (link.getX(), link.getY());
		net->setCost(link.getX(),link.getY(),cost);
		net->setCost(link.getY(),link.getX(),cost);
		
		net->setBand(link.getX(),link.getY(),band);
		net->setBand(link.getY(),link.getX(),band);

		net->addAdjacentVertex(link.getX(), link.getY());
		net->addAdjacentVertex(link.getY(), link.getX());

		net->m_links.insert (link);
	}

	for (auto s : sources) {
		rca::Link link (NODES, s, 0);

		net->setCost(link.getX(),link.getY(),0);
		net->setCost(link.getY(),link.getX(),0);		
		net->setBand(link.getX(),link.getY(),MAXINT);
		net->setBand(link.getY(),link.getX(),MAXINT);
		net->addAdjacentVertex(link.getX(), link.getY());
		net->addAdjacentVertex(link.getY(), link.getX());
	}

	// net->m_links = this->m_links;

	net->m_removeds = this->m_removeds;

	return net;
}

void Network::setCost(unsigned row, unsigned col, double value) {
	m_costMatrix.assign(row,col,value);
}

void Network::setBand(unsigned row, unsigned col, double value) {
	m_bandMatrix.assign(row,col,value);
}

bool Network::isRemoved(const Link& link) const{

	return m_removeds_edge[ link.getX() ][ link.getY() ].removed;
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
		
		m_removeds_edge[link.getX()][link.getY()].removed = false;
		m_removeds_edge[link.getX()][link.getY()].pos = -1;
		
		auto it = std::find (m_removeds.begin (), m_removeds.end(), link);
		m_removeds.erase ( it );
		
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

std::vector<int>
Network::breadth_first_search (int start) {

	std::vector<int> nodes(this->m_nodes);
	nodes[start] = 1;

	std::vector<int> stack;
	std::vector<int> pred (this->m_nodes, -2);
	stack.push_back (start);
	pred[start] = -1;

	do {

		int curr = *stack.begin();
		nodes [curr] = 1;

		for (auto && neighbor : m_adjacent_vertex[curr]) {
			
			if (nodes[neighbor] == 0) {
				stack.push_back (neighbor);
				pred[neighbor] = curr;
				nodes[neighbor] = 1;
			}
		}

		stack.erase (stack.begin ());

	} while ( !stack.empty () );

	return pred;

}


} /* namespace rca */
