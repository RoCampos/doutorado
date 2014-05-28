#include "chenalgorithm.h"

using namespace std;

Chen::Chen (std::string _instance, std::string _input) {
	
	input = _input;
	instance = _instance;
	
}

void Chen::init () {
	
	Reader reader (instance);
	reader.configNetwork ( m_net.get () );
	
	
}

void Chen::pre_processing () {
	
	
}