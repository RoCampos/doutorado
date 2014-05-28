#include "chenalgorithm.h"

using namespace std;

Chen::Chen (std::string _instance, std::string _input) {
	
	m_input = _input;
	m_instance = _instance;
	
	m_net = make_shared<Network> ();	
	init ();
	
}

void Chen::init () {
	
	Reader reader (m_instance);
	reader.configNetwork ( m_net.get () );
	m_groups = reader.readerGroup ();
	
}

void Chen::pre_processing () {
	
	int NODES = m_net->getNumberNodes ();
	
	int edges[NODES][NODES];
	
	ifstream file_ (m_input.c_str ());
	
	int x;
	int y;
	string str;
	while (file_.good ()) {
		file_ >> y;
		file_ >> str;
		file_ >> str;
		str = str.substr (0, str.length () - 2);
		x = atoi ( str.c_str () );
		
		//cout << --x << " " << --y << endl;
		edges[y][x] += 1;
		edges[x][y] += 1;
	}
	
	file_.close ();
}