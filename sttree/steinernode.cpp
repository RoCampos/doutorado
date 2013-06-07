#include "steinernode.h"

SteinerNode::SteinerNode (int idx, int _value, bool _terminal) 
: index(idx), value(_value), terminal (_terminal), degree(0),leaf(false)
{
	firstAdjacent = -1;
	node = NULL;
	_edge = NULL;
}

void SteinerNode::increaseDegree ()
{
	degree++;
	leaf = (degree <=1 ? true : false);
}

void SteinerNode::decreaseDegree ()
{
	degree--;
	if (degree == 1) {
		leaf = true;
	}
}

void SteinerNode::setNode (Node* _node) 
{
	node = _node;
}

std::ostream & operator<< (std::ostream & out, const SteinerNode &st)
{
	out << "Index: "<< st.index << "\n";
	out << "degree: "<< st.degree << "\n";
	out << "value: "<< st.value << "\n";
	out << "leaf: "<< st.leaf << "\n";
	out << "terminal: "<< st.terminal << "\n";
	
	return out;
}