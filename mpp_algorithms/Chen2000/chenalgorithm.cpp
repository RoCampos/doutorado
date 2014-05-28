#include "chenalgorithm.h"

using namespace std;

Chen::Chen (std::string _instance, std::string _input) {
	
	input = _input;
	instance = _instance;
	
	m_net = make_shared<Network> ();
	
}

void Chen::init () {
	
	Reader reader (instance);
	reader.configNetwork ( m_net.get () );
	m_groups = reader.readerGroup ();
	
}

void Chen::pre_processing () {
	
	
}