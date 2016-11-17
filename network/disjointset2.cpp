#include "disjointset2.h"

using namespace std;

#define INVALID -1

DisjointSet2::DisjointSet2 (int size) 
: m_size (size)
{
	m_numberSets = 0;
	m_partition = vector<NodeDs*>(m_size);
	for(int i=0; i < m_size; i++) {
		make_set (i);
		m_numberSets++;
	}
}

DisjointSet2::DisjointSet2 () {
	
	m_numberSets = 0;
}

void DisjointSet2::init (int size) {
	
	
	m_size = size;
	m_partition = vector<NodeDs*>(m_size);
	for(int i=0; i < m_size; i++) {
		m_partition[i] = new NodeDs(-1);
	}
	
}

DisjointSet2::~DisjointSet2 ()
{
	for (int i = 0; i < m_partition.size (); ++i)
	{
		delete (m_partition[i]);
	}		
	m_partition.clear ();
}

void DisjointSet2::make_set (int item) {

	m_partition[item] = new NodeDs (item);
}

/**
 * Simple Find algorithm to find an 
 * a set in the Disjoint Set.
 * Returns a node that represets the set
 * related to item.
 * @author Romerito Campos
 */
NodeDs * DisjointSet2::find (int item) {
	
	NodeDs * ptr = m_partition[item];
	
	while (ptr->parent != NULL) {
		ptr = ptr->parent;
	}
	return ptr;
}

NodeDs * DisjointSet2::find2 (int item) 
{
	
	NodeDs *root = find (item);
	
	NodeDs *ptr = m_partition[item];
	//compress path
	
	while (ptr->parent != NULL) {
	
		NodeDs *tmp = ptr->parent;
		ptr->parent = root;
		ptr = tmp;
	}
	
	root = NULL;
	
	return ptr;
	
}

void DisjointSet2::addPartition (int item) {
	
	if (m_partition[item]->item == -1) {
		delete m_partition[item];
		m_partition[item] = NULL;
		m_partition[item] = new NodeDs (item);
		++m_numberSets;
	}
}

bool DisjointSet2::simpleUnion (int item1, int item2) {
	
	NodeDs * one = find (item1);
	NodeDs * two = find (item2);
	
	if (one != two) {
		one->parent = two;
		--m_numberSets;
		one = NULL;
		two = NULL;
		
		return true;
	}
	
	one = NULL;
	two = NULL;
	return false;
	
}

void DisjointSet2::print () {
	
	for(int i=0; i < (int)m_partition.size(); i++) {
		if (m_partition[i]->item != INVALID)
			cout <<*m_partition[i] <<endl;
	}
}

