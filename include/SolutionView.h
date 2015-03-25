#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>

#include <unistd.h>

#include "network.h"
#include "group.h"

namespace rca {
	
class TreeBuildViewer {

public:
	TreeBuildViewer (){};
	
	TreeBuildViewer (rca::Network *net) {
		m_network = net;
		
		m_last_window = -1;
	}
	
	bool addLink (rca::Link & l) {
		
		std::cout << __FUNCTION__ << std::endl;
		
		//teste se existe
		if ( !exists (l) ) {
			m_links.push_back (l);
			return true;
		}
		
		return false;
	}
	
	bool removedLink (rca::Link & l) {
		std::cout << __FUNCTION__ << std::endl;
		
		auto it = std::find (m_links.begin (), m_links.end(), l);
		if (it == m_links.end ()) {
			m_links.erase (it);
			return true;
		}
		
		return false;
	}
	
	void draw (std::string file) {
		std::cout << __FUNCTION__ << std::endl;
		
		std::ofstream out (file, std::ofstream::out);
		
		std::stringstream str;
		str << "Graph {\n";
		
		for (rca::Link &l: m_links) {
			str << l << " [style=bold, color=red];\n";
		}
		
		int NODES = m_network->getNumberNodes ();
		for (int i=0; i < NODES; i++) {
			for (int j=0; j < i; j++) {
				rca::Link aux (i,j,0);
				
				if (m_network->getCost (i,j) > 0)
				if ( !exists (aux)) {					
					str << aux << ";\n";
				}
				
			}
		}
		
		str << "}";
		
		//std::string out( str.str() );
		out << str.str ();
		
		out.close ();
		
		//std::cout << out << std::endl;		
		//getchar ();
		show (file);
	}
	
	void clear () {
		m_links.clear ();
	}
	
private:
	/*método que cria arquivo externo e faz chamada de sistema*/
	void show (std::string file) {
		
		std::string kill( "killall xdot");
		system ( kill.c_str ());
		
		std::string cmd ("xdot");
		cmd += " " + file +" & ";
		system ( cmd.c_str() );
		
		cmd = " sleep 1s && wmctrl -r 'teste - Dot Viewer' -e 0,0,0,1200,700";
		cmd += " && sleep 5s";
		system (cmd.c_str ());
		
	}
	
	bool exists (rca::Link &l) {
		//std::cout << __FUNCTION__ << std::endl;
		auto it = std::find (m_links.begin (), m_links.end(), l);
		return (it != m_links.end() ? true : false);
	}
	
	bool exists (const rca::Link &l) {
		//std::cout << __FUNCTION__ << std::endl;
		return exists (l);
	}
	
private:
	rca::Network * m_network;
	std::vector<rca::Link> m_links;
	
	int m_last_window;
	
};

}