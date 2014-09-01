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
	
}

void Ant::join (const Ant & move) {

#ifdef DEBUG
	std::cout << "Doing join ()\n";
#endif
	
}

int Ant::get_current_position () const {
	return m_current_pos;
}

int Ant::get_id () const {
	return m_id;
}

namespace rca {

std::ostream& operator << (std::ostream & os, const Ant & ant) 
{

	os << "ID: " << ant.get_id () << "\n";
	os << "Current Pos: " << ant.get_current_position () << "\n";
	
	return os;
}

}