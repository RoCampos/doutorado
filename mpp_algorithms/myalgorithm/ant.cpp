#include "ant.h"

using namespace rca;

void Ant::move (int to) {

#ifdef DEBUG
	std::cout << "Doing move ()\n";
#endif 
	
	rca::Link link(m_current_pos, to, 0.0);
	m_current_pos = to;
	
	m_nodes.push_back (to); 
	m_links.push_back (link);
	
	mcpos = m_nodes.size()-1;
	
}

void Ant::join (const Ant & move) {

#ifdef DEBUG
	std::cout << "Doing join ()\n";
#endif
	
	auto it = move.m_nodes.begin ();
	for (; it != move.m_nodes.end(); it++) {
		
		if (std::find (m_nodes.begin(), m_nodes.end(), *it) == m_nodes.end()) {
			m_nodes.push_back (*it);
		}
	}
	
	m_current_pos = m_nodes[m_nodes.size()-1];
	
	//corrigindo a posição do para method back
	mcpos = m_nodes.size()-1;
	
	auto l = move.m_links.begin ();
	for (; l != move.m_links.end(); l++) {
		m_links.push_back (*l);
	}
	
}

int Ant::get_current_position () const {
	return m_current_pos;
}

int Ant::get_id () const {
	return m_id;
}

void Ant::back () {

	if (--mcpos < 0) {
		mcpos = m_nodes.size ()-1;
	}	
		
}

namespace rca {

std::ostream& operator << (std::ostream & os, const Ant & ant) 
{

	os << "ID: " << ant.get_id () << "\n";
	os << "Current Pos: " << ant.get_current_position () << "\n";
	for (unsigned i=0; i < ant.m_links.size (); i++) {
		os << ant.m_links[i] << std::endl;
	}
	os << "Nodes: ";
	for (unsigned i=0; i < ant.m_nodes.size (); i++) {
		os << ant.m_nodes[i] << " ";
	}
	
	return os;
}

}