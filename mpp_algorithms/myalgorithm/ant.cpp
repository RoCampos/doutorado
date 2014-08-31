#include "ant.h"

using namespace rca;

void Ant::move (int to) {

#ifdef DEBUG
	std::cout << "Doing move ()\n";
#endif 
	
	m_current_pos = to;
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