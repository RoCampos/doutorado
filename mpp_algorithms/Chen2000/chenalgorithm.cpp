#include "chenalgorithm.h"

using namespace std;

Chen::Chen (std::string _instance, std::string _input) {
	
	m_input = _input;
	m_instance = _instance;
	
	m_net = make_shared<Network> ();
	
}

void Chen::init () {
	
	Reader reader (m_instance);
	reader.configNetwork ( m_net.get () );
	m_groups = reader.readerGroup ();
	
}

void Chen::pre_processing () {
	
	int NODES = m_net->getNumberNodes ();
	m_edges = std::vector< std::vector<int>> (NODES);
	for (int i=0; i < NODES; i++) {
		m_edges[i] = std::vector<int> (NODES);
	}
	
	ifstream file_ (m_input.c_str ());
	
}